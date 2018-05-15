////////////////////////////////////////////////////////////////////////////////
/// \file Kin2Body.hxx
/// \author D. Connolly
/// \brief Defines methods and utilities to calculate and plot 2-body reaction
///  kinematics
////////////////////////////////////////////////////////////////////////////////

#ifndef KIN2BODY_HXX
#define KIN2BODY_HXX
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

#include "Uncertainty.hxx"
#include "Constants.hxx"
#include "AutoPtr.hxx"
#include "Dragon.hxx"

// class TGraph;
// class TGraphErrors;

namespace dragon {

  class Kin2Body {
  public:
    Kin2Body(const char* projectile, const char* target, const char* ejectile, const char* recoil,
             double enregy = 0, const char* frame = "CM", int qb = 0);
	/// Get CM energy
	double GetBrho()    const { return fBrho; } // MeV
	double GetEcm()     const { return fEcm;  } // MeV
	double GetEx()      const { return fEx;   } // MeV
	double GetLabTb()   const { return fTb;   } // MeV
	double GetLabTbA()  const { return fTbA;  } // A MeV
	double GetTtarget() const { return fTtgt; } // MeV
	double GetV2b()     const { return fV2b;  } // MeV / u
	double GetLabTej(double theta);   // MeV
	double GetLabTrec(double theta);  // MeV
	/// Get beam mass in amu
	double GetM1() const { return fM1 / (1e3*dragon::Constants::AMU()); }
	/// Get target mass in amu
	double GetM2() const { return fM2 / (1e3*dragon::Constants::AMU()); }
    double GetMaxAngle(int which);
  // private:
	void Init(const char* projectile, const char* target, const char* ejectile, const char* recoil,
              double energy, const char* frame, int qb);
    void Set4Mom(double energy, const char* frame);
  // private:
	/// Beam nucloen number
	int fA1;
	/// Beam magnetic rigidity in T*m
	double fBrho;
    /// CM rapidity
    double fChi;
	/// Center of mass kinetic energy [MeV]
	double fEcm;
	/// Excitation energy [MeV]
	double fEx;
	/// Beam mass [AMU]
	double fM1amu;
	/// Beam mass [MeV/c^2]
	double fM1;
	/// Target mass [MeV/c^2]
	double fM2;
	/// Ejectile mass [MeV/c^2]
	double fM3;
	/// Recoil mass [MeV/c^2]
	double fM4;
    /// momentum of projectile
    double fPb;
    /// CM momentum of projectile and target
    double fPcm;
    /// CM momentum of ejectile and recoil
    double fPprime;
	/// Reaction Q value [MeV]
	double fQrxn;
	/// Beam charge state
	int fqb;
    /// Invariant 4-momentum
    double fS;
    /// Total kinetic energy of the beam in the laboratory frame MeV
    double fTb;
    /// Total kinetic energy of the beam in the laboratory frame A MeV
    double fTbA;
    /// Total kinetic energy of the the target frame
    double fTtgt;
    /// Total kinetic energy of the the beam in MeV / u
    double fV2b;
  };

} // namespace dragon

#endif
