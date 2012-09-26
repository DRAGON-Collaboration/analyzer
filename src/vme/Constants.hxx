//! \file Constants.hxx
//! \author G. Christian
//! \brief Defines VME-related constant values.
#ifndef  DRAGON_VME_CONSTANTS_HXX
#define  DRAGON_VME_CONSTANTS_HXX
#include <stdint.h>

/// Encloses all VME related classes
namespace vme {

#ifdef VME_NO_DATA

/// "Empty" code used when no data is present in a measurement channel
/*!
 * \note #defining VME_NO_DATA as a value globally chooses vme::NONE; by default it's -1
 */
const int16_t NONE = VME_NO_DATA; 

#else

/// "Empty" code used when no data is present in a measurement channel
/*!
 * \note #defining VME_NO_DATA as a value globally chooses vme::NONE; by default it's -1
 */
const int16_t NONE = -1;

#endif

}


#endif
