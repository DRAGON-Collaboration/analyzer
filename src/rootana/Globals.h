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

// DRAGON Globals //
#include "Dragon.hxx"
#include "TStamp.hxx"

#ifndef G__DICTIONARY
/// Provide 'extern' linkage except in CINT dictionary
#define EXTERN extern
#else
#define EXTERN
#endif

namespace rootana {

/// Global timestamp diagnostics class
EXTERN tstamp::Diagnostics gDiagnostics;

/// Gloal gamma event class
EXTERN dragon::Head gHead;

/// Global heavy-ion event class
EXTERN dragon::Tail gTail;

/// Global coincidence event class
EXTERN dragon::Coinc gCoinc;

/// Global head scaler class
EXTERN dragon::Scaler gHeadScaler;

/// Global tail scaler class
EXTERN dragon::Scaler gTailScaler;

/// Global epics clas
EXTERN dragon::Epics gEpics;

}


#endif // end
