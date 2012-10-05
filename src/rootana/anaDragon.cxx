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

#include "TMidasOnline.h"
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
#include "Timestamp.hxx"
#include "RootanaDragon.hxx"
#include "HistParser.hxx"
#include "Histos.hxx"
#include "Events.hxx"
#include "Timer.hxx"
#include "Globals.h"


// Global Variables
int  gRunNumber = 0;
bool gIsRunning = false;
bool gIsPedestalsRun = false;
bool gIsOffline = false;
bool gDebugEnable = false;
int  gEventCutoff = 0;

TDirectory* gOnlineHistDir = NULL;
TFile* gOutputFile = NULL;
VirtualOdb* gOdb = NULL;

rootana::TSQueue gQueue ( ROOTANA_QUEUE_TIME );


int ProcessMidasFile(TApplication*app,const char*fname)
{
  TMidasFile f;
  bool tryOpen = f.Open(fname);

  if (!tryOpen)
    {
      printf("Cannot open input file \"%s\"\n",fname);
      return -1;
    }

  int i=0;
  while (1)
    {
      TMidasEvent event;
      if (!f.Read(&event))
	break;


      int eventId = event.GetEventId();

      if ((eventId & 0xFFFF) == 0x8000)
	{
	  // begin run
	  printf("---- BEGIN RUN ---- \n");
	  //event.Print();

	  //char buf[256];
	  //memset(buf,0,sizeof(buf));
	  //memcpy(buf,event.GetData(),255);
	  //printf("buf is [%s]\n",buf);

	  //
	  // Load ODB contents from the ODB XML file
	  //
	  if (gOdb)
	    delete gOdb;
	  gOdb = new XmlOdb(event.GetData(),event.GetDataSize());

		rootana_run_start(0, event.GetSerialNumber(), 0);
	}
      else if ((eventId & 0xFFFF) == 0x8001)
	{
	  printf("---- END RUN ---- \n");
	  // end run
	  //event.Print();
	}
      else
	{
	  //printf("case 3\n");	  
	  event.SetBankList();
	  //event.Print();
	  rootana_handle_event(event.GetEventHeader(), event.GetData(), event.GetDataSize());
	}
      
      if((i%500)==0)
	{
	  //resetClock2time();
	  printf("Processing event %d\n",i);
	  //SISperiodic();
	  //StepThroughSISBuffer();
	}
	
      i++;
      if ((gEventCutoff!=0)&&(i>=gEventCutoff))
	{
	  printf("Reached event %d, exiting loop.\n",i);
	  break;
	}
    }
  
  f.Close();

	rootana_run_stop(0, gRunNumber, 0);

  return 0;
}

#ifdef MIDASSYS

void MidasPollHandler()
{
  if (!(TMidasOnline::instance()->poll(0)))
    gSystem->ExitLoop();
}

int ProcessMidasOnline(TApplication*app, const char* hostname, const char* exptname)
{
   TMidasOnline *midas = TMidasOnline::instance();

   int err = midas->connect(hostname, exptname, "anaDragon");
   printf("Connecting to experiment %s on host %s!\n", hostname, exptname);

   if (err != 0)
     {
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

if ((gOdb->odbReadInt("/runinfo/State") == 3))
     rootana_run_start(0, gRunNumber, 0);

   /* create histograms */
	 rootana::HistParser parse ("src/rootana/histos.dat");
	 try { parse.run(); }
	 catch (std::exception& e) {
		 std::cerr << "\n*******\n";
		 dragon::err::Error("HistParser") << e.what();
		 std::cerr << "*******\n\n";
		 midas->disconnect();
		 exit(1);
	 }

   printf("Startup: run %d, is running: %d, is pedestals run: %d\n",gRunNumber,gIsRunning,gIsPedestalsRun);
   printf("Hostname: %s, exptname: %s\n", hostname, exptname);

	 rootana::Timer tm (100, MidasPollHandler);

   /*---- start main loop ----*/

   //loop_online();
   app->Run(kTRUE);

   /* disconnect from experiment */
   midas->disconnect();

   return 0;
}

#endif

static bool gEnableShowMem = false;

int ShowMem(const char* label)
{
  if (!gEnableShowMem)
    return 0;

  FILE* fp = fopen("/proc/self/statm","r");
  if (!fp)
    return 0;

  int mem = 0;
  fscanf(fp,"%d",&mem);
  fclose(fp);

  if (label)
    printf("memory at %s is %d\n", label, mem);

  return mem;
}
 
void help()
{
  printf("\nUsage:\n");
  printf("\n./analyzer.exe [-h] [-Hhostname] [-Eexptname] [-eMaxEvents] [-P9091] [-p9090] [-m] [-g] [file1 file2 ...]\n");
  printf("\n");
  printf("\t-h: print this help message\n");
  printf("\t-T: test mode - start and serve a test histogram\n");
  printf("\t-Hhostname: connect to MIDAS experiment on given host\n");
  printf("\t-Eexptname: connect to this MIDAS experiment\n");
  printf("\t-P: Start the TNetDirectory server on specified tcp port (for use with roody -Plocalhost:9091)\n");
  printf("\t-p: Start the old midas histogram server on specified tcp port (for use with roody -Hlocalhost:9090)\n");
  printf("\t-e: Number of events to read from input data files\n");
  printf("\t-m: Enable memory leak debugging\n");
  printf("\t-g: Enable graphics display when processing data files\n");
  printf("\t-d: Enable user debug statements\n");
  printf("\n");
  printf("Example1: analyze online data: ./analyzer.exe -P9091\n");
  printf("Example2: analyze existing data: ./analyzer.exe /data/alpha/current/run00500.mid\n");
  exit(1);
}

// Main function call

int main(int argc, char *argv[])
{
	setbuf(stdout,NULL);
	setbuf(stderr,NULL);
 
	signal(SIGILL,  SIG_DFL);
	signal(SIGBUS,  SIG_DFL);
	signal(SIGSEGV, SIG_DFL);
 
	std::vector<std::string> args;
	for (int i=0; i<argc; i++)
	{
		if (strcmp(argv[i],"-h")==0)
			help(); // does not return
		args.push_back(argv[i]);
	}

	TApplication *app = new TApplication("rootana", &argc, argv);

	bool forceEnableGraphics = false;
	bool testMode = false;
	int  oldTcpPort = 0;
	int  tcpPort = 0;
	const char* hostname = NULL;
	const char* exptname = NULL;

	for (unsigned int i=1; i<args.size(); i++) // loop over the commandline options
	{
		const char* arg = args[i].c_str();
		//printf("argv[%d] is %s\n",i,arg);
	   
		if (strncmp(arg,"-e",2)==0)  // Event cutoff flag (only applicable in offline mode)
			gEventCutoff = atoi(arg+2);
		else if (strncmp(arg,"-m",2)==0) // Enable memory debugging
			gEnableShowMem = true;
		else if (strncmp(arg,"-p",2)==0) // Set the histogram server port
			oldTcpPort = atoi(arg+2);
		else if (strncmp(arg,"-P",2)==0) // Set the histogram server port
			tcpPort = atoi(arg+2);
		else if (strcmp(arg,"-T")==0)
			testMode = true;
		else if (strcmp(arg,"-d")==0)
			gDebugEnable = true;
		else if (strcmp(arg,"-g")==0)
			forceEnableGraphics = true;
		else if (strncmp(arg,"-H",2)==0)
			hostname = strdup(arg+2);
		else if (strncmp(arg,"-E",2)==0)
			exptname = strdup(arg+2);
		else if (strcmp(arg,"-h")==0)
			help(); // does not return
		else if (arg[0] == '-')
			help(); // does not return
	}

	if (!tcpPort) tcpPort = 9091;
    
	gROOT->cd();
	gOnlineHistDir = new TDirectory("rootana", "rootana online plots");

	if (tcpPort)
		StartNetDirectoryServer(tcpPort, gOnlineHistDir);
	 
	gIsOffline = false;

	for (unsigned int i=1; i<args.size(); i++)
	{
		const char* arg = args[i].c_str();

		if (arg[0] != '-')  
		{  
			gIsOffline = true;
			//gEnableGraphics = false;
			//gEnableGraphics |= forceEnableGraphics;
			ProcessMidasFile(app,arg);
		}
	}

	if (testMode)
	{
		gOnlineHistDir->cd();
		TH1D* hh = new TH1D("test", "test", 100, 0, 100);
		hh->Fill(1);
		hh->Fill(10);
		hh->Fill(50);

		app->Run(kTRUE);
		return 0;
	}

	// if we processed some data files,
	// do not go into online mode.
	if (gIsOffline)
		return 0;
	   
	gIsOffline = false;
	//gEnableGraphics = true;

#ifdef MIDASSYS
	ProcessMidasOnline(app, hostname, exptname);
#endif
   
	return 0;
}

//end
