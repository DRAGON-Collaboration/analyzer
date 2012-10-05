/*!
 * \file RootanaDragon.cxx
 * \author G. Christian
 * \brief Implements RootanaDragon.hxx
 */
#include <cassert>
#include <TFile.h>

#ifdef HAVE_LIBNETDIRECTORY
#include "libNetDirectory/netDirectoryServer.h"
#endif

#include "utils/definitions.h"
#include "utils/Error.hxx"
#include "midas/Event.hxx"
#include "Events.hxx"
#include "Timestamp.hxx"
#include "RootanaDragon.hxx"


/// \todo Reduce globals as much as possible
extern int  gRunNumber;
extern bool gIsRunning;
extern bool gIsPedestalsRun;
extern bool gIsOffline;
extern bool gDebugEnable;
extern int  gEventCutoff;

extern TDirectory* gOnlineHistDir;
extern TFile* gOutputFile;
// extern VirtualOdb* gOdb;

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

#ifdef HAVE_LIBNETDIRECTORY
  NetDirectoryExport(gOutputFile, "outputFile");
#endif

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
	if (head->eventId == DRAGON_TAIL_EVENT)
		strcpy(tscBk, "TSCT");
	else 
		strcpy(tscBk, "TSCH");
	
	rootana::App::instance()->handle_event
		( midas::Event(tscBk, pheader, pdata, head->fDataSize) );
}



// APPLICATION CLASS //

rootana::App* rootana::App::instance()
{
	/*!
	 *  \note Runtime failure if gApplication is NULL or not as instance
	 *   of rootana::App
	 */
	App* out = dynamic_cast<App*>(gApplication);
	assert (out);
	return out;
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

int rootana::App::midas_file(const char* fname)
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

  int i=0;
  while (1) {

		midas::Event event;
		if (!event.ReadFromFile(f)) break;

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
			handle_event(event);
		}
      
		if((i%500)==0)
			printf("Processing event %d\n",i);
		
		i++;

		if ((gEventCutoff!=0)&&(i>=gEventCutoff)) {
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
}
