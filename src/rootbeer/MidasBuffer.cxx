/// \file MidasBuffer.cxx
/// \author G. Christian
/// \brief Implements MidasBuffer.hxx
#include <cassert>
#include "rootbeer/Timestamp.hxx"
#include "utils/definitions.h"
#include "midas/Database.hxx"
#include "midas/Event.hxx"
#include "DragonEvents.hxx"
#include "MidasBuffer.hxx"


namespace {

const int MIDAS_BOR  = 0x8000;
const int MIDAS_EOR  = 0x8001;
const int FLUSH_TIME = 60;
const double QUEUE_TIME = 4e6;
const double COINC_WINDOW = 10.;

rootbeer::TSQueue gQueue (QUEUE_TIME);

}

namespace {
template <typename T>
inline void Malloc(T*& buf, size_t size)
{
	buf = static_cast<T*>(malloc(size));
	assert(buf);
} 
inline void reset_scalers()
{
	rb::Event::Instance<rootbeer::HeadScaler>()->Reset();
	rb::Event::Instance<rootbeer::TailScaler>()->Reset();
	rb::Event::Instance<rootbeer::RunParameters>()->Reset();
	rb::Event::Instance<rootbeer::TStampDiagnostics>()->Reset();
} }

rootbeer::MidasBuffer::MidasBuffer(size_t size):
	fBufferSize(size),
	fIsTruncated(false),
	fType(MidasBuffer::NONE)
{
	/*!
	 * \param size Size of the internal buffer in bytes. This should be larger than the
	 * biggest expected event.
	 */
	Malloc(fBuffer, size);
}

rootbeer::MidasBuffer::~MidasBuffer()
{
	free(fBuffer);
}

Bool_t rootbeer::MidasBuffer::ReadBufferOffline()
{
	/*!
	 * Reads event data into fBuffer
	 * \todo Could be made more efficient (no copy)??
	 */
	TMidasEvent temp;
	Bool_t have_event = fFile.Read(&temp);

	if(have_event) {
		memcpy (fBuffer, temp.GetEventHeader(), sizeof(midas::Event::Header));
		memcpy (fBuffer + sizeof(midas::Event::Header), temp.GetData(), temp.GetDataSize());

		if ( temp.GetDataSize() + sizeof(midas::Event::Header) > fBufferSize ) {
			utils::err::Warning("rootbeer::MidasBuffer::ReadBufferOffline")
				<< "Received a truncated event: event size = "
				<< ( temp.GetDataSize() + sizeof(midas::Event::Header) )
				<< ", max size = " << fBufferSize << " (Id, serial = "
				<< temp.GetEventId() << ", " << temp.GetSerialNumber() << ")";
			fIsTruncated = true;
		}
	}

	return have_event;
}

Bool_t rootbeer::MidasBuffer::UnpackBuffer()
{
	/*!
	 * Figures out the type of MIDAS event buffer received and sends onto the
	 * appropriate event handler (see below):
	 */
	const bool coincMatch = true;
	midas::Event::Header* evtHeader = reinterpret_cast<midas::Event::Header*>(fBuffer);

	switch (evtHeader->fEventId) {
	case DRAGON_HEAD_EVENT:  /// - DRAGON_HEAD_EVENT: Insert into timestamp matching queue
		{
			const char* const tscbank = rb::Event::Instance<rootbeer::GammaEvent>()->TscBank();
			midas::Event event(fBuffer, evtHeader->fDataSize, tscbank, COINC_WINDOW);
			if(coincMatch) 
				gQueue.Push(event, rb::Event::Instance<rootbeer::TStampDiagnostics>()->GetDiagnostics());
			else
				rb::Event::Instance<rootbeer::GammaEvent>()->Process(&event, 0);
			break;
		}
	case DRAGON_TAIL_EVENT:  /// - DRAGON_TAIL_EVENT: Insert into timestamp matching queue
		{
			const char* const tscbank = rb::Event::Instance<rootbeer::HeavyIonEvent>()->TscBank();
			midas::Event event(fBuffer, evtHeader->fDataSize, tscbank, COINC_WINDOW);
			if(coincMatch)
				gQueue.Push(event, rb::Event::Instance<rootbeer::TStampDiagnostics>()->GetDiagnostics());
			else
				rb::Event::Instance<rootbeer::HeavyIonEvent>()->Process(&event, 0);
			break;
		}
	case DRAGON_HEAD_SCALER: /// - DRAGON_HEAD_SCALER: Unpack event
		{
			midas::Event event(fBuffer, evtHeader->fDataSize, 0);
			rb::Event::Instance<rootbeer::HeadScaler>()->Process(&event, 0);
			break;
		}
	case DRAGON_TAIL_SCALER: /// - DRAGON_TAIL_SCALER: Unpack event
		{
			midas::Event event(fBuffer, evtHeader->fDataSize, 0);
			rb::Event::Instance<rootbeer::TailScaler>()->Process(&event, 0);
			break;
		}
	case MIDAS_BOR: ///  - Begin-of-run: Ignore
		{
			char *pxml = fBuffer + sizeof(midas::Event::Header);
			midas::Database db(pxml, evtHeader->fDataSize);
			rb::Event::Instance<rootbeer::RunParameters>()->Process(&db, 0);
			break;
		}
	case MIDAS_EOR: ///  - End-of-run: read global parameters from the ODB
		{
			char *pxml = fBuffer + sizeof(midas::Event::Header);
			midas::Database db(pxml, evtHeader->fDataSize);
			rb::Event::Instance<rootbeer::RunParameters>()->Process(&db, 0);
			break;
		}
	default:        ///  - Warn about unknown event types
		{
			utils::err::Warning("UnpackBuffer") << "Unkonwn event ID: " << evtHeader->fEventId;
			break;
		}
	}

	return true;
}

#ifdef MIDASSYS

#define M_ONLINE_BAIL_OUT cm_disconnect_experiment(); return false

Bool_t rootbeer::MidasBuffer::ConnectOnline(const char* host, const char* experiment, char**, int)
{
	/*!
	 * \param host hostname:port where the experiment is running (e.g. ladd06:7071)
	 * \param experiment Experiment name on \e host (e.g. "dragon")
	 *
	 * See list below for what's specifically handled by this function.
	 */
	INT status;
	char syncbuf[] = "SYSTEM";

	/// - Connect to MIDAS experiment
	status = cm_connect_experiment (host, experiment, "rootbeer", NULL);
	if (status != CM_SUCCESS) {
		utils::err::Error("rootbeer::MidasBuffer::ConnectOnline")
			<< "Couldn't connect to experiment \"" << experiment << "\" on host \""
			<<  host << "\", status = " << status << DRAGON_ERR_FILE_LINE;
		return false;
	}
	utils::err::Info("rootbeer::MidasBuffer::ConnectOnline")
		<< "Connected to experiment \"" << experiment << "\" on host \"" << host;

	/// - Connect to "SYNC" shared memory buffer
  status = bm_open_buffer(syncbuf, 2*MAX_EVENT_SIZE, &fBufferHandle);
	if (status != CM_SUCCESS) {
		utils::err::Error("rootbeer::MidasBuffer::ConnectOnline")
			<< "Error opening \"" << syncbuf << "\" shared memory buffer, status = "
			<< status << DRAGON_ERR_FILE_LINE;
		M_ONLINE_BAIL_OUT;
	}

	/// - Request (nonblocking) all types of events from the "SYNC" buffer
	status = bm_request_event(fBufferHandle, -1, -1, GET_NONBLOCKING, &fRequestId, NULL);
	if (status != CM_SUCCESS) {
		utils::err::Error("rootbeer::MidasBuffer::ConnectOnline")
			<< "Error requesting events from \"" << syncbuf << "\", status = "
			<< status << DRAGON_ERR_FILE_LINE;
		M_ONLINE_BAIL_OUT;
	}

	/// - Reset scalers and diagnostics
	reset_scalers();

	/// - Register transition handlers
	/// \note Stop transition needs to have a 'late' (>700) priority to receive
	///  events flushed from the "SYNC" buffer at the end of the run
	cm_register_transition (TR_STOP,   rootbeer_run_stop,   750);
	cm_register_transition (TR_START,  rootbeer_run_start,  500);
	cm_register_transition (TR_PAUSE,  rootbeer_run_pause,  500);
	cm_register_transition (TR_RESUME, rootbeer_run_resume, 500);

	/// - Update variables from ODB
	this->ReadVariables("online");

	fType = MidasBuffer::ONLINE;
	return true;
}

Bool_t rootbeer::MidasBuffer::OpenFile(const char* file_name, char** other, int nother)
{
	/*!
	 * Reset scalers, diagnostics, open MIDAS file w/ TMidasFile::Open(),
	 * read ODB values from the file.
	 */
	reset_scalers();
	fType = MidasBuffer::OFFLINE;
	bool status = fFile.Open(file_name);
	if (status == true) ReadVariables(file_name);
	return status;
}

void rootbeer::MidasBuffer::DisconnectOnline()
{
	/*! Calls cm_disconnect_experiment() and flushes queue */
	cm_disconnect_experiment();
	gQueue.Flush(FLUSH_TIME, rb::Event::Instance<rootbeer::TStampDiagnostics>()->GetDiagnostics());
	utils::err::Info("rootbeer::MidasBuffer::DisconnectOnline")
		<< "Disconnecting from experiment";
	fType = MidasBuffer::NONE;
}

void rootbeer::MidasBuffer::CloseFile()
{
	/*! Close file, flush queue */
	utils::err::Info("rootbeer::MidasBuffer::CloseFile")
		<< "Closing MIDAS file: \"" << fFile.GetFilename() << "\"";
	fFile.Close();
	gQueue.Flush(FLUSH_TIME, rb::Event::Instance<rootbeer::TStampDiagnostics>()->GetDiagnostics());
	fType = MidasBuffer::NONE;
}


void rootbeer::MidasBuffer::ReadVariables(const char* dbname)
{
	std::string dbMessage = dbname;
	if (dbMessage == "online")
		dbMessage = "the online database";
	else if (dbMessage.rfind("/") < dbMessage.size())
		dbMessage = dbMessage.substr(dbMessage.rfind("/")+1);
	else
		;
	utils::err::Info("rootbeer::MidasBuffer")
		<< "Synching variable values with " << dbMessage;

	rb::Event::Instance<rootbeer::GammaEvent>()->ReadVariables(dbname);
	rb::Event::Instance<rootbeer::HeavyIonEvent>()->ReadVariables(dbname);
	rb::Event::Instance<rootbeer::CoincEvent>()->ReadVariables(dbname);
	rb::Event::Instance<rootbeer::HeadScaler>()->ReadVariables(dbname);
	rb::Event::Instance<rootbeer::TailScaler>()->ReadVariables(dbname);
}

Bool_t rootbeer::MidasBuffer::ReadBufferOnline()
{
	/*!
	 * Uses bm_receive_event to directly receive events from "SYNC" shared memory. The function
	 * requests events in a loop until it either gets one or receives a signal to exit.
	 *
	 * See the list below for what is done in the request loop.
	 */
	bool have_event = false;
	const int timeout = 10;
	INT size, status;
	do {
		size = fBufferSize;

		/// - Check status of client w/ cm_yield()
		status = cm_yield(timeout);

		/// - Then check for an event
		if (status != RPC_SHUTDOWN) status = bm_receive_event (fBufferHandle, fBuffer, &size, ASYNC);

		/// - If we have an event (full or partial), return to outer loop (rb::attach::Online)
		if (status == BM_SUCCESS || status == BM_TRUNCATED) have_event = true;

		/// - Exit loop if any of the following are true:
		///     - We have an event
		///     - We receive SS_ABORT from bm_receive_event()
		///     - We receive RPC_SHUTDOWN from cm_yield [odb shutdown signal]
		///     - We have requested events using an invalid buffer handle
		///     - We receive an Unattach() signal from ROOTBEER
	} while ( !have_event &&
						status != SS_ABORT &&
						status != RPC_SHUTDOWN && 
						status != BM_INVALID_HANDLE &&
						rb::Thread::IsRunning(rb::attach::ONLINE_THREAD_NAME) );

	/// - Print a warning message if the event was truncated
	if (status == BM_TRUNCATED) {
		utils::err::Warning("rootbeer::MidasBuffer::ReadBufferOnline")
			<< "Received a truncated event: event size = "
			<< ( reinterpret_cast<midas::Event::Header*>(fBuffer)->fDataSize + sizeof(midas::Event::Header) )
			<< ", max size = " << fBufferSize;
		fIsTruncated = true;
	}

	/// - Print an error message if the buffer handle was invalid
	if (status == BM_INVALID_HANDLE) {
		utils::err::Error("rootbeer::MidasBuffer::ReadBufferOnline") << "Invalid buffer handle: " << fBufferHandle;
	}

	if(!have_event && rb::Thread::IsRunning(rb::attach::ONLINE_THREAD_NAME)) {
		utils::err::Info("rootbeer::MidasBuffer::ReadBufferOnline")
			<< "Received external command to shut down: status = " << status;
	}
	
	/// \returns true if we received an event (full or partial), false otherwise 
	return have_event;
	/// \todo Figure out best cm_yield timeout argument
	/// \todo Optimize buffer size (could probably get away w/ smaller?)
}

#else

#define M_NO_MIDASSYS (FUNC) do {																				\
		utils::err::Error(FUNC) <<																					\
			"Online functionality requires MIDAS installed on your system"		\
														 << DRAGON_ERR_FILE_LINE; }									\
	while (0)

Bool_t rootbeer::MidasBuffer::ConnectOnline(const char* host, const char* experiment, char**, int)
{
	M_NO_MIDASSYS("rootbeer::MidasBuffer::ConnectOnline()");
	return false;
}

void rootbeer::MidasBuffer::DisconnectOnline()
{
	M_NO_MIDASSYS("rootbeer::MidasBuffer::DisconnectOnline()");
}

Bool_t rootbeer::MidasBuffer::ReadBufferOnline()
{
	M_NO_MIDASSYS("rootbeer::MidasBuffer::ReadBufferOnline()");
	return false;
}

#endif


INT rootbeer_run_stop(INT runnum, char* err)
{
	gQueue.Flush(FLUSH_TIME, rb::Event::Instance<rootbeer::TStampDiagnostics>()->GetDiagnostics());
	utils::err::Info("rb::Midas") << "Stopping run number " << runnum;
	return CM_SUCCESS;
}

INT rootbeer_run_start(INT runnum, char* err)
{
	utils::err::Info("rb::Midas") << "Starting run number " << runnum;
	reset_scalers();
	return CM_SUCCESS;
}

INT rootbeer_run_pause(INT runnum, char* err)
{
	utils::err::Info("rb::Midas") << "Pausing run number " << runnum;
	return CM_SUCCESS;
}

INT rootbeer_run_resume(INT runnum, char* err)
{
	utils::err::Info("rb::Midas") << "Resuming run number " << runnum;
	return CM_SUCCESS;
}
