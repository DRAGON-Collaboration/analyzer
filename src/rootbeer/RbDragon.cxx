///
/// \file DragonRootbeer.cxx
/// \author G. Christian
/// \brief Implementation of rb::BufferSource::New() and rb::Rint::RegisterEvents()
///
#include <cassert>

// ROOTBEER includes //
#include "Main.hxx"
#include "Rint.hxx"

// DRAGON includes //
#include "utils/ErrorDragon.hxx"
#include "utils/definitions.h"
#include "utils/Stringify.h"
#include "midas/Database.hxx"
#include "midas/Event.hxx"
#include "DragonEvents.hxx"
#include "RbDragon.hxx"




// ============ Class rbdragon::MidasBuffer ============ //

rbdragon::MidasBuffer::MidasBuffer():
	rb::MidasBuffer(1024*1024, 500, 750, 500, 500),
	fUnpacker(rb::Event::Instance<rbdragon::GammaEvent>()->Get(),
						rb::Event::Instance<rbdragon::HeavyIonEvent>()->Get(),
						rb::Event::Instance<rbdragon::CoincEvent>()->Get(),
						rb::Event::Instance<rbdragon::HeadScaler>()->Get(),
						rb::Event::Instance<rbdragon::TailScaler>()->Get(),
						rb::Event::Instance<rbdragon::RunParameters>()->Get(),
						rb::Event::Instance<rbdragon::TStampDiagnostics>()->Get())
{
	///
	/// Set transition priorities different from default (750 for stop), set buffer size to 1024*1024,
	/// initialize fUnpacker w/ rb::Event<> instances.
	;
}

rbdragon::MidasBuffer::~MidasBuffer()
{
	;
}

void rbdragon::MidasBuffer::RunStartTransition(Int_t runnum)
{
	///
	/// Does the following:

	/// - Reset scalers
	rb::Event::Instance<rbdragon::HeadScaler>()->Reset();
	rb::Event::Instance<rbdragon::TailScaler>()->Reset();
	rb::Event::Instance<rbdragon::RunParameters>()->Reset();
	rb::Event::Instance<rbdragon::TStampDiagnostics>()->Reset();

	/// - Read variables from the ODB if online
	if (fType == rb::MidasBuffer::ONLINE) {
		dragon::utils::err::Info("rbdragon::MidasBuffer")
			<< "Synching variable values with the online database.";
		midas::Database db("online");
		ReadVariables(&db);
	}

	/// - Call parent class implementation (prints a message)
	rb::MidasBuffer::RunStartTransition(runnum);
}


void rbdragon::MidasBuffer::RunStopTransition(Int_t runnum)
{
	///
	/// Does the following:

	/// - Flush timestamp queue (max 60 seconds for online)
	Int_t flush_time = fType == ONLINE ? 60 : -1;
	fUnpacker.FlushQueue(flush_time);

	/// - Call parent class implementation (prints a message)
	rb::MidasBuffer::RunStopTransition(runnum);
}


void rbdragon::MidasBuffer::ReadVariables(midas::Database* db)
{
	rb::Event::Instance<rbdragon::GammaEvent>()->ReadVariables(db);
	rb::Event::Instance<rbdragon::HeavyIonEvent>()->ReadVariables(db);
	rb::Event::Instance<rbdragon::CoincEvent>()->ReadVariables(db);
	rb::Event::Instance<rbdragon::HeadScaler>()->ReadVariables(db);
	rb::Event::Instance<rbdragon::TailScaler>()->ReadVariables(db);
}

Bool_t rbdragon::MidasBuffer::UnpackEvent(void* header, char* data)
{
	///
	/// Does the following:

	midas::Event::Header* phead = reinterpret_cast<midas::Event::Header*>(header);

	/// - If BOR event (ODB dump), set variables from there
	if (phead->fEventId == MIDAS_BOR) {
		midas::Database db(data, phead->fDataSize);
		ReadVariables(&db);
	}

	/// - For all other events, delegate to rb::Unpacker (note: vector output can
	//  be ignored b/c rootbeer handles TTree filling automatically).
	std::vector<Int_t> v = fUnpacker.UnpackMidasEvent(phead, data);
	return v.empty() ? false : true;
}



// ============ rb::Event Classes ============ //

namespace {

template <class T, class E>
inline void handle_event(rb::data::Wrapper<T>& data, const E* buf)
{
	data->reset();
	data->unpack(*buf);
	data->calculate();
}

} // namespace


// ======== Class rbdragon::RunParameters ======== //

rbdragon::RunParameters::RunParameters():
	fParameters("runpar", this, false, "") { }

Bool_t rbdragon::RunParameters::DoProcess(const void* addr, Int_t)
{
	const midas::Database* pdb = reinterpret_cast<const midas::Database*> (addr);
	fParameters->read_data(pdb);
	return kTRUE;
}

void rbdragon::RunParameters::HandleBadEvent()
{
	dragon::utils::err::Error("RunParameters")
		<< "Unknown error encountered during event processing";
}


// ======== Class rbdragon::TStampDiagnostics ======== //

rbdragon::TStampDiagnostics::TStampDiagnostics():
	fDiagnostics("tstamp", this, false, "") { }

void rbdragon::TStampDiagnostics::HandleBadEvent()
{
	dragon::utils::err::Error("TStampDiagnostics")
		<< "Unknown error encountered during event processing";
}


// ======== Class rbdragon::GammaEvent ======== //

rbdragon::GammaEvent::GammaEvent():
	fGamma("gamma", this, true, "") { }

void rbdragon::GammaEvent::HandleBadEvent()
{
	dragon::utils::err::Error("GammaEvent")
		<< "Unknown error encountered during event processing";
}

Bool_t rbdragon::GammaEvent::DoProcess(const void* addr, Int_t nchar)
{
  /*!
	 * Calls unpacking routines of dragon::Head to extract the raw MIDAS
	 * data into a dragon::Head structure. Then calculates higher-level
	 * parameters.
	 */
	if(!addr) {
		dragon::utils::err::Error("rbdragon::GammaEvent::DoProcess") << "Received NULL event address";
		return false;
	}

	handle_event(fGamma, AsMidasEvent(addr));

	return true;
}

void rbdragon::GammaEvent::ReadVariables(midas::Database* db)
{
	fGamma->set_variables(db);
}


// ======== Class rbdragon::HeavyIonEvent ======== //

rbdragon::HeavyIonEvent::HeavyIonEvent():
	fHeavyIon("hi", this, true, "") { }

void rbdragon::HeavyIonEvent::HandleBadEvent()
{
	dragon::utils::err::Error("HeavyIonEvent")
		<< "Unknown error encountered during event processing";
}

Bool_t rbdragon::HeavyIonEvent::DoProcess(const void* addr, Int_t nchar)
{
  /*!
	 * Calls unpacking routines of dragon::Tail to extract the raw MIDAS
	 * data into a dragon::Head structure. Then calculates higher-level
	 * parameters.
	 */
	if(!addr) {
		dragon::utils::err::Error("rbdragon::HeavyIonEvent::DoProcess") << "Received NULL event address";
		return false;
	}

	handle_event(fHeavyIon, AsMidasEvent(addr));
	return true;
}

void rbdragon::HeavyIonEvent::ReadVariables(midas::Database* db)
{
		fHeavyIon->set_variables(db);
}


// ======== Class rbdragon::CoincEvent ======== //

rbdragon::CoincEvent::CoincEvent():
	fCoinc("coinc", this, false, "") { }

void rbdragon::CoincEvent::HandleBadEvent()
{
	dragon::utils::err::Error("CoincEvent")
		<< "Unknown error encountered during event processing";
}

Bool_t rbdragon::CoincEvent::DoProcess(const void* addr, Int_t nchar)
{
	/*!
	 * Unpacks data from the head and tail MIDAS events into the corresponding
	 * fields of fDragon, then calls the calculate() methods of each.
	 * \todo Figure out a way to handle this without unpacking the events; in principle,
	 * they should have already been handled as singles events, thus we add extra overhead
	 * by going through the unpacking routines twice - it should be possible to buffer and
	 * then copy the already-unpacked head and tail structures directly.
	 */
	if(!addr) {
		dragon::utils::err::Error("rbdragon::CoincEvent::DoProcess") << "Received NULL event address";
		return false;
	}

	handle_event(fCoinc, AsCoincMidasEvent(addr));
	return true;
}

void rbdragon::CoincEvent::ReadVariables(midas::Database* db)
{
	fCoinc->set_variables(db);
}

// ======== Class rbdragon::HeadScaler ======== //

rbdragon::HeadScaler::HeadScaler():
	fScaler("head_scaler", this, true)
{
	assert(fScaler.Get());
	fScaler->variables.set_bank_names("SCH");
}

void rbdragon::HeadScaler::HandleBadEvent()
{
	dragon::utils::err::Error("HeadScaler")
		<< "Unknown error encountered during event processing";
}

Bool_t rbdragon::HeadScaler::DoProcess(const void* addr, Int_t nchar)
{
	/*!
	 * Unpacks into scaler event structure.
	 * \returns true is given a valid \d addr input, false otherwise
	 */
	if(!addr) {
		dragon::utils::err::Error("rbdragon::HeadScaler::DoProcess") << "Received NULL event address";
		return false;
	}

	fScaler->unpack(*AsMidasEvent(addr));
	return true;
}

void rbdragon::HeadScaler::ReadVariables(midas::Database* db)
{
	fScaler->set_variables(db, "head");
}


// ======== Class rbdragon::TailScaler ======== //

rbdragon::TailScaler::TailScaler():
	fScaler("head_scaler", this, true)
{
	assert(fScaler.Get());
	fScaler->variables.set_bank_names("SCT");
}

void rbdragon::TailScaler::HandleBadEvent()
{
	dragon::utils::err::Error("TailScaler")
		<< "Unknown error encountered during event processing";
}

Bool_t rbdragon::TailScaler::DoProcess(const void* addr, Int_t nchar)
{
	/*!
	 * Unpacks into scaler event structure.
	 * \returns true is given a valid \d addr input, false otherwise
	 */
	if(!addr) {
		dragon::utils::err::Error("rbdragon::TailScaler::DoProcess") << "Received NULL event address";
		return false;
	}

	fScaler->unpack(*AsMidasEvent(addr));
	return true;
}

void rbdragon::TailScaler::ReadVariables(midas::Database* db)
{
	fScaler->set_variables(db, "tail");
}



// ============ Required Static / Free Function Implementations ============ //

rb::MidasBuffer* rb::MidasBuffer::Create()
{
	return new rbdragon::MidasBuffer();
}

rb::Main* rb::GetMain()
{
	/*!
	 * Use the default main() implementation by returning a value of the base class rb::Main.
	 */
	return new rb::Main();
}

void rb::Rint::RegisterEvents()
{
	/*!
	 * From http://trshare.triumf.ca/~gchristian/rootbeer/doc/html/classrb_1_1_buffer_source.html :
	 *
	 * Registers all event processors in the program.
	 *
	 * This needs to be implemented by users to account for the different event processors
	 * they want to use in the program. See rb::Event documentation for more information.
	 *
	 * In this specific case, we registers event processors for:
	 */

	/// - Gamma singles [ rbdragon::GammaEvent ]: "GammaEvent"
	RegisterEvent<rbdragon::GammaEvent> (DRAGON_HEAD_EVENT, "GammaEvent");

	/// - Heavy-ion singles [ rbdragon::HeavyIonEvent ]: "HeavyIonEvent"
	RegisterEvent<rbdragon::HeavyIonEvent> (DRAGON_TAIL_EVENT, "HeavyIonEvent");

	/// - Coincidences [ rbdragon::CoincEvent ]: "CoincEvent"
  RegisterEvent<rbdragon::CoincEvent> (DRAGON_COINC_EVENT, "CoincEvent");

	/// - Head Scalers [ rbdragon::HeadScaler ]: "HeadScaler"
	RegisterEvent<rbdragon::HeadScaler> (DRAGON_HEAD_SCALER, "HeadScaler");

	/// - Tail Scalers [ rbdragon::TailScaler ]: "TailScaler"
	RegisterEvent<rbdragon::TailScaler> (DRAGON_TAIL_SCALER, "TailScaler");

	/// - Timestamp Diagnostics [ rbdragon::TstampDiagnostics ]: "TstampDiagnostics"
	RegisterEvent<rbdragon::TStampDiagnostics> (6, "TStampDiagnostics");

	/// - Run parameters [ rbdragon::RunParameters ]: "RunParameters"
	RegisterEvent<rbdragon::RunParameters> (7, "RunParameters");

	/// Also: expose macros in definitions.h to CINT
	std::stringstream cmd;
	cmd << "#include \"" << DRAGON_UTILS_STRINGIFY(RB_DRAGON_HOMEDIR) << "/src/utils/definitions.h\"";
	gROOT->ProcessLine(cmd.str().c_str());
}
