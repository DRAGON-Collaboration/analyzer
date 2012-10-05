/*!
 * \file RootanaDragon.cxx
 * \author G. Christian
 * \brief Implements RootanaDragon.hxx
 */
#include <cassert>
#include <cstring>

#include <TFile.h>
#include <TSystem.h>

#include "midas.h"
#include "XmlOdb.h"
#include "libNetDirectory/netDirectoryServer.h"

#include "utils/definitions.h"
#include "utils/Error.hxx"
#include "midas/Event.hxx"
#include "Timer.hxx"
#include "Events.hxx"
#include "Timestamp.hxx"
#include "HistParser.hxx"
#include "IncludeMidasOnline.h"
#include "RootanaDragon.hxx"


/// \todo Reduce globals as much as possible
extern int  gRunNumber;
extern bool gIsRunning;
extern bool gDebugEnable;

extern TDirectory* gOnlineHistDir;
extern TFile* gOutputFile;
extern VirtualOdb* gOdb;

extern double gQueueTime;
namespace {
rootana::TSQueue gQueue ( gQueueTime );
}


// CALLBACKS //

void rootana_run_start(int transition, int run, int time)
{
	/*!
	 *  Sets status flags, calls EventHandler::BeginRun(), opens output file.
	 */
  gIsRunning = true;
  gRunNumber = run;
    
  if(gOutputFile!=NULL) {
    gOutputFile->Write();
    gOutputFile->Close();
    gOutputFile=NULL;
  }
	
	rootana::EventHandler::Instance()->BeginRun();

  char filename[1024];
  sprintf(filename, "output%05d.root", run); /// \todo better Output directory
  gOutputFile = new TFile(filename,"RECREATE");

  NetDirectoryExport(gOutputFile, "outputFile");

	dragon::err::Info("rootana") << "Start of run " << run;
}

void rootana_run_stop(int transition, int run, int time)
{
	/*!
	 *  Sets appropriate status flags, calls EventHandler::EndRun() to
	 *  save histograms, closes output root file.
	 */
  gIsRunning = false;
  gRunNumber = run;

#ifdef OLD_SERVER
  if (gManaHistosFolder)
    gManaHistosFolder->Clear();
#endif

	rootana::EventHandler::Instance()->EndRun();

  if(gOutputFile) {
		gOutputFile->Write();
		gOutputFile->Close();		//close the histogram file
		gOutputFile = NULL;
	}

	dragon::err::Info("rootana") << "End of run " << run;
}

void rootana_run_pause(int transition, int run, int time)
{
	/*!
	 * Prints an information message.
	 */
	dragon::err::Info("rootana") << "Pausing run " << run;
}

void rootana_run_resume(int transition, int run, int time)
{
	/*!
	 * Prints an information message.
	 */
	dragon::err::Info("rootana") << "Resuming run " << run;
}

void rootana_handle_event(const void* pheader, const void* pdata, int size)
{
	/*!
	 * Figure out the TSC bank name from event id, then pass on the work to
	 * rootana::App::handle_event().
	 */
	const midas::Event::Header* head = reinterpret_cast<const midas::Event::Header*>(pheader);
	char tscBk[5];
	if (head->fEventId == DRAGON_TAIL_EVENT)
		strcpy(tscBk, "TSCT");
	else 
		strcpy(tscBk, "TSCH");

	midas::Event e(tscBk, pheader, pdata, head->fDataSize);
	rootana::App::instance()->handle_event(e);
}



// APPLICATION CLASS //

rootana::App::App(const char* appClassName, Int_t* argc, char** argv, void* options, Int_t numOptions):
	TApplication (appClassName, argc, argv, options, numOptions)
{ }

rootana::App* rootana::App::instance()
{
	/*!
	 *  \note Runtime failure if gApplication is NULL or not as instance
	 *   of rootana::App
	 */
	App* app = dynamic_cast<App*>( gApplication );
	assert (app);
	return app;
}

void rootana::App::handle_event(midas::Event& event)
{
	/*!
	 * \todo Write detailed documentation
	 */
	switch (event.GetEventId()) {
	case DRAGON_HEAD_EVENT:  /// - DRAGON_HEAD_EVENT: Insert into timestamp matching queue
		gQueue.Push(event);
		break;
	case DRAGON_TAIL_EVENT:  /// - DRAGON_TAIL_EVENT: Insert into timestamp matching queue
		gQueue.Push(event);
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

int rootana::App::create_histograms(const char* definition_file)
{
	/*!
	 * Parses histogram definition file & creates histograms.
	 * \returns 0 if successful, -1 otherwise
	 */
	rootana::HistParser parse (definition_file);
	try {
		parse.run();
	}
	catch (std::exception& e) {
		std::cerr << "\n*******\n";
		dragon::err::Error("HistParser") << e.what();
		std::cerr << "*******\n\n";
		return -1;
	}
	return 0;
}

int rootana::App::midas_file(const char* fname, int cutoff)
{
	/*!
	 *  \todo Write detailed documentation
	 *  \returns 0 if successful, -1 if failure
	 */
  TMidasFile f;
  bool tryOpen = f.Open(fname);
  if (!tryOpen) {
		printf("Cannot open input file \"%s\"\n",fname);
		return -1;
	}

	int hists = create_histograms("src/rootana/histos.dat");
	if (hists != 0) return -1;	

  int i=0;
  while (1) {

		TMidasEvent event;
		if (!f.Read(&event)) break;

		int eventId = event.GetEventId();

		if ((eventId & 0xFFFF) == 0x8000) { // begin run

			printf("---- BEGIN RUN ---- \n");

			if (gOdb)
				delete gOdb;
			gOdb = new XmlOdb(event.GetData(),event.GetDataSize());

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

		if ( (cutoff!=0) && (i >= cutoff) ) {
			printf("Reached event %d, exiting loop.\n",i);
			break;
		}
	}
  
  f.Close();

	rootana_run_stop(0, gRunNumber, 0);

  return 0;
}

int rootana::App::midas_online(const char* host, const char* experiment)
{
	/*!
	 *  \todo Write detailed documentation
	 */

	TMidasOnline *midas = TMidasOnline::instance();

	int err = midas->connect(host, experiment, "anaDragon");
	printf("Connecting to experiment %s on host %s!\n", host, experiment);

	if (err != 0) {
		fprintf(stderr,"Cannot connect to MIDAS, error %d\n", err);
		return -1;
	}

	gOdb = midas;

	midas->setTransitionHandlers(rootana_run_start, rootana_run_stop, rootana_run_resume, rootana_run_pause);
	midas->registerTransitions();

	/* reqister event requests */

	midas->setEventHandler(rootana_handle_event);
	midas->eventRequest("SYNC",-1,-1,(1<<1));

	/* fill present run parameters */

	gRunNumber = gOdb->odbReadInt("/runinfo/Run number");

	if ((gOdb->odbReadInt("/runinfo/State") == 3)) {
		printf ("State is running... executing run start transitino handler.\n");
		rootana_run_start(0, gRunNumber, 0);
	}

	/* create histograms */
	int hists = create_histograms("src/rootana/histos.dat");
	if(hists != 0) {
		midas->disconnect();
		return -1;
	}

	printf("Startup: run %d, is running: %d\n",gRunNumber,gIsRunning);
	printf("Host: %s, experiment: %s\n", host, experiment);
	printf("Enter \"!\" to exit.\n");

	rootana::Timer tm(100);

	/*---- start main loop ----*/

	//loop_online();
	this->Run(kTRUE);

	/* disconnect from experiment */
	midas->disconnect();
	rootana_run_stop(0, gRunNumber, 0);

	return 0;
}

