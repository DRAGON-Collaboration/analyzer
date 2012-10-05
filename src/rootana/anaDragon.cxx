//
// ROOT analyzer
//
// K.Olchanski
//
// $Id: analyzer.cxx 64 2008-12-22 03:14:11Z olchansk $
//
#include "midas.h"
#include <stdio.h>
#include <iostream>
#include <assert.h>
#include <signal.h>

#include "TMidasEvent.h"
#include "TMidasFile.h"
#include "XmlOdb.h"
#include "libNetDirectory/netDirectoryServer.h"

#include <TSystem.h>
#include <TROOT.h>
#include <TApplication.h>
#include <TFile.h>
#include <TDirectory.h>
#include <TGClient.h>
#include <TGFrame.h>
#include <TFolder.h>
#include <TH1D.h>

#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>

#include "utils/definitions.h"
#include "utils/Error.hxx"
#include "midas/Database.hxx"
#include "midas/Event.hxx"
#include "dragon/Coinc.hxx"
#include "IncludeMidasOnline.h"
#include "RootanaDragon.hxx"
#include "HistParser.hxx"
#include "Timestamp.hxx"
#include "Histos.hxx"
#include "Events.hxx"
#include "Timer.hxx"
#include "Globals.h"


// Global Variables
int  gRunNumber = 0;
bool gIsRunning = false;

TDirectory* gOnlineHistDir = NULL;
TFile* gOutputFile = NULL;
VirtualOdb* gOdb = NULL;

double gQueueTime = 10e6;


// Main function call

int main(int argc, char *argv[])
{
	setbuf(stdout,NULL);
	setbuf(stderr,NULL);
 
	signal(SIGILL,  SIG_DFL);
	signal(SIGBUS,  SIG_DFL);
	signal(SIGSEGV, SIG_DFL);

	rootana::App* app = new rootana::App("rootana", &argc, argv);
	app->Run();
	return app->ReturnVal();
}

//end
