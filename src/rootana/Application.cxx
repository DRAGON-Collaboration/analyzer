/*!
 * \file Application.cxx
 * \author G. Christian
 * \brief Implements Application.hxx
 */
#include <cassert>
#include <vector>
#include <string>

#include <TROOT.h>
#include <TFile.h>
#include <TSystem.h>

#ifdef MIDASSYS
#include "midas.h"
#include "IncludeMidasOnline.h"
#endif
#include "XmlOdb.h"
#include "libNetDirectory/netDirectoryServer.h"

#include "utils/TStamp.hxx"
#include "Timer.hxx"
#include "Histos.hxx"
#include "HistParser.hxx"
#include "Callbacks.hxx"
#include "Directory.hxx"

#include "Globals.h"

// APPLICATION CLASS //

#define ROOTANA_STRINGIFY_MACRO(S) ROOTANA_STRINGIFY (S)
#define ROOTANA_STRINGIFY(S) #S

rootana::App::App(const char* appClassName, Int_t* argc, char** argv):
	TApplication (appClassName, argc, argv, 0, -1),
	fRunNumber(0),
	fMode(ONLINE),
	fCutoff(0),
	fReturn(0),
	fTcp(9091),
	fFilename(""),
	fHost(""),
	fExpt(""),
	fHistos( ROOTANA_STRINGIFY_MACRO(ROOTANA_DEFAULT_HISTOS) ),
	fHistosOnline(""),
	fOutputFile(""),
	fOnlineHists(),
	fOdb(0)
{ 
/*!
 *  Also: process command line arguments, starts histogram server if appropriate.
 *  \todo Need to properly handle startup if NOT running!!!
 */
	process_argv (*argc, argv);
	if (!fQueue) fQueue = tstamp::NewOwnedQueue (10e6, this);
	if (fMode == ONLINE) {
		gROOT->cd();
		fOnlineHists.Open(fTcp, fHistosOnline.c_str());
	}
}

rootana::App* rootana::App::instance()
{
	/*!
	 *  \note Runtime failure if gApplication is NULL or not an instance
	 *   of rootana::App
	 */
	App* app = dynamic_cast<App*>( gApplication );
	assert (app);
	return app;
}

void rootana::App::process_argv(int argc, char** argv)
{
	std::vector<std::string> args(argv + 1, argv + argc);
	for (std::vector<std::string>::iterator iarg = args.begin(); iarg != args.end(); ++iarg) {
		if (0) { }
		else if ( iarg->compare("-h") == 0 )
			this->help();
		else if ( iarg->compare(0, 2, "-e") == 0 )
			fCutoff =  atoi (iarg->substr(2).c_str() );
		else if ( iarg->compare(0, 2, "-P") == 0 )
			fTcp  = atoi (iarg->substr(2).c_str() );
		else if ( iarg->compare(0, 2, "-H") == 0 )
			fHost = iarg->substr(2);
		else if ( iarg->compare(0, 2, "-E") == 0 )
			fExpt = iarg->substr(2);
		else if ( iarg->compare(0, 2, "-Q") == 0 )
			fQueue = tstamp::NewOwnedQueue( atof(iarg->substr(2).c_str()), this );
		else if ( iarg->compare("-histos")  == 0 )
			fHistos = *(++iarg);
		else if ( iarg->compare("-histos0")  == 0 )
			fHistosOnline = *(++iarg);
		else if ( iarg->compare(0, 1, "-" ) == 0 )
			this->help();
		else {
			fMode = OFFLINE;
			fFilename = *iarg;
		}
	}
}

void rootana::App::handle_event(midas::Event& event)
{
	/*!
	 * \todo Write detailed documentation
	 */
	switch (event.GetEventId()) {
	case DRAGON_HEAD_EVENT:  /// - DRAGON_HEAD_EVENT: Insert into timestamp matching queue
		fQueue->Push(event);
		break;
	case DRAGON_TAIL_EVENT:  /// - DRAGON_TAIL_EVENT: Insert into timestamp matching queue
		fQueue->Push(event);
		break;
	case DRAGON_HEAD_SCALER: /// - DRAGON_HEAD_SCALER: TODO: implement C. Stanford's scaler codes
		// <...process...> //
		break;
	case DRAGON_TAIL_SCALER: /// - DRAGON_TAIL_SCALER: TODO: implement C. Stanford's scaler codes
		// <...process...> //
		break;
	default: /// - Silently ignore other event types
		break;
	}
}


namespace {
template <class T, class E>
inline void unpack_event(T& data, const E& buf)
{
	data.reset();
	data.unpack(buf);
	data.calculate();
} }

void rootana::App::Process(const midas::Event& event)
{
	const uint16_t EID = event.GetEventId();
	switch (EID) {

	case DRAGON_HEAD_EVENT:  /// - DRAGON_HEAD_EVENT: Calculate head params & fill head histos
		unpack_event(rootana::gHead, event);
		fOutputFile.CallForAll( &rootana::HistBase::fill, EID );
		break;

	case DRAGON_TAIL_EVENT:  /// - DRAGON_TAIL_EVENT: Calculate tail params & fill tail histos
		unpack_event(rootana::gTail, event);
		fOutputFile.CallForAll( &rootana::HistBase::fill, EID );
		break;

	case DRAGON_HEAD_SCALER: /// - DRAGON_HEAD_SCALER: \todo implement C. Stanford's scaler codes
		// <...process...> //
		break;

	case DRAGON_TAIL_SCALER: /// - DRAGON_TAIL_SCALER: \todo implement C. Stanford's scaler codes
		// <...process...> //
		break;

	default: /// - Silently ignore other event types
		break;
	}

}

void rootana::App::Process(const midas::Event& event1, const midas::Event& event2)
{
	/// \todo Try to avoid extra unpacking step
	midas::CoincEvent coincEvent(event1, event2);

	if (coincEvent.fHeavyIon == 0 ||	coincEvent.fGamma == 0) {
		dragon::err::Error("rootana::TSQueue::HandleCoinc")
			<< "Invalid coincidence event, skipping...\n";
		return;
	}

	unpack_event(rootana::gCoinc, coincEvent);
	fOutputFile.CallForAll( &rootana::HistBase::fill, DRAGON_COINC_EVENT );
}

int rootana::App::midas_file(const char* fname)
{
	/*!
	 * Loops through a midas file and processes the events, up
	 * to fCutoff.
	 * \returns 0 if successful, -1 if failure
	 */
	printf ("Processing offline file: %s", fname);
	if (fCutoff) printf (" (%i events)\n", fCutoff);
	else printf("\n");

  TMidasFile f;
  bool tryOpen = f.Open(fname);
  if (!tryOpen) {
		printf("Cannot open input file \"%s\"\n",fname);
		return -1;
	}

  int i=0;
  while (1) {

		TMidasEvent event;
		if (!f.Read(&event)) break;

		int eventId = event.GetEventId();

		if ((eventId & 0xFFFF) == 0x8000) { // begin run

			printf("---- BEGIN RUN ---- \n");

			if (fOdb)
				delete fOdb;
			fOdb = new XmlOdb(event.GetData(),event.GetDataSize());

			rootana_run_start(0, event.GetSerialNumber(), 0);
		}

		else if ((eventId & 0xFFFF) == 0x8001) { // end run
			printf("---- END RUN ---- \n");
		}

		else {
			event.SetBankList();
			rootana_handle_event(event.GetEventHeader(), event.GetData(), event.GetDataSize());
		}
      
		if((i%500)==0)
			printf("Processing event %d\n",i);
		
		i++;

		if ( (fCutoff!=0) && (i >= fCutoff) ) {
			printf("Reached event %d, exiting loop.\n",i);
			break;
		}
	}
  
  f.Close();
  return 0;
}

int rootana::App::midas_online(const char* host, const char* experiment)
{
	/*!
	 *  \todo Write detailed documentation
	 */

	TMidasOnline *midas = TMidasOnline::instance();

	int err = midas->connect(host, experiment, "anaDragon");
	printf("Connecting to experiment \"%s\" on host \"%s\"!\n", host, experiment);

	if (err != 0) {
		fprintf(stderr,"Cannot connect to MIDAS, error %d\n", err);
		return -1;
	}

	fOdb = midas;

	midas->setTransitionHandlers(rootana_run_start, rootana_run_stop, rootana_run_resume, rootana_run_pause);
	midas->registerTransitions();

	/* reqister event requests */

	midas->setEventHandler(rootana_handle_event);
	midas->eventRequest("SYNC",-1,-1,(1<<1));

	/* fill present run parameters */

	fRunNumber = fOdb->odbReadInt("/runinfo/Run number");

	if ((fOdb->odbReadInt("/runinfo/State") == 3)) {
		printf ("State is running... executing run start transition handler.\n");
		rootana_run_start(0, fRunNumber, 0);
	}
	
	printf("Startup: run %d\n",fRunNumber);
	printf("Host: \"%s\", experiment: \"%s\"\n", host, experiment);
	printf("Enter \"!\" to exit.\n");

	rootana::Timer tm(100);


	/*---- start main loop ----*/

	//loop_online();
	TApplication::Run(kTRUE);

	/* disconnect from experiment */
	midas->disconnect();

	return 0;
}

void rootana::App::Run(Bool_t)
{
	/*!
	 *  Calls either midas_online() or midas_file() depending on 'mode'.
	 */
	switch (fMode) {

	case OFFLINE:
		fReturn = midas_file(fFilename.c_str());
		break;

	case ONLINE:
#ifdef MIDASSYS
		fReturn = midas_online(fHost.c_str(), fExpt.c_str());
#else
		fprintf(stderr, "Can't run in online mode without MIDAS libraries.\n");
		fReturn = 1;
#endif
		break;

	default:
		assert ("Shouldn't get here!" && 0);
	}

	if(fReturn == 0) rootana_run_stop(0, fRunNumber, 0);
}

void rootana::App::Terminate(Int_t status)
{
	exit(status);
}

rootana::App::~App()
{
	printf ("Terminating application...\n");
	if (fQueue) { delete fQueue; fQueue = 0; }
}

void rootana::App::run_start(int runnum)
{
	/*!
	 *  Sets status flags, calls EventHandler::BeginRun(), opens output file.
	 */
  fRunNumber = runnum;
    
	bool opened = fOutputFile.Open(runnum, fHistos.c_str());
	if(!opened) { 
		TMidasOnline::instance()->disconnect();
		exit(1);
	}

	dragon::err::Info("rootana") << "Start of run " << runnum;
}

void rootana::App::run_stop(int runnum)
{
	/*!
	 *  Sets appropriate status flags, calls EventHandler::EndRun() to
	 *  save histograms, closes output root file.
	 */
  fRunNumber = runnum;
	fQueue->Flush(30);
	fOutputFile.Close();
	dragon::err::Info("rootana") << "End of run " << runnum;
}


void rootana::App::help()
{
  printf("\nUsage:\n");
  printf("\n./anaDragon [-h] [-histos <histogram file>] [-histos0 <histogram file>] [-Qtime] [-Hhostname] [-Eexptname] [-eMaxEvents] [-P9091] [file1 file2 ...]\n");
  printf("\n");
  printf("\t-h: print this help message\n");
  printf("\t-T: test mode - start and serve a test histogram\n");
	printf("\t-histos: Specify offline/online histogram definition file\n");
	printf("\t-histos: Specify online *only* histogram definition file\n");
  printf("\t-Hhostname: connect to MIDAS experiment on given host\n");
  printf("\t-Eexptname: connect to this MIDAS experiment\n");
	printf("\t-Qtime: Set timestamp matching queue time in microseconds (default: 10e6)\n");
  printf("\t-P: Start the TNetDirectory server on specified tcp port (for use with roody -Plocalhost:9091)\n");
  printf("\t-e: Number of events to read from input data files\n");
  printf("\n");
  exit(1);
}
