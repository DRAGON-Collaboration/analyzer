/// \file MidasBuffer.cxx
/// \author G. Christian
/// \brief Implements MidasBuffer.hxx
#include "rootbeer/Timestamp.hxx"
#include "utils/definitions.h"
#include "midas/Event.hxx"
#include "DragonEvents.hxx"
#include "MidasBuffer.hxx"


namespace {

const int FLUSH_TIME = 5;
const double QUEUE_TIME = 10e6;

rootbeer::TSQueue gQueue (QUEUE_TIME);

}


Bool_t rootbeer::MidasBuffer::ReadBufferOffline()
{
	/*!
	 * Reads event data into fBuffer
	 */
	TMidasEvent temp;
	temp.SetData(sizeof(fBuffer), fBuffer);
	temp.Clear();

	Bool_t have_event = fFile.Read(&temp);
	if ( have_event &&
			 temp.GetDataSize() + sizeof(midas::Event::Header) > sizeof(fBuffer) ) {
		utils::err::Warning("rootbeer::MidasBuffer::ReadBufferOffline") << "Received a truncated event";
		fIsTruncated = true;
	}

	return have_event;
}

Bool_t rootbeer::MidasBuffer::UnpackBuffer()
{
	/*!
	 * Figures out the type of MIDAS event buffer received and sends onto the
	 * appropriate event handler (see below):
	 */

	midas::Event::Header* evtHeader = reinterpret_cast<midas::Event::Header*>(fBuffer);

	switch (evtHeader->fEventId) {
	case DRAGON_HEAD_EVENT:  /// - DRAGON_HEAD_EVENT: Insert into timestamp matching queue
		gQueue.Push(midas::Event(rb::Event::Instance<rootbeer::GammaEvent>()->TscBank(),
														 fBuffer, evtHeader->fDataSize));
		break;
	case DRAGON_TAIL_EVENT:  /// - DRAGON_TAIL_EVENT: Insert into timestamp matching queue
		gQueue.Push(midas::Event(rb::Event::Instance<rootbeer::HeavyIonEvent>()->TscBank(),
														 fBuffer, evtHeader->fDataSize));
		break;
	case DRAGON_HEAD_SCALER: /// - DRAGON_HEAD_SCALER: Unpack event
		rb::Event::Instance<rootbeer::HeadScaler>()->Process(fBuffer, 0);
		break;
	case DRAGON_TAIL_SCALER: /// - DRAGON_TAIL_SCALER: Unpack event
		rb::Event::Instance<rootbeer::TailScaler>()->Process(fBuffer, 0);
		break;
	default: /// - Silently ignore other event types
		break;
	}

	return true;

	/// \todo Explicitly add case (even if empty) for any other expected types (EPICS), and have default
	/// print an error message
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
	char syncbuf[] = "SYNC";

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

	/// - Register transition handlers
	/// \note Stop transition needs to have a 'late' (>700) priority to receive
	///  events flushed from the "SYNC" buffer at the end of the run
	cm_register_transition (TR_STOP,   rootbeer_run_stop,   750);
	cm_register_transition (TR_START,  rootbeer_run_start,  500);
	cm_register_transition (TR_PAUSE,  rootbeer_run_pause,  500);
	cm_register_transition (TR_RESUME, rootbeer_run_resume, 500);

	return true;
}

void rootbeer::MidasBuffer::DisconnectOnline()
{
	/*! Calls cm_disconnect_experiment() and flushes queue */
	cm_disconnect_experiment();
	gQueue.Flush(FLUSH_TIME);
	utils::err::Info("rootbeer::MidasBuffer::DisconnectOnline")
		<< "Disconnecting from experiment";
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
	const int timeout = 0;
	INT size, status;
	do {
		size = sizeof(fBuffer);

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
		utils::err::Warning("rootbeer::MidasBuffer::ReadBufferOnline") << "Received a truncated event";
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
	gQueue.Flush(FLUSH_TIME);
	utils::err::Info("rb::Midas") << "Stopping run number " << runnum;
	return CM_SUCCESS;
}

INT rootbeer_run_start(INT runnum, char* err)
{
	utils::err::Info("rb::Midas") << "Starting run number " << runnum;
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
#include <midas.h>
#include "utils/Error.hxx"
#include "midas/Database.hxx"

void dbtest()
{

	midas::Database db("online");
	double slope = -1001;
	db.ReadValue("/dragon/head/variables/xtdc/slope", slope);
	printf("slope: %f\n", slope);


	// int hndle;
	
	// printf("Exists? %i [ CM_SUCCESS = %i, CM_NO_CLIENT = %i ]\n", cm_exist("rootbeer", TRUE), CM_SUCCESS, CM_NO_CLIENT);

	// cm_get_experiment_database(&hndle, 0);
	// if (hndle == 0)
	// 	err::Error("midas::Odb") << "Not connected to an experiment\n";

}
