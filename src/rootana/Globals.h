//
// Global variables for the ROOT analyzer
//
// Name: Globals.h
//
// $Id$
//

// Run parameters

extern int  gRunNumber;
extern bool gIsRunning;
extern bool gIsPedestalsRun;
extern bool gIsOffline;
extern bool gDebugEnable;
//extern int gUserMod;
//extern int gUserChan;

// Output files

extern TFile* gOutputFile;

// ODB access

#include "VirtualOdb.h"

extern VirtualOdb* gOdb;

// ADC data
extern std::vector<int> chADC;
extern std::vector<double> dataADC;

extern double gADC[];

// TDC data
extern std::vector<int> chTDC;
extern std::vector<double> dataTDC;

// end

