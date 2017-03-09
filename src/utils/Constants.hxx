///
/// \file Constants.hxx
/// \brief Defines some useful physical constants
/// \note Future developers: if more constnats are needed, please extend this file rather than
///  hard-coding in your source files.
///
#ifndef DRAGON_CONSTANTS_HXX
#define DRAGON_CONSTANTS_HXX

#include "TMath.h"

namespace dragon {

/// Class containing useful physical constants as static members
/*! Example use:
 *  \code
 *  std::cout << "The electron charge in coulombs is: " << dragon::Constants::ElectronCharge() << "\n";
 *  \endcode
 *  \note All values taken from 2014 CODATA values:
 *  <a href="http://physics.nist.gov/cuu/Constants/index.html">http://physics.nist.gov/cuu/Constants/index.html</a>
 *  \These values should be updated in 2018 the CODATA values are adjusted to the revised SI units. See
 *  \http://www.bipm.org/en/news/full-stories/2015-01/si-roadmap.html for details.
 *
 *	\todo
 *  consider adding functionality to read and parse a <a href="http://physics.nist.gov/cuu/Constants/Table/allascii.txt">text file</a>
 *  containing physical constant data (much like
 *  \code TAtomicMass \endcode
 *  \endtodo
 */
class Constants {
public:

	/// Avogadro's number 6.022140857(74)e23 mol^-1
	static const double Avogadro()   { return 6.022140857e23; }

	/// AMU in MeV/c^2
	static const double AMU()   { return 931.4940954; }

	/// AMU uncertainty in MeV/c^2
	static const double AMUUnc() { return 0.0000057; } //MeV

	/// AMU in kilograms
	static const double AMUkg() { return 1.660538921e-27; }

	/// fine structure constant 7.2973525664(17)e-3
	static const double Alpha() { return 7.2973525664e-3; }

	/// fine structure constant uncertainty
	static const double AlphaUnc() { return 0.0000000017e-3; }

	/// Bohr radius in m 0.52917721067(12)
	static const double RohrR() { return 0.52917721067e-10; }

	/// Bohr radius uncertainty
	static const double RohrRUnc() { return 0.00000000012e-10; }

	/// Boltzmann constant in eV K^-1
	static const double Boltzmann() { return 8.6173303e-5; }

	/// Boltzmann constant uncertainty
	static const double BoltzUnc() { return 0.0000050e-5; }

	/// Electron charge in A*s \note Implemented in TMath::Qe(), but outdated
	static const double ElectronCharge() { return 1.6021766208e-19; }

	/// Electron charge uncertainty
	static const double ElectronChargeUnc() { return 0.0000000098e-19; }

	/// Electron charge squared in MeV*fm
	static const double QeSquared() { return Alpha()*HbarC(); } // MeV*fm

	/// Electron mass in MeV/c^2 0.5109989461(31)
	static const double ElectronMass() { return 0.5109989461; }

	/// Electron mass uncertainty
	static const double EMassUnc() { return 0.0000000031; }

	/// Epsilon naught
	static const double Eps0() { return 1/(pow(TMath::C(),2)*Mu0())*ElectronCharge()*1e-9; }

	/// Hbar*c 197.3269788(12) MeV*fm
	static const double HbarC() { return 197.3269788; } // MeV*fm

	/// Hbar*c uncertainty
	static const double HbarCUnc() { return 0.0000012; } // MeV*fm

	/// vacuum permeability N*A^{-2}
	static const double Mu0() { return 4*TMath::Pi()*1e-7; }

	/// Loschmidt constant at 273.15 K and 101.325 kPa 2.6867811(15)e19 cm^-3
	static const double Losch() { return 2.6867811e19; } //cm^-3

	/// Loschmidt constant uncertainty
	static const double LoschUnc() { return 0.0000015; }

	/// NIST normal temperature in K
	static const double T_std() { return 293.15; } //K

	/// standard atm in Torr
	static const double P_std() { return 760; } //Torr

};

}

#endif
