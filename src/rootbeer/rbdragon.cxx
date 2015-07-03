///
/// \file rbdragon.cxx
/// \author G. Christian
/// \brief Implements rbdragon.hxx
///
#include <ctime>
#include <cassert>

#include <TSystem.h>
#include <TFile.h>

// ROOTBEER includes //
#include <TString.h>
#include "Rint.hxx"
#include "Attach.hxx"
#include "Rootbeer.hxx"
#include "Event.hxx"

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

#define G_ERROR_RESET(level) ErrorReset err_reset_dummy_123456789 (level)
namespace { class ErrorReset {
	Int_t fIgnore;
public:
	ErrorReset(Int_t level): fIgnore(gErrorIgnoreLevel)
		{ gErrorIgnoreLevel = level; }
	~ErrorReset()
		{ gErrorIgnoreLevel = fIgnore; }
}; }

namespace { Int_t gAutoZero = 1; bool gAutoZeroOdb = true; }


// ============ Free Functions ============ //

void rbdragon::SetAutoZero(Int_t level)
{
	///
	/// \note The auto zero level can be controlled for online analysis by modifying the
	/// "/dragon/rootbeer/AutoZero" ODB variable. Set this variable to the desired auto
	///  zeroing level as explained for the _level_ parameter.
	if      (level > 2) gAutoZero = 2;
	else if (level < 0) gAutoZero = 0;
	else                gAutoZero = level;
}

Int_t rbdragon::GetAutoZero()
{
	return gAutoZero;
}

void rbdragon::SetAutoZeroOdb(bool on)
{
	gAutoZeroOdb = on;
}

bool rbdragon::GetAutoZeroOdb()
{
	return gAutoZeroOdb;
}



// ============ Class rbdragon::MidasBuffer ============ //

rbdragon::MidasBuffer::MidasBuffer():
	rb::MidasBuffer(1024*1024, 500, 750, 500, 500),
	fUnpacker(rb::Event::Instance<rbdragon::GammaEvent>()->Get(),
						rb::Event::Instance<rbdragon::HeavyIonEvent>()->Get(),
						rb::Event::Instance<rbdragon::CoincEvent>()->Get(),
						rb::Event::Instance<rbdragon::EpicsEvent>()->Get(),
						rb::Event::Instance<rbdragon::HeadScaler>()->Get(),
						rb::Event::Instance<rbdragon::TailScaler>()->Get(),
						rb::Event::Instance<rbdragon::AuxScaler>()->Get(),
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
	rb::Event::Instance<rbdragon::AuxScaler>()->Reset();
	rb::Event::Instance<rbdragon::RunParameters>()->Reset();
	rb::Event::Instance<rbdragon::TStampDiagnostics>()->Reset();

	/// - Read variables from the ODB if online and if told to (SetAutoZeroOdb)
	if (fType == rb::MidasBuffer::ONLINE && GetAutoZeroOdb() == true) {
		dragon::utils::Info("rbdragon::MidasBuffer")
			<< "Syncing variable values with the online database.";
		midas::Database db("online");
		ReadVariables(&db);

		// Also set auto zero level from "/dragon/rootbeer/AutoZero"
		{
			Int_t autoZeroLevel = 0;
			Bool_t success = db.ReadValue("/dragon/rootbeer/AutoZero", autoZeroLevel);
			if(success) {
				SetAutoZero(autoZeroLevel);
			}
			else {
				dragon::utils::Warning("RunStartTransition")
					<< "Unable to read auto zero level from MIDAS database \"/dragon/rootbeer/AutoZero\", "
					<< "defaulting to level 1.";
				SetAutoZero(1);
			}
		}
	}

	/// - Zero all histograms if online and enabled:
	if (fType == rb::MidasBuffer::ONLINE) {
		switch (GetAutoZero()) {
		case 0:       /// - Level 0 - do nothing
			break;
		case 1:       /// - Level 1 - clear scalers only
			{
				Int_t codes[] = { DRAGON_HEAD_SCALER, DRAGON_TAIL_SCALER, DRAGON_AUX_SCALER };
				Int_t ncodes = sizeof(codes) / sizeof(Int_t);
				for(Int_t i=0; i< ncodes; ++i) {
					rb::Event* evt = rb::Rint::gApp()->GetEvent (codes[i]);
					if(!evt) continue;
					rb::hist::Manager* manager = evt->GetHistManager();
					if(manager) manager->ClearAll();
				}
			}
			break;
		case 2:       /// - Level 2 - clear everything
			rb::hist::ClearAll(); 
			break;
		default:
			break;
		}
	}

	/// - Look at ODB and set canvas to web saving if enabled
	if (fType == rb::MidasBuffer::ONLINE) {
		midas::Database db("online");

		std::string webFile = "";
		Bool_t saveCanvas = true, success = kTRUE;
		if(success) saveCanvas = midas::Odb::ReadBool("/dragon/rootbeer/canvas/SaveToWeb");
		if(success) success = db.ReadValue("/dragon/rootbeer/canvas/WebFile", webFile);
		
		if(success && !webFile.empty()) {
			rb::canvas::SetWebFile(webFile.c_str());
		} else {
			if(!success) {
				dragon::utils::Warning("RunStartTransition")
					<< "Couldn't read web canvas settings from ODB";
			} else {
				dragon::utils::Warning("RunStartTransition")
					<< "Empty \"/dragon/rootbeer/WebFile\" in ODB";
			}
		}
	}

	/// - Call parent class implementation (prints a message)
	rb::MidasBuffer::RunStartTransition(runnum);
}

#if 0
namespace {
void IterateDir(TDirectory* dir, TDirectory* newdir)
{
	for(Int_t i=0; i< dir->GetList()->GetEntries(); ++i) {
		newdir->cd();
		TObject* obj = dir->GetList()->At(i);
		if(obj->InheritsFrom(TDirectory::Class())) {
			TDirectory* newdir1 = gDirectory->mkdir(obj->GetName(), obj->GetTitle());
			IterateDir(static_cast<TDirectory*>(obj), static_cast<TDirectory*>(newdir1));
		}
		else if(obj->InheritsFrom(rb::hist::Base::Class())) {
			static_cast<rb::hist::Base*>(obj)->GetHist()->Write(obj->GetName());
		}
	}
} }
#endif

void rbdragon::MidasBuffer::RunStopTransition(Int_t runnum)
{
	///
	/// Does the following:

	/// - Flush timestamp queue (max 15 seconds for online)
	Int_t flush_time = fType == ONLINE ? 15 : -1;
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
	if (qsize) {
		fUnpacker.GetQueue()->FlushTimeoutMessage(flush_time);
		fUnpacker.GetQueue()->Clear();
	}

	/// - Print delayed error messages
	dragon::utils::gDelayedMessageFactory.Flush();

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
	rb::Event::Instance<rbdragon::AuxScaler>()->ReadVariables(db);
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
	dragon::utils::Error("RunParameters", __FILE__, __LINE__)
		<< "Unknown error encountered during event processing";
}


// ======== Class rbdragon::TStampDiagnostics ======== //

rbdragon::TStampDiagnostics::TStampDiagnostics():
	fDiagnostics("tstamp", this, false, "") { }

void rbdragon::TStampDiagnostics::HandleBadEvent()
{
	dragon::utils::Error("TStampDiagnostics", __FILE__, __LINE__)
		<< "Unknown error encountered during event processing";
}


// ======== Class rbdragon::GammaEvent ======== //

rbdragon::GammaEvent::GammaEvent():
	fGamma("head", this, true, "") { }

void rbdragon::GammaEvent::HandleBadEvent()
{
	dragon::utils::Error("GammaEvent", __FILE__, __LINE__)
		<< "Unknown error encountered during event processing";
}

void rbdragon::GammaEvent::ReadVariables(midas::Database* db)
{
	fGamma->set_variables(db);
}


// ======== Class rbdragon::HeavyIonEvent ======== //

rbdragon::HeavyIonEvent::HeavyIonEvent():
	fHeavyIon("tail", this, true, "") { }

void rbdragon::HeavyIonEvent::HandleBadEvent()
{
	dragon::utils::Error("HeavyIonEvent", __FILE__, __LINE__)
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
	dragon::utils::Error("CoincEvent", __FILE__, __LINE__)
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
	dragon::utils::Error("EpicsEvent", __FILE__, __LINE__)
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
	dragon::utils::Error("HeadScaler", __FILE__, __LINE__)
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
	dragon::utils::Error("TailScaler", __FILE__, __LINE__)
		<< "Unknown error encountered during event processing";
}

void rbdragon::TailScaler::ReadVariables(midas::Database* db)
{
	fScaler->set_variables(db, "tail");
}

// ======== Class rbdragon::AuxScaler ======== //

rbdragon::AuxScaler::AuxScaler():
	fScaler("aux_scaler", this, true)
{
	assert(fScaler.Get());
}

void rbdragon::AuxScaler::HandleBadEvent()
{
	dragon::utils::Error("AuxScaler", __FILE__, __LINE__)
		<< "Unknown error encountered during event processing";
}

void rbdragon::AuxScaler::ReadVariables(midas::Database* db)
{
	fScaler->set_variables(db, "aux");
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
	 {
		 dragon::utils::ChangeErrorIgnore err_ignore(8001);
		 rb::ReadHistXML("$RB_CONFIGDIR/histograms/dragon_hists.xml");
	 }

	 rbApp.Run();
	 return 0;

 }
}

