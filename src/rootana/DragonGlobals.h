/// \file DragonGlobals.h
/// \author G. Christian
/// \brief Defines global instances of dragon::*
/// data structures for use in rootana.
#ifndef DRAGON_GLOBALS_H
#define DRAGON_GLOBALS_H
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

#undef EXTERN

#endif
