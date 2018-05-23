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
#include <TGraph.h>
#include <TMultiGraph.h>

#include "Uncertainty.hxx"
#include "Constants.hxx"
#include "AutoPtr.hxx"
#include "Dragon.hxx"
#include "TAtomicMass.h"

class TGraph;
class TMultiGraph;
class TAtomicMass;

namespace dragon {

  class Kin2Body {
  public:
      // private:
	/// Beam velocity in units of c
	Double_t fBeta_b;
	/// Recoil velocity in units of c (0&deg; &gamma;)
	Double_t fBeta0rec;
	/// Recoil velocity in units of c (90&deg; &gamma;)
	Double_t fBeta90rec;
	/// Recoil velocity in units of c (180&deg; &gamma;)
	Double_t fBeta180rec;
	/// Beam magnetic rigidity in T*m
	Double_t fBrho;
    /// CM rapidity
    Double_t fChi;
	/// Total energy of beam nuclide [MeV]
	Double_t fEb;
	/// Center of mass kinetic energy [MeV]
	Double_t fEcm;
	/// Total energy of recoil nuclide (0&deg; &gamma;) [MeV]
	Double_t fE0rec;
	/// Total energy of recoil nuclide (90&deg; &gamma;) [MeV]
	Double_t fE90rec;
	/// Total energy of recoil nuclide (180&deg; &gamma;) [MeV]
	Double_t fE180rec;
	/// Excitation energy [MeV]
	Double_t fEx;
	/// Lorentz factor of beam [MeV]
	Double_t fGamma_b;
	/// Lorentz factor of recoil (0&deg; &gamma;) [MeV]
	Double_t fGamma0rec;
	/// Lorentz factor of recoil (90&deg; &gamma;) [MeV]
	Double_t fGamma90rec;
	/// Lorentz factor of recoil (180&deg; &gamma;) [MeV]
	Double_t fGamma180rec;
	/// Beam mass [AMU]
	Double_t fM1amu;
	/// Beam mass [MeV/c^2]
	Double_t fM1;
	/// Target mass [MeV/c^2]
	Double_t fM2;
	/// Ejectile mass [MeV/c^2]
	Double_t fM3;
	/// Recoil mass [MeV/c^2]
	Double_t fM4;
	/// Beam MCP TOF [ns]
	Double_t fMcpTofb;
	/// Recoil MCP TOF (0&deg; &gamma;) [MeV] [ns]
	Double_t fMcpTofrec0;
	/// Recoil MCP TOF (90&deg; &gamma;) [MeV] [ns]
	Double_t fMcpTofrec90;
	/// Recoil MCP TOF (180&deg; &gamma;) [MeV] [ns]
	Double_t fMcpTofrec180;
    /// momentum of projectile
    Double_t fPb;
    /// CM momentum of projectile and target
    Double_t fPcm;
    /// CM momentum of ejectile and recoil
    Double_t fPprime;
	/// Reaction Q value [MeV]
	Double_t fQrxn;
	/// Beam charge state
	Int_t fqb;
    /// Invariant 4-momentum
    Double_t fS;
	/// Beam separator TOF [ns]
	Double_t fSepTofb;
	/// Recoil separator TOF (0&deg; &gamma;) [&mu;s]
	Double_t fSepTofrec0;
	/// Recoil separator TOF (90&deg; &gamma;) [&mu;s]
	Double_t fSepTofrec90;
	/// Recoil separator TOF (180&deg; &gamma;) [&mu;s]
	Double_t fSepTofrec180;
    /// Total kinetic energy of the beam in the laboratory frame MeV
    Double_t fTb;
    /// Total kinetic energy of the beam in the laboratory frame A MeV
    Double_t fTbA;
    /// Total kinetic energy of the the target frame
    Double_t fTtgt;
    /// Total kinetic energy of the the beam in MeV / u
    Double_t fV2b;
    /// Projectile Nuclide
    const TAtomicMassTable::Nucleus_t* fProj;
    /// Target Nuclide
    const TAtomicMassTable::Nucleus_t* fTgt;
    /// Ejectile Nuclide
    const TAtomicMassTable::Nucleus_t* fEj;
    /// Recoil Nuclide
    const TAtomicMassTable::Nucleus_t* fRec;
    /// Reaction string
    const char* fRxnString;
    /// Projectile string
    const char* fProjString;
    /// Projectile string
    const char* fTgtString;
    /// Projectile string
    const char* fEjString;
    /// Projectile string
    const char* fRecString;

  public:
    /// Default ctor for radiative capture
    Kin2Body(const char* projectile, const char* target,
             Double_t enregy = 0, const char* frame = "CM", Int_t qb = 0);
    /// Overload ctor for particle ejectiles
    Kin2Body(const char* projectile, const char* target, const char* ejectile,
             Double_t enregy = 0, const char* frame = "CM", Int_t qb = 0);
    Double_t CalcTLabTheta(Double_t theta, const char* which, Bool_t negative = kFALSE);
	/// Get CM energy
    Double_t GetMaxAngle(const char* which);
    TMultiGraph* PlotTLabvsThetaLab(Option_t *option_e = "", Option_t *option_r = "");
  private:
	void Init(const char* projectile, const char* target,
              Double_t energy, const char* frame, Int_t qb);
	void Init(const char* projectile, const char* target, const char* ejectile,
              Double_t energy, const char* frame, Int_t qb);
    void Set4Mom(Double_t energy, const char* frame);
  };

} // namespace dragon

#endif
