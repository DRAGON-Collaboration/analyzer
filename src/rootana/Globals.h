///
/// \file Globals.h
///
/// \brief Global variables for the ROOT analyzer
///
/// \authors K. Olchanski
/// \authors G. Christian
///
#ifndef ROOTANA_GLOBALS_H
#define ROOTANA_GLOBALS_H

class TFile;

// Run parameters

extern int  gRunNumber;
extern bool gIsRunning;
extern bool gIsPedestalsRun;
extern bool gIsOffline;
extern bool gDebugEnable;

// Output files

extern TFile* gOutputFile;

// ODB access

#include "VirtualOdb.h"

extern VirtualOdb* gOdb;


// DRAGON Globals //
#include "dragon/Coinc.hxx"

#ifndef G__DICTIONARY
/// Provide 'extern' linkage except in CINT dictionary
#define EXTERN extern
#else
#define EXTERN
#endif

namespace rootana {

/// Gloal gamma event class
EXTERN dragon::Head gHead;

/// Global heavy-ion event class
EXTERN dragon::Tail gTail;

/// Global coincidence event class
EXTERN dragon::Coinc gCoinc;

}


#endif // end
