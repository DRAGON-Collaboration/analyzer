////////////////////////////////////////////////////////////////////////////////
/// \file Kin2Body.hxx
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
    Kin2Body(const char* projectile, const char* target, const char* ejectile, const char* recoil,
             Double_t enregy = 0, const char* frame = "CM", Int_t qb = 0);
      Kin2Body(Int_t Zb, Int_t Ab, Int_t Zt, Int_t At, Double_t ecm = 0, const char* frame = "CM", Int_t qb = 0);
	/// Get CM energy
	Double_t GetBrho()    const { return fBrho; } // keV
	Double_t GetEcm()     const { return fEcm;  } // keV
	Double_t GetEx()      const { return fEx;   } // keV
	Double_t GetLabTb()   const { return fTb;   } // keV
	Double_t GetLabTbA()  const { return fTbA;  } // A keV
	Double_t GetTtarget() const { return fTtgt; } // keV
	Double_t GetV2b()     const { return fV2b;  } // keV / u
	Double_t GetLabTej(Double_t theta);   // keV
	Double_t GetLabTrec(Double_t theta);  // keV
	/// Get beam mass in amu
	Double_t GetM1() const { return fM1 / (1e3*dragon::Constants::AMU()); }
	/// Get target mass in amu
	Double_t GetM2() const { return fM2 / (1e3*dragon::Constants::AMU()); }
    Double_t GetMaxAngle(Int_t which);
    void Set4Mom();
  private:
	/// Ctor helper for energy given in CM frame
	void Init(const char* projectile, const char* target, const char* ejectile, const char* recoil,
              Double_t energy, const char* frame, Int_t qb);
	/// Ctor helper
	void Init(int Zb, int Ab, int Zt, int At, int Ze, int Ae, int Zr, int Ar, double ecm, int qb);
  private:
	/// Beam nucloen number
	Double_t fA1;
	/// Beam magnetic rigidity in T*m
	Double_t fBrho;
    /// CM rapidity
    Double_t fChi;
	/// Center of mass kinetic energy [keV]
	Double_t fEcm;
	/// Beam mass [AMU]
	Double_t fM1amu;
	/// Beam mass [keV/c^2]
	Double_t fM1;
	/// Target mass [keV/c^2]
	Double_t fM2;
	/// Ejectile mass [keV/c^2]
	Double_t fM3;
	/// Recoil mass [keV/c^2]
	Double_t fM4;
    /// CM momentum of projectile and target
    Double_t fPcm;
    /// CM momentum of ejectile and recoil
    Double_t fPprime;
	/// Reaction Q value [keV]
	Double_t fQ;
    /// Invariant 4-momentum
    Double_t fS;
    /// Total kinetic energy of the beam in the laboratory frame keV
    Double_t fTb;
    /// Total kinetic energy of the beam in the laboratory frame A keV
    Double_t fTbA;
    /// Total kinetic energy of the the target frame
    Double_t fTtarget;
    /// Total kinetic energy of the the beam in keV / u
    Double_t fV2b;
  };

} // namespace dragon
