///
/// \file Constants.hxx
/// \brief Defines some useful physical constants
/// \note Future developers: if more constnats are needed, please extend this file rather than
///  hard-coding in your source files.
///
#ifndef DRAGON_CONSTANTS_HXX
#define DRAGON_CONSTANTS_HXX

namespace dragon {

/// Class containing useful physical constants as static members
/*! Example use:
 *  \code
 *  std::cout << "The electron charge in coulombs is: " << dragon::Constants::ElectronCharge() << "\n";
 *  \endcode
 *  \note All values taken from the `gsl_const` library which uses CODATA 2006 values.
 */
class Constants {
public:

  /// Electron charge in A*s
	static const double ElectronCharge() { return 1.602176487e-19; }

};

}



#endif
