///
/// \file rbdragon.cxx
/// \author G. Christian
/// \brief Implements rbdragon.hxx
///
#include <ctime>
#include <cassert>

// ROOTBEER includes //
#include <TString.h>
#include "Rint.hxx"
#include "Attach.hxx"
#include "Rootbeer.hxx"
namespace rb { namespace hist { void ClearAll(); } }

// DRAGON includes //
#include "utils/ErrorDragon.hxx"
#include "utils/definitions.h"
#include "utils/Functions.hxx"
#include "midas/Database.hxx"
#include "midas/Event.hxx"
#include "rbdragon.hxx"





//
// Helper function to process unpacked events
namespace { void process_events(const std::vector<Int_t>& codes)
{
	for (size_t i=0; i< codes.size(); ++i) {
		rb::Event* event = rb::Rint::gApp()->GetEvent (codes[i]);
		if(event) event->Process(0, 0);
	}
} }

// ============ Class rbdragon::MidasBuffer ============ //

rbdragon::MidasBuffer::MidasBuffer():
	rb::MidasBuffer(1024*1024, 500, 750, 500, 500),
	fUnpacker(rb::Event::Instance<rbdragon::GammaEvent>()->Get(),
						rb::Event::Instance<rbdragon::HeavyIonEvent>()->Get(),
						rb::Event::Instance<rbdragon::CoincEvent>()->Get(),
						rb::Event::Instance<rbdragon::EpicsEvent>()->Get(),
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
		dragon::utils::Info("rbdragon::MidasBuffer")
			<< "Syncing variable values with the online database.";
		midas::Database db("online");
		ReadVariables(&db);
	}

	/// - Zero all histograms if online
	if (fType == rb::MidasBuffer::ONLINE)
		rb::hist::ClearAll();

	/// - Call parent class implementation (prints a message)
	rb::MidasBuffer::RunStartTransition(runnum);
}


void rbdragon::MidasBuffer::RunStopTransition(Int_t runnum)
{
	///
	/// Does the following:

	/// - Flush timestamp queue (max 60 seconds for online)
	Int_t flush_time = fType == ONLINE ? 60 : -1;
	time_t t_begin = time(0);
	size_t qsize;
	while (1) {
		if (flush_time >= 0 &&
				(difftime(time(0), t_begin) > flush_time))
			break;

		fUnpacker.ClearUnpackedCodes();
		qsize = fUnpacker.FlushQueueIterative(); // Fills classes implicitly
		if (qsize == 0) break;

		std::vector<Int_t> v = fUnpacker.GetUnpackedCodes();
		process_events(v);
	} 

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

	/// - For all other events, delegate to rb::Unpacker, then call the Process() function
	std::vector<Int_t> v = fUnpacker.UnpackMidasEvent(phead, data);
	process_events(v);

	return kTRUE;
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

void rbdragon::RunParameters::HandleBadEvent()
{
	dragon::utils::Error("RunParameters")
		<< "Unknown error encountered during event processing";
}


// ======== Class rbdragon::TStampDiagnostics ======== //

rbdragon::TStampDiagnostics::TStampDiagnostics():
	fDiagnostics("tstamp", this, false, "") { }

void rbdragon::TStampDiagnostics::HandleBadEvent()
{
	dragon::utils::Error("TStampDiagnostics")
		<< "Unknown error encountered during event processing";
}


// ======== Class rbdragon::GammaEvent ======== //

rbdragon::GammaEvent::GammaEvent():
	fGamma("gamma", this, true, "") { }

void rbdragon::GammaEvent::HandleBadEvent()
{
	dragon::utils::Error("GammaEvent")
		<< "Unknown error encountered during event processing";
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
	dragon::utils::Error("HeavyIonEvent")
		<< "Unknown error encountered during event processing";
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
	dragon::utils::Error("CoincEvent")
		<< "Unknown error encountered during event processing";
}

void rbdragon::CoincEvent::ReadVariables(midas::Database* db)
{
	fCoinc->set_variables(db);
}


// ======== Class rbdragon::EpicsEvent ======== //

rbdragon::EpicsEvent::EpicsEvent():
	fEpics("epics", this, false)
{
	assert(fEpics.Get());
}

void rbdragon::EpicsEvent::HandleBadEvent()
{
	dragon::utils::Error("EpicsEvent")
		<< "Unknown error encountered during event processing";
}

void rbdragon::EpicsEvent::ReadVariables(midas::Database* db)
{
	fEpics->set_variables(db);
}


// ======== Class rbdragon::HeadScaler ======== //

rbdragon::HeadScaler::HeadScaler():
	fScaler("head_scaler", this, true)
{
	assert(fScaler.Get());
}

void rbdragon::HeadScaler::HandleBadEvent()
{
	dragon::utils::Error("HeadScaler")
		<< "Unknown error encountered during event processing";
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
}

void rbdragon::TailScaler::HandleBadEvent()
{
	dragon::utils::Error("TailScaler")
		<< "Unknown error encountered during event processing";
}

void rbdragon::TailScaler::ReadVariables(midas::Database* db)
{
	fScaler->set_variables(db, "tail");
}


// ======== Class rbdragon::Main ======== //

namespace {
void usage(const char* arg0) {
	std::string progname(arg0);
	unsigned long slashPos = progname.rfind('/');
	if (slashPos < progname.size())
		progname = progname.substr (slashPos + 1);
	std::cout << "usage: " << progname << " --unpack <input file>\n\n";
	exit(1);
}
void handle_args(int argc, char** argv, std::string& fin) {
	if(argc != 3) usage(argv[0]);
	fin  = argv[2];
} }

int rbdragon::Main::Run(int argc, char** argv)
{
	/// Copy of normal rb::Main::Run() with the following
	/// additions:
 if (argc > 1 && !strcmp(argv[1], "--unpack")) { // 'rbunpack'

	 std::string fin;
	 handle_args(argc, argv, fin);
	 int argc2 = argc + 1;
	 char** argv2 = (char**)malloc(argc2*sizeof(char*));
	 for(int i=0; i< argc; ++i) {
		 argv2[i] = (char*)malloc(strlen(argv[i]+1));
		 strcpy(argv2[i], argv[i]);
	 }
	 argv2[argc] = (char*)malloc(4);
	 strcpy(argv2[argc], "-ng");

	 rb::Rint rbApp("Rbunpack", &argc2, argv2, 0, 0, true);
	 rbApp.StartSave(false);
	 rb::AttachFile(fin.c_str());
	 gSystem->Sleep(1e2);
	 while(rb::FileAttached())
		 assert(0);
	 rbApp.Terminate(0);
	 for(int i=0; i< argc2; ++i)
		 free(argv2[i]);
	 free(argv2);
	 return 0;

 } else { // Standard ROOTBEER

	 std::set<std::string> args(argv, argv+argc);
	 Bool_t lite = args.count("-l");
	 rb::Rint rbApp("Rootbeer", &argc, argv, 0, 0, lite);
	 gROOT->ProcessLine("gStyle->SetOptTitle(kTRUE)");
	 gROOT->ProcessLine("gStyle->SetOptStat(\"emri\")");

	 /// - Load standard histograms if the file exists
	 Int_t processed;
	 {
		 dragon::utils::ChangeErrorIgnore err_ignore(8001);
		 processed = gROOT->ProcessLine(".x $RB_CONFIGDIR/dragon_hists.C");
	 }
	 if(processed == 0) {
		 dragon::utils::Error("Main")
			 << "Couldn't load macro file: \"$RB_CONFIGDIR/dragon_hists.C\": "
			 << "Check that the RB_CONFIGDIR environment variable is set and dragon_hists.C exists.";
	 }

	 rbApp.Run();
	 return 0;

 }
}


// ============ Required Static / Free Function Implementations ============ //

rb::MidasBuffer* rb::MidasBuffer::Create()
{
	/*!
	 * \returns <tt>new rbdragon::MidasBuffer();</tt>
	 */
	return new rbdragon::MidasBuffer();
}

rb::Main* rb::GetMain()
{
	/*!
	 * Use the default main() implementation.
	 * \returns <tt>new rb::Main();</tt>
	 */
	return new rbdragon::Main();
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

	/// - Epics [ rbdragon::EpicsEvent ]: "EpicsEvent"
  RegisterEvent<rbdragon::EpicsEvent> (DRAGON_EPICS_EVENT, "EpicsEvent");

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
