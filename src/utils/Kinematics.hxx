////////////////////////////////////////////////////////////////////////////////
/// \file Kinematics.hxx
/// \author D. Connolly
/// \brief Defines classes and utilities to calculate reaction kinematics
////////////////////////////////////////////////////////////////////////////////

#ifndef DRAGON_ROOT_ANALYSIS_HEADER
#define DRAGON_ROOT_ANALYSIS_HEADER
#include <map>
#include <memory>
#include <fstream>
#ifndef __MAKECINT__
#include <iostream>
#endif
#include <algorithm>

#include <TH1.h>
#include <TCut.h>
#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TSelector.h>

#include "midas/libMidasInterface/TMidasStructs.h"
#include "Uncertainty.hxx"
#include "Constants.hxx"
#include "AutoPtr.hxx"
#include "Dragon.hxx"
#include "Vme.hxx"

class TGraph;
class TGraphErrors;

namespace dragon {

  enum MeasurementType_t {
	kGammaSingles = 1,
	kHiSingles    = 3,
	kCoinc        = 5
  };

////////////////////////////////////////////////////////////////////////////////
/// Helper class for reaction kinematics calculations
/*!
 * Calulations are fully relativistic:
 * For a moving beam (1) and stationary target (2):
 *       Ecm^2 = m1^2 + m2^2 + 2*m2*E1
 * or in terms of kinetic energy:
 *       (Tcm + m1 + m2)^2 = m1^2 + m2^2 + 2*m2*(m1 + T1)
 */

class Kinematics {
public:
	/// Ctor
	LabCM(int Zbeam, int Abeam, int Ztarget, int Atarget);
	/// Ctor, w/ CM energy specification
	LabCM(int Zbeam, int Abeam, int Ztarget, int Atarget, double ecm);
	/// Ctor, using amu masses
	LabCM(double mbeam, double mtarget, double ecm = 0.);
	/// Get CM energy
	double GetEcm() const { return fTcm; } // keV
	/// Get beam energy in keV
	double GetEbeam() const;  // keV
	/// Get beam energy in keV/u
	double GetV2beam() const; // keV/u
	/// Get target frame energy in keV
	double GetEtarget() const;  // keV
	/// Get target frame energy in keV/u
	double GetV2target() const; // keV/u
	/// Get beam mass in amu
	double GetM1() const { return fM1 / dragon::Constants::AMU(); }
	/// Get target mass in amu
	double GetM2() const { return fM2 / dragon::Constants::AMU(); }
	/// Set CM energy
	void SetEcm(double ecm);
	/// Set beam energy in keV
	void SetEbeam(double ebeam);
	/// Set beam energy in keV/u
	void SetV2beam(double ebeam);
	/// Set target energy in keV
	void SetEtarget(double etarget);
	/// Set target energy in keV/u
	void SetV2target(double etarget);
	/// Set beam mass in amu
	void SetM1(double m1) { fM1 = m1*dragon::Constants::AMU(); }
	/// Set target mass in amu
	void SetM2(double m2) { fM2 = m2*dragon::Constants::AMU(); }
private:
	/// Ctor helper
	void Init(int Zbeam, int Abeam, int Ztarget, int Atarget, double ecm);
private:
	/// Beam mass [keV/c^2]
	double fM1;
	/// Target mass [keV/c^2]
	double fM2;
	/// Center of mass kinetic energy [keV]
	double fTcm;
};

} // namespace dragon
