///
/// \file rbsymbols.hxx
/// \brief Header file for functions / classes that should be imported into CINT.
///
#ifndef RB_SYMBOLS_HXX
#define RB_SYMBOLS_HXX
#include <RTypes.h>

namespace rbdragon {

/// Turn on/off histogram auto-zeroing at run start.
void SetAutoZero(Bool_t on = kTRUE);

/// Check if auto-zeroing is on or off
Bool_t GetAutoZero();

}


#endif
