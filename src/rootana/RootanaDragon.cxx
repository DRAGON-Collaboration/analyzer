/*!
 * \file RootanaDragon.cxx
 * \author G. Christian
 * \brief Implements RootanaDragon.hxx
 */
#include <TFile.h>

#ifdef HAVE_LIBNETDIRECTORY
#include "libNetDirectory/netDirectoryServer.h"
#endif

#include "utils/definitions.h"
#include "utils/Error.hxx"
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

/// Run resume transition handler
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
	 *
	 */
	const EventHeader_t* head = reinterpret_cast<const EventHeader_t*>(pheader);
	switch (head->fEventId) {
	case DRAGON_HEAD_EVENT:  /// - DRAGON_HEAD_EVENT: Insert into timestamp matching queue
		gQueue.Push(midas::Event("TSCH", pheader, pdata, head->fDataSize));
		break;
	case DRAGON_TAIL_EVENT:  /// - DRAGON_TAIL_EVENT: Insert into timestamp matching queue
		gQueue.Push(midas::Event("TSCT", pheader, pdata, head->fDataSize));
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

