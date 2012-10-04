/*!
 * \file RootanaDragon.hxx
 * \author G. Christian
 * \brief Implements RootanaDragon.hxx
 */
#include <TFile.h>

#ifdef HAVE_LIBNETDIRECTORY
#include "libNetDirectory/netDirectoryServer.h"
#endif

#include "utils/Error.hxx"
#include "Events.hxx"
#include "Timestamp.hxx"
#include "RootanaDragon.hxx"

extern int  gRunNumber;
extern bool gIsRunning;
extern bool gIsPedestalsRun;
extern bool gIsOffline;
extern bool gDebugEnable;
extern int  gEventCutoff;

extern TDirectory* gOnlineHistDir;
extern TFile* gOutputFile;
// extern VirtualOdb* gOdb;

extern rootana::TSQueue gQueue;


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
}

