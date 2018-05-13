///
/// \file RootAnalysis.cxx
/// \author G. Christian
/// \brief Implements RootAnalysis.hxx
///
#include <vector>
#include <string>
#include <cassert>
#include <sstream>
#include <numeric>
#include <algorithm>

#include <TMath.h>
#include <TClass.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TString.h>
#include <TSystem.h>
#include <TThread.h>
#include <TRegexp.h>
#include <TFitResult.h>
#include <TDataMember.h>
#include <TTreeFormula.h>

#include "midas/Database.hxx"
#include "utils/Functions.hxx"
#include "Dragon.hxx"
#include "Constants.hxx"
#include "ErrorDragon.hxx"
#include "LinearFitter.hxx"
#include "TAtomicMass.h"
#include "RootAnalysis.hxx"

namespace dutils = dragon::utils;

////////////////////////////////////////////////////////////////////////////////
//////////////////////// Class dragon::Kin2Body ///////////////////////////////////
/// Helper class for lab -> CM conversions (fully relativistic)
/*!
 * Calulations are fully relativistic:
 * For a moving beam (1) and stationary target (2):
 *       Ecm^2 = m1^2 + m2^2 + 2*m2*E1
 * or in terms of kinetic energy:
 *       (Ecm + m1 + m2)^2 = m1^2 + m2^2 + 2*m2*(m1 + T1)
 */

////////////////////////////////////////////////////////////////////////////////
/// Ctor, w/ CM energy specification
/// Sets beam and target masses from AME12 compilation.
/// Energies are initially unset, use SetE*() functions to specify an
/// energy.
/// \param Zbeam Beam charge
/// \param Abeam Beam mass number
/// \param Ztarget Target charge
/// \param Atarget Target mass number
/// \param ecm Center-of-mass kinetic energy in keV
/// \attention Uses "nuclear" (fully ionized) masses.

dragon::Kin2Body::Kin2Body(Int_t Zb, Int_t Ab, Int_t Zt, Int_t At, Int_t Ze, Int_t Ae,
                           Int_t Zr, Int_t Ar, Double_t ecm, Int_t qb, Int_t qt)
{
  Init(Zb, Ab, Zt, At, Ze, Ae, Zr, Ar, ecm, qb, qt);
}

////////////////////////////////////////////////////////////////////////////////
/// Initialize variables

void dragon::Kin2Body::Init(Int_t Zb, Int_t Ab, Int_t Zt, Int_t At, Int_t Ze,
                            Int_t Ae, Int_t Zr, Int_t Ar, Double_t ecm,
                            Int_t qb, Int_t qt)
{
  TAtomicMassTable mt; // AME16
  fM1     = mt.IonMass(Zb, Ab, qb);
  fM2     = mt.IonMass(Zt, At, qt);
  fM3     = mt.NuclearMass(Ze, Ae);
  fM4     = mt.NuclearMass(Zr, Ar);
  fQ      = mt.Qvalue(Zt, At, Zb, Ab, Ze, Ae, kFALSE);
  fEcm    = ecm;
  fEx     = SetEx();
  fS      = Set4Mom();
  fChi    = SetRapidity();
  fPcm    = SetPcm();
  fPprime = SetPcmPrime();
}

////////////////////////////////////////////////////////////////////////////////
/// Set CM energy
/// \param ecm Center-of-mass kinetic energy in keV

void dragon::Kin2Body::SetEcm(Double_t ecm)
{
  fEcm = ecm;
}

////////////////////////////////////////////////////////////////////////////////
/// Set CM momentum of projectile and target

void dragon::Kin2Body::SetPcm()
{
  fPcm = sqrt( (pow(fS - fM1*fM1 - fM2*fM2, 2) - 4*pow(fM2*fM2, 2) ) / (4*fS) );
}

////////////////////////////////////////////////////////////////////////////////
/// Set CM momentum of ejectile and recoil

void dragon::Kin2Body::SetPcmPrime()
{
  fPprime = sqrt( (pow(fS - fM1*fM1 - fM2*fM2, 2) - 4*pow(fM2*fM2, 2) ) / (4*fS) );
}

////////////////////////////////////////////////////////////////////////////////
/// Set lab frame beam energy in keV
/// \param ebeam Beam energy in keV

void dragon::Kin2Body::SetTbeam(Double_t Tb)
{
  Double_t E1tot = Tb + fM1; // total energy
  Double_t Ecmtot = sqrt(fM1*fM1 + fM2*fM2 + 2*fM2*E1tot);
  fEcm = Ecmtot - fM1 - fM2;
}

////////////////////////////////////////////////////////////////////////////////
/// Set squred velocity of the beam in keV/u
/// \param ebeam Beam energy in keV/u

void dragon::Kin2Body::SetV2beam(Double_t vb)
{
  SetTbeam(vb*fM1/dragon::Constants::AMU()); // keV/u -> keV
}

////////////////////////////////////////////////////////////////////////////////
/// Set target frame energy in keV
/// \param etarget Target energy in keV

void dragon::Kin2Body::SetEtarget(Double_t Tt)
{
  Double_t E2tot = Tt + fM2; // total energy
  Double_t Ecmtot = sqrt(fM1*fM1 + fM2*fM2 + 2*fM1*E2tot);
  fEcm = Ecmtot - fM1 - fM2;
}

////////////////////////////////////////////////////////////////////////////////
/// Set target frame energy in keV/u
/// \param etarget Target energy in keV/u

void dragon::Kin2Body::SetV2target(Double_t vt)
{
  SetTbeam(vt*fM2/dragon::Constants::AMU()); // keV/u -> keV
}

////////////////////////////////////////////////////////////////////////////////
/// Get Maximum half angle of particle
/// \param which particle index (1 = projectile, 2 = target, 3 = ejectile,
///              4 = recoil)

Double_t dragon::Kin2Body::GetMaxAngle(Int_t which)
{
  if (which == 1){
    return asin(fPprime / (fM1*sinh(fChi)));
  }else if (which == 2){
    return asin(fPprime / (fM2*sinh(fChi)));
  }else if (which == 3){
    return asin(fPprime / (fM3*sinh(fChi)));
  }else if (which == 4){
    return asin(fPprime / (fM4*sinh(fChi)));
  } else {
    cout << "__Error!__: Invalid particle index (must be between 1 and 4).\n";
    return 0;
  }
}

////////////////////////////////////////////////////////////////////////////////
/// Get squared velocity of the beam in keV / u

Double_t dragon::Kin2Body::GetV2beam() const
{
  return GetTbeam() / (fM1/dragon::Constants::AMU());
}

////////////////////////////////////////////////////////////////////////////////
/// Get lab frame kinetic energy of the beam in keV

Double_t dragon::Kin2Body::GetTbeam() const
{
  Double_t Ecm = fEcm + fM1 + fM2; // total energy
  Double_t E1 = (Ecm*Ecm - fM1*fM1 - fM2*fM2) / (2*fM2); // total energy
  Double_t T1 = E1 - fM1; // kinetic energy
  return T1;
}

////////////////////////////////////////////////////////////////////////////////
/// Get target frame squred velocity in keV/u

Double_t dragon::Kin2Body::GetV2target() const
{
  return GetEtarget() / (fM2/dragon::Constants::AMU());
}

////////////////////////////////////////////////////////////////////////////////
/// Get target frame energy in keV

Double_t dragon::Kin2Body::GetEtarget() const
{
  Double_t Ecm = fEcm + fM1 + fM2; // total energy
  Double_t E2 = (Ecm*Ecm - fM1*fM1 - fM2*fM2) / (2*fM1); // total energy
  Double_t T2 = E2 - fM2; // kinetic energy
  return T2;
}
