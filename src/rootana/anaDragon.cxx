//
// ROOT analyzer
//
// K.Olchanski
//
// $Id: analyzer.cxx 64 2008-12-22 03:14:11Z olchansk $
//
#include "midas.h"
#include <stdio.h>
#include <sys/time.h>
#include <iostream>
#include <assert.h>
#include <signal.h>

#include "TMidasOnline.h"
#include "TMidasEvent.h"
#include "TMidasFile.h"
#include "XmlOdb.h"
#ifdef OLD_SERVER
#include "midasServer.h"
#endif
#ifdef HAVE_LIBNETDIRECTORY
#include "libNetDirectory/netDirectoryServer.h"
#endif

#include <TSystem.h>
#include <TROOT.h>
#include <TApplication.h>
#include <TTimer.h>
#include <TFile.h>
#include <TDirectory.h>
#include <TGClient.h>
#include <TGFrame.h>
#include <TFolder.h>
#include <TH1D.h>

#include "Globals.h"

#include "utils/definitions.h"
#include "midas/Event.hxx"
#include "Timestamp.hxx"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include "rootana/Histos.hxx"
#include "rootana/Events.hxx"
#include "midas/Database.hxx"
#include "dragon/Coinc.hxx"
#include <TRandom3.h>
#include <TCanvas.h>

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

//TCanvas  *gMainWindow = NULL; 	// the online histogram window

double GetTimeSec()
{
  struct timeval tv;
  gettimeofday(&tv,NULL);
  return tv.tv_sec + 0.000001*tv.tv_usec;
}

class MyPeriodic : public TTimer
{
public:
  typedef void (*TimerHandler)(void);

  int          fPeriod_msec;
  TimerHandler fHandler;
  double       fLastTime;

  MyPeriodic(int period_msec,TimerHandler handler)
  {
    assert(handler != NULL);
    fPeriod_msec = period_msec;
    fHandler  = handler;
    fLastTime = GetTimeSec();
    Start(period_msec,kTRUE);
  }

  Bool_t Notify()
  {
    double t = GetTimeSec();
    //printf("timer notify, period %f should be %f!\n",t-fLastTime,fPeriod_msec*0.001);

    if (t - fLastTime >= 0.9*fPeriod_msec*0.001)
      {
	//printf("timer: call handler %p\n",fHandler);
	if (fHandler)
	  (*fHandler)();

	fLastTime = t;
      }

    Reset();
    return kTRUE;
  }

  ~MyPeriodic()
  {
    TurnOff();
  }
};

struct doClear {
	void operator() (rootana::HistBase* p) { p->clear(); }
};

void startRun(int transition,int run,int time)
{
  gIsRunning = true;
  gRunNumber = run;
  //gIsPedestalsRun = gOdb->odbReadBool("/experiment/edit on start/Pedestals run");
  printf("Begin run: %d, pedestal run: %d\n", gRunNumber, gIsPedestalsRun);
    
  if(gOutputFile!=NULL)
  {
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
}

void endRun(int transition,int run,int time)
{
  gIsRunning = false;
  gRunNumber = run;

#ifdef OLD_SERVER
  if (gManaHistosFolder)
    gManaHistosFolder->Clear();
#endif

	rootana::EventHandler::Instance()->EndRun();

  if (gOutputFile)
    {
      gOutputFile->Write();
      gOutputFile->Close();		//close the histogram file
      gOutputFile = NULL;
    }

  printf("End of run %d\n",run);
}


void HandleSample(int ichan, void* ptr, int wsize)
{
  uint16_t *samples = (uint16_t*) ptr;
  int numSamples = wsize;

  if (numSamples != 512)
    return;

  char name[256];
  sprintf(name, "channel%d", ichan);

  if (gOutputFile)
    gOutputFile->cd();

  TH1D* samplePlot = (TH1D*)gDirectory->Get(name);

  if (!samplePlot)
    {
      printf("Create [%s]\n", name);
      samplePlot = new TH1D(name, name, numSamples, 0, numSamples);
      //samplePlot->SetMinimum(0);
#ifdef OLD_SERVER
      if (gManaHistosFolder)
        gManaHistosFolder->Add(samplePlot);
#endif
    }

  for(int ti=0; ti<numSamples; ti++)
    samplePlot->SetBinContent(ti, samples[ti]);
}

void HandleMidasEvent(const void* header, const void* pdata, int size)
{
	const EventHeader_t* head = reinterpret_cast<const EventHeader_t*>(header);
	switch (head->fEventId) {
	case DRAGON_HEAD_EVENT:  /// - DRAGON_HEAD_EVENT: Insert into timestamp matching queue
		gQueue.Push(midas::Event("TSCH", header, pdata, head->fDataSize));
		break;
	case DRAGON_TAIL_EVENT:  /// - DRAGON_TAIL_EVENT: Insert into timestamp matching queue
		gQueue.Push(midas::Event("TSCT", header, pdata, head->fDataSize));
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

inline void HandleMidasEvent(TMidasEvent& event)
{
	HandleMidasEvent(event.GetEventHeader(), event.GetData(), event.GetDataSize());
}

inline void eventHandler(const void*pheader,const void*pdata,int size)
{
  HandleMidasEvent(pheader, pdata, size);
}

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

	  startRun(0,event.GetSerialNumber(),0);
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
	  HandleMidasEvent(event);
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

  endRun(0,gRunNumber,0);

  // start the ROOT GUI event loop
  //  app->Run(kTRUE);

  return 0;
}

#ifdef HAVE_MIDAS

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

   midas->setTransitionHandlers(startRun,endRun,NULL,NULL);
   midas->registerTransitions();

   /* reqister event requests */

   midas->setEventHandler(eventHandler);
   midas->eventRequest("SYNC",-1,-1,(1<<1));

   /* fill present run parameters */

   gRunNumber = gOdb->odbReadInt("/runinfo/Run number");

   if ((gOdb->odbReadInt("/runinfo/State") == 3))
     startRun(0,gRunNumber,0);

	 std::ifstream ifs ("src/rootana/histos.dat");
	 assert (ifs.good());
	 std::string line, gDir;
	 while (std::getline(ifs, line)) {

		 rootana::HistBase* h = 0;
		 std::string sParam;
		 
		 if (line.find("DIR:") < line.size()) {
			 std::getline(ifs, line);
			 gDir = line;
			 std::cout << "new directory: " << gDir << "\n";
		 }

		 else if (line.find("SUMMARY_HIST:") < line.size()) {
			 std::string sHst, sNum;
			 assert (std::getline(ifs, sHst));
			 {
				 TString tmp(sHst); tmp.ReplaceAll("\t", " ");
				 sHst = tmp.Data();
			 }
			 assert (std::getline(ifs, sParam));
			 {
				 TString tmp(sParam); tmp.ReplaceAll("\t", " ");
				 sParam = tmp.Data();
			 }
			 assert (std::getline(ifs, sNum));
			 {
				 TString tmp(sNum); tmp.ReplaceAll("\t", " ");
				 sNum = tmp.Data();
			 }

			 std::stringstream cmdData;
			 cmdData << "rootana::DataPointer::New(" << sParam << ", " << sNum << ")";
			 rootana::DataPointer* data = (rootana::DataPointer*)gROOT->ProcessLineFast(cmdData.str().c_str());
			 assert(data);

			 std::stringstream cmdHst;
			 cmdHst << "rootana::HistBase::NewSummary(" << sHst << ", " << cmdData.str() << ")";
			 h = (rootana::HistBase*)gROOT->ProcessLineFast(cmdHst.str().c_str());
			 assert(h);
		 }
		 else if (line.find("HIST:") < line.size()) {
			 
			 std::string sHst;
			 assert (std::getline(ifs, sHst));
			 {
				 TString tmp(sHst); tmp.ReplaceAll("\t", " ");
				 sHst = tmp.Data();
			 }
			 assert (std::getline(ifs, sParam));
			 {
				 TString tmp(sParam); tmp.ReplaceAll("\t", " ");
				 sParam = tmp.Data();
			 }

			 std::stringstream cmdHst, cmdParam;
			 cmdHst << "new " << sHst;
			 cmdParam << "rootana::DataPointer::New(" << sParam << ")";

			 if (0) { }
			 else if (sHst.find("TH1D") < sHst.size()) {
				 TH1D* hst = (TH1D*)gROOT->ProcessLineFast(cmdHst.str().c_str());
				 assert (hst);
				 rootana::DataPointer* data = (rootana::DataPointer*)gROOT->ProcessLineFast(cmdParam.str().c_str());
				 assert (data);
				 h = new rootana::Hist<TH1D>(hst, data);
				 assert (h);
			 }
			 else if (sHst.find("TH2D") < sHst.size()) {
				 TH2D* hst = (TH2D*)gROOT->ProcessLineFast(cmdHst.str().c_str());
				 assert (hst);
				 rootana::DataPointer* datax = (rootana::DataPointer*)gROOT->ProcessLineFast(cmdParam.str().c_str());
				 assert (datax);
				 assert (std::getline(ifs, sParam));
				 {
					 TString tmp(sParam); tmp.ReplaceAll("\t", " ");
					 sParam = tmp.Data();
					 cmdParam.str("");
					 cmdParam << "rootana::DataPointer::New(" << sParam << ")";
				 }
				 rootana::DataPointer* datay = (rootana::DataPointer*)gROOT->ProcessLineFast(cmdParam.str().c_str());
				 assert(datay);

				 h = new rootana::Hist<TH2D>(hst, datax, datay);
				 assert (h);
			 }
			 else if (sHst.find("TH3D") < sHst.size()) {
				 // TH3D* hst = (TH3D*)gROOT->ProcessLineFast(cmdHst.str().c_str());
				 // assert (hst);
				 // rootana::DataPointer* data = (rootana::DataPointer*)gROOT->ProcessLineFast(cmdParam.str().c_str());
				 // assert (data);
				 // h = new rootana::Hist<TH3D>(hst, data);
				 // assert (h);
				 assert (!"TH3:: Not yet!");
			 }
			 else {
				 std::cerr << "Bad Hist line: " << sHst << "\n";
				 assert (0);
			 }
		 }

		 if (h) {
			 uint16_t type;
			 if (0) { }
			 else if (sParam.find("rootana::gHead")  < sParam.size())  type = DRAGON_HEAD_EVENT;
			 else if (sParam.find("rootana::gTail")  < sParam.size())  type = DRAGON_TAIL_EVENT;
			 else if (sParam.find("rootana::gCoinc") < sParam.size())  type = DRAGON_COINC_EVENT;
			 else { std::cout << "Bad Param:: " << sParam << "\n"; assert (0); }

			 rootana::EventHandler::Instance()->AddHisto(h, type, gOutputFile, gDir.c_str());
		 }
	 }
			 
   printf("Startup: run %d, is running: %d, is pedestals run: %d\n",gRunNumber,gIsRunning,gIsPedestalsRun);
   printf("Hostname: %s, exptname: %s\n", hostname, exptname);

   MyPeriodic tm(100,MidasPollHandler);
   //MyPeriodic th(1000,SISperiodic);
   //MyPeriodic tn(1000,StepThroughSISBuffer);
   //MyPeriodic to(1000,Scalerperiodic);

   /*---- start main loop ----*/

   //loop_online();
   app->Run(kTRUE);

   /* disconnect from experiment */
   midas->disconnect();

   return 0;
}

#endif

#include <TGMenu.h>

class MainWindow: public TGMainFrame {

private:
  TGPopupMenu*		menuFile;
  //TGPopupMenu* 		menuControls;
  TGMenuBar*		menuBar;
  TGLayoutHints*	menuBarLayout;
  TGLayoutHints*	menuBarItemLayout;
  
public:
  MainWindow(const TGWindow*w,int s1,int s2);
  virtual ~MainWindow(); // Closing the control window closes the whole program
  virtual void CloseWindow();
  
  Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
};

#define M_FILE_EXIT 0

Bool_t MainWindow::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
   // printf("GUI Message %d %d %d\n",(int)msg,(int)parm1,(int)parm2);
    switch (GET_MSG(msg))
      {
      default:
	break;
      case kC_COMMAND:
	switch (GET_SUBMSG(msg))
	  {
	  default:
	    break;
	  case kCM_MENU:
	    switch (parm1)
	      {
	      default:
		break;
	      case M_FILE_EXIT:
	        if(gIsRunning)
    		   endRun(0,gRunNumber,0);
		gSystem->ExitLoop();
		break;
	      }
	    break;
	  }
	break;
      }

    return kTRUE;
}

MainWindow::MainWindow(const TGWindow*w,int s1,int s2) // ctor
    : TGMainFrame(w,s1,s2)
{
   //SetCleanup(kDeepCleanup);
   
   SetWindowName("ROOT Analyzer Control");

   // layout the gui
   menuFile = new TGPopupMenu(gClient->GetRoot());
   menuFile->AddEntry("Exit", M_FILE_EXIT);

   menuBarItemLayout = new TGLayoutHints(kLHintsTop|kLHintsLeft, 0, 4, 0, 0);

   menuFile->Associate(this);
   //menuControls->Associate(this);

   menuBar = new TGMenuBar(this, 1, 1, kRaisedFrame);
   menuBar->AddPopup("&File",     menuFile,     menuBarItemLayout);
   //menuBar->AddPopup("&Controls", menuControls, menuBarItemLayout);
   menuBar->Layout();

   menuBarLayout = new TGLayoutHints(kLHintsTop|kLHintsLeft|kLHintsExpandX);
   AddFrame(menuBar,menuBarLayout);
   
   MapSubwindows(); 
   Layout();
   MapWindow();
}

MainWindow::~MainWindow()
{
    delete menuFile;
    //delete menuControls;
    delete menuBar;
    delete menuBarLayout;
    delete menuBarItemLayout;
}

void MainWindow::CloseWindow()
{
    if(gIsRunning)
    	endRun(0,gRunNumber,0);
    gSystem->ExitLoop();
}

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

#include <TStyle.h>
int main(int argc, char *argv[])
{
#if 1

	gStyle->SetPalette(1);

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

#if 0
   if(gROOT->IsBatch()) {
   	printf("Cannot run in batch mode\n");
	return 1;
   }
#endif

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
    
   MainWindow *mainWindow = NULL;

   if (!gROOT->IsBatch()) {
     mainWindow = new MainWindow(gClient->GetRoot(), 200, 300);
   }

   gROOT->cd();
   gOnlineHistDir = new TDirectory("rootana", "rootana online plots");

#ifdef OLD_SERVER
   if (oldTcpPort)
     StartMidasServer(oldTcpPort);
#else
   if (oldTcpPort)
     fprintf(stderr,"ERROR: No support for the old midas server!\n");
#endif
#ifdef HAVE_LIBNETDIRECTORY
   if (tcpPort)
     StartNetDirectoryServer(tcpPort, gOnlineHistDir);
#else
   if (tcpPort)
     fprintf(stderr,"ERROR: No support for the TNetDirectory server!\n");
#endif
	 
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

#ifdef HAVE_MIDAS
	ProcessMidasOnline(app, hostname, exptname);
#endif
   
   return 0;
#else
	
	 int x[9];
	 rootana::DataPointer* xx = rootana::DataPointer::New(x, 9);
	 rootana::Hist<TH2D> hist ("hist", "", 100, 0, 100, xx);

	 for (int i=0; i< 100000; ++i) {
		 for (int j=0; j< 9; ++j) {
			 x[j] = gRandom->Uniform(0, 100);
		 }
		 hist.fill();
	 }

	 hist->Draw("COLZ");
	 gPad->SaveAs("c1.png");

#endif
}

//end
