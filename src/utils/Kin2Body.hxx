////////////////////////////////////////////////////////////////////////////////
/// \file Kinematics.hxx
/// \author D. Connolly
/// \brief Defines methods and utilities to calculate and plot 2-body reaction
///  kinematics
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

  class Kin2Body {
  public:
	Kin2Body(Int_t Zb, Int_t Ab, Int_t Zt, Int_t At, Double_t ecm = 0, Int_t qb = 0, Int_t qt = 0);
	/// Get CM energy
	Double_t GetEcm() const { return fEcm; } // keV
	Double_t GetTbeam() const;  // keV
	Double_t GetV2beam() const; // keV/u
	Double_t GetEtarget() const;  // keV
	Double_t GetV2target() const; // keV/u
	/// Get beam mass in amu
	Double_t GetM1() const { return fM1 / (1e3*dragon::Constants::AMU()); }
	/// Get target mass in amu
	Double_t GetM2() const { return fM2 / (1e3*dragon::Constants::AMU()); }
    Double_t GetMaxAngle(Double_t mass);
    void Set4Mom() { fS = pow(fM1*fM1 + fM2*fM2 + fEcm, 2); }
    void SetRapidity() { fChi = log( (fPcm + sqrt(fM1*fM1+fPcm*fPcm) ) / fM1 );  }
    void SetPcm();
    void SetPcmPrime();
	void SetEcm(Double_t ecm);
	void SetTbeam(Double_t Tb);
	void SetV2beam(Double_t vb);
	void SetTtarget(Double_t Tt);
	void SetV2target(Double_t vt);
    /// Set excitation energy
    void SetEx() { fEx = fEcm + fQ; }
	/// Set beam mass in amu
	void SetM1(Double_t m1) { fM1 = 1e3*m1*dragon::Constants::AMU(); }
	/// Set target mass in amu
	void SetM2(Double_t m2) { fM2 = 1e3*m2*dragon::Constants::AMU(); }
  private:
	/// Ctor helper
	void Init(int Zb, int Ab, int Zt, int At, int Ze, int Ae, int Zr, int Ar, double ecm, int qb, int qt);
  private:
	/// Beam mass [keV/c^2]
	Double_t fM1;
	/// Target mass [keV/c^2]
	Double_t fM2;
	/// Ejectile mass [keV/c^2]
	Double_t fM3;
	/// Recoil mass [keV/c^2]
	Double_t fM4;
	/// Reaction Q value [keV]
	Double_t fQ;
	/// Center of mass kinetic energy [keV]
	Double_t fEcm;
    /// Invariant 4-momentum
    Double_t fS;
    /// CM momentum of projectile and target
    Double_t fPcm;
    /// CM momentum of ejectile and recoil
    Double_t fPprime;
    /// CM rapidity
    Double_t fChi;
  };

} // namespace dragon
