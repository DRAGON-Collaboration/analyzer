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
#include "dragon/Coinc.hxx"
#include "dragon/Scaler.hxx"
#include "utils/TStamp.hxx"

#ifndef G__DICTIONARY
/// Provide 'extern' linkage except in CINT dictionary
#define EXTERN extern
#define CREATE_SCALER_ARG(symbol, arg) extern dragon::Scaler symbol
#else
#define EXTERN
#define CREATE_SCALER_ARG(symbol, arg)        dragon::Scaler symbol(arg)
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
CREATE_SCALER_ARG(gHeadScaler, "head");

/// Global tail scaler class
CREATE_SCALER_ARG(gTailScaler, "tail");

}


#endif // end
