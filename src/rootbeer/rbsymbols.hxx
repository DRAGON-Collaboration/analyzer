///
/// \file rbsymbols.hxx
/// \brief Header file for functions / classes that should be imported into CINT.
///
#ifndef RB_SYMBOLS_HXX
#define RB_SYMBOLS_HXX
#include <RTypes.h>

namespace rbdragon {

/// \brief Turn on/off histogram auto-zeroing at run start.
/// \param level Integer specifying auto zero level:
///    - 0: No auto zeroing
///    - 1: Auto zero scalers only
///    - 2: Auto zero all histograms
void SetAutoZero(Int_t level = 1);

/// Check auto-zeroing level
Int_t GetAutoZero();

/// Turn on/off reading auto zero from ODB
void SetAutoZeroOdb(bool on);

/// Check SetAutoZeroOdb()
bool GetAutoZeroOdb();

}


#endif
