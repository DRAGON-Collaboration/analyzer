///
/// \file Constants.hxx
/// \author D. Connolly
/// \author G. Christian
/// \brief Defines some useful physical constants
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
   *
   *  \attention Future developers: if more constnats are needed, please extend this file rather than hard-coding in your source files.
   *
   *  \note All values taken from 2014 CODATA values:
   *  <a href="http://physics.nist.gov/cuu/Constants/index.html">http://physics.nist.gov/cuu/Constants/index.html</a>
   *  These values should be updated in 2019 when the CODATA values are adjusted to adhere to the
   *  <a href="http://www.bipm.org/en/news/full-stories/2015-01/si-roadmap.html">revision of the International System of Units (SI)</a>.
   *
   *	\todo
   *  consider adding functionality (similar to TAtomicMassTable::ParseFile ) to read and parse a
   *  <a href="http://physics.nist.gov/cuu/Constants/Table/allascii.txt">text file</a> containing physical constant data.
   */

  class Constants {
  public:

	////////////////////////////////////////////////////////////////////////////////
	/// Avogadro's number 6.022140857(74)e23 mol<sup>-1</sup>
	static double Avogadro()   { return 6.022140857e23; }

	////////////////////////////////////////////////////////////////////////////////
	/// Avogadro uncertainty (mol<sup>-1</sup>)
	static double AvogadroUnc()   { return 0.000000074e23; }

	////////////////////////////////////////////////////////////////////////////////
	/// atomic mass unit (energy equivalent) 931.4940954(57) MeV c<sup>-2</sup>
	static double AMU()        { return 931.4940954; }

	////////////////////////////////////////////////////////////////////////////////
	/// atomic mass unit (energy equivalent) uncertainty (in MeV c<sup>-2</sup>)
	static double AMUUnc()     { return 0.0000057; }

	////////////////////////////////////////////////////////////////////////////////
	/// atomic mass unit 1.660539040(20) kg
	static double AMUkg() { return 1.660538921e-27; }

	////////////////////////////////////////////////////////////////////////////////
	/// atomic mass unit uncertainty (in kg)
	static double AMUkgUnc() { return 0.000000020e-27; }

	////////////////////////////////////////////////////////////////////////////////
	/// fine structure constant 7.2973525664(17)e-3
	static double Alpha() { return 7.2973525664e-3; }

	////////////////////////////////////////////////////////////////////////////////
	/// fine structure constant uncertainty
	static double AlphaUnc() { return 0.0000000017e-3; }

	////////////////////////////////////////////////////////////////////////////////
	/// Bohr radius in m 0.52917721067(12)
	static double BohrR() { return 0.52917721067e-10; }

	////////////////////////////////////////////////////////////////////////////////
	/// Bohr radius uncertainty
	static double BohrRUnc() { return 0.00000000012e-10; }

	////////////////////////////////////////////////////////////////////////////////
	/// Boltzmann constant 8.6173303(50)e-5 eV K<sup>-1</sup>
	static double Boltzmann() { return 8.6173303e-5; }

    ////////////////////////////////////////////////////////////////////////////////
    /// Boltzmann constant uncertainty
    static double BoltzUnc() { return 0.0000050e-5; }

    ////////////////////////////////////////////////////////////////////////////////
    /// DRAGON MD1 constant 48.15 MeV / T<sup>2</sup>
    static double CMD1() { return 48.15; }

    ////////////////////////////////////////////////////////////////////////////////
    /// Absolute uncertainty of DRAGON MD1 constant in MeV / T<sup>2</sup>
    static double CMD1Unc() { return 0.072225; }

	////////////////////////////////////////////////////////////////////////////////
	/// Electron charge 1.6021766208(98)e-19 A*s \note Implemented in ROOT's TMath::Qe(), but its value is outdated.
	static double ElectronCharge() { return 1.6021766208e-19; }

	////////////////////////////////////////////////////////////////////////////////
	/// Electron charge uncertainty
	static double ElectronChargeUnc() { return 0.0000000098e-19; }

	////////////////////////////////////////////////////////////////////////////////
	/// Electron charge squared in MeV*fm
	static double QeSquared() { return Alpha()*HbarC(); }

	////////////////////////////////////////////////////////////////////////////////
	/// Electron mass in MeV / c<sup>2</sup> 0.5109989461(31)
	static double ElectronMass() { return 0.5109989461; }

	////////////////////////////////////////////////////////////////////////////////
	/// Electron mass uncertainty
	static double EMassUnc() { return 0.0000000031; }

	////////////////////////////////////////////////////////////////////////////////
	/// Vacuum permittivity F*m<sup>-1</sup>
	static double Eps0() { return 1 / ( Mu0()*pow(TMath::C(),2) ); }

	////////////////////////////////////////////////////////////////////////////////
	/// hbar*c 197.3269788(12) MeV*fm
	static double HbarC() { return 197.3269788; }

	////////////////////////////////////////////////////////////////////////////////
	/// Hbar*c uncertainty
	static double HbarCUnc() { return 0.0000012; }

	////////////////////////////////////////////////////////////////////////////////
	/// Distance between DRAGON's gas target and focal plane m
	static double Ldra() { return 21.0; }

	////////////////////////////////////////////////////////////////////////////////
	/// Uncertainty in distance between DRAGON's gas target and focal plane m
    /// \attention <b>this is just a guess</b>.
	static double LdraUnc() { return 0.3; }

	////////////////////////////////////////////////////////////////////////////////
	/// Distance between DRAGON's MCP foils cm
	static double Lmcp() { return 0.59; }

	////////////////////////////////////////////////////////////////////////////////
	/// Uncertainty in distance between DRAGON's MCP foils m
	static double LmcpUnc() { return 0.005; }

	////////////////////////////////////////////////////////////////////////////////
	/// vacuum permeability N*A<sup>-2</sup>
	static double Mu0() { return 4*TMath::Pi()*1e-7; }

	////////////////////////////////////////////////////////////////////////////////
	/// Loschmidt constant at 273.15 K and 101.325 kPa 2.6867811(15)e19 cm<sup>-3</sup>
	static double Losch() { return 2.6867811e19; }

	////////////////////////////////////////////////////////////////////////////////
	/// Loschmidt constant uncertainty
	static double LoschUnc() { return 0.0000015; }

	////////////////////////////////////////////////////////////////////////////////
	/// NIST normal temperature in K
	static double T_std() { return 293.15; }

	////////////////////////////////////////////////////////////////////////////////
	/// standard atm in Torr
	static double P_std() { return 760; }

  };

} //namespace dragon

#endif
