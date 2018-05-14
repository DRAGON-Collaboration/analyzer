////////////////////////////////////////////////////////////////////////////////
/// \file Kin2Body.cxx
/// \author D. Connolly
/// \brief Implements Kin2Body.hxx
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
//////////////////////// Class dragon::Kin2Body ////////////////////////////////
/// Class for relativistic 2 body reaction kinematics
/*!
 * Calculates kinematic relationships for 2 body reactions.
 * Consider the reaction
 * \f{equation}{
 *   m_2(m_1,m_3)m_4
 * \f}
 * where, by convention, the projectile with mass \f$m_1\f$ and kinetic energy
 * \f$T_b\f$ and is incident on a (stationary) target of mass \f$m_2\f$. The
 * kinematics of this reaction can be described by its (invariant) squared
 * 4-momentum
 * \f{equation}{
 *   S = \left(\sum_i E_i\right)^2 - \left(\sum_i \vec{p}_{i}\right)^2
 * \f}
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
/// \param energy beam energy in units corresponding to specified frame string
/// \param frame string specifying frame / units of beam energy
/// \param qb charge state of beam (default = 0)

dragon::Kin2Body::Kin2Body(const char* projectile, const char* target, const char* ejectile,
                           const char* recoil, Double_t energy, const char* frame, Int_t qb)
{
  InitCM(projectile, target, ejectile, recoil, energy, frame, qb);
}

////////////////////////////////////////////////////////////////////////////////
/// Ctor, w/ CM energy specification
/// Sets beam and target masses from AME12 compilation.
/// Energies are initially unset, use SetE*() functions to specify an
/// energy.
/// \param Zb beam proton number
/// \param Ab beam mass number
/// \param Zt target proton number
/// \param At target mass number
/// \param Ze ejectile proton number
/// \param Ae ejectile mass number
/// \param Zr recoil proton number
/// \param Ar recoil mass number
/// \param ecm Center-of-mass kinetic energy in keV
/// \param qb beam charge state

dragon::Kin2Body::Kin2Body(Int_t Zb, Int_t Ab, Int_t Zt, Int_t At, Int_t Ze, Int_t Ae,
                           Int_t Zr, Int_t Ar, Double_t ecm, Int_t qb)
{
  Init(Zb, Ab, Zt, At, Ze, Ae, Zr, Ar, ecm, qb);
}

////////////////////////////////////////////////////////////////////////////////
/// Initialize variables

void dragon::Kin2Body::Init(const char* projectile, const char* target, const char* ejectile,
                              const char* recoil, Double_t energy, Int_t qb)
{
  TAtomicMassTable mt; // AME16
  TAtomicMassTable::Nucleus_t proj = mt.GetNucleus(projectile);
  TAtomicMassTable::Nucleus_t tgt  = mt.GetNucleus(target);
  TAtomicMassTable::Nucleus_t ej   = mt.GetNucleus(ejectile);
  TAtomicMassTable::Nucleus_t rec  = mt.GetNucleus(recoil);
  fA1      = proj->fA;
  fM1      = mt.IonMass(proj->fZ, proj->fZ, qb);
  fM1amu   = mt.IonMassAMU(proj->fZ, proj->fZ, qb);
  fM2      = mt.IonMass(tgt->fZ, tgt->fA, 0);
  fM3      = mt.NuclearMass(ej->fZ, ej->fA);
  fM4      = mt.NuclearMass(rec->fZ, rec->fA);
  fQ       = mt.Qvalue(projectile, target, ejectile, recoil, kFALSE);
  Set4Mom(energy, frame);
  fPcm     = sqrt( (pow(fS - fM1*fM1 - fM2*fM2, 2) - 4*pow(fM1*fM2, 2) ) / (4*fS) );
  fPprime  = sqrt( (pow(fS - fM3*fM3 - fM4*fM4, 2) - 4*pow(fM3*fM4, 2) ) / (4*fS) );
  fChi     = log( (fPcm + sqrt(fM1*fM1+fPcm*fPcm) ) / fM1 );
  fEcm     = sqrt(fS) - fM1 - fM2;
  fEx      = fEcm + fQ;
  fTb      = ( fS - pow(fM1*fM1+fM2*fM2,2) ) / (2*fM1);
  fTtarget = ( fS - pow(fM1*fM1+fM2*fM2,2) ) / (2*fM2);
  fTbA     = fTb / proj->fA;
  fV2      = fTb / fM1amu;
  if (qb != 0){
    fBrho  = 3.3356*sqrt(pow(fTb+fM1,2) - fM1*fM1) / (1.e3*qb);
  }
}

////////////////////////////////////////////////////////////////////////////////
/// Initialize variables

void dragon::Kin2Body::Init(Int_t Zb, Int_t Ab, Int_t Zt, Int_t At, Int_t Ze,
                            Int_t Ae, Int_t Zr, Int_t Ar, Double_t ecm,
                            Int_t qb)
{
  TAtomicMassTable mt; // AME16
  fM1     = mt.IonMass(Zb, Ab, qb);
  fM2     = mt.IonMass(Zt, At, 0);
  fM3     = mt.NuclearMass(Ze, Ae);
  fM4     = mt.NuclearMass(Zr, Ar);
  fQ      = mt.Qvalue(Zt, At, Zb, Ab, Ze, Ae, kFALSE);
  SetEcm();
  SetEx();
  Set4Mom();
  SetRapidity();
  SetPcm();
  SetPcmPrime();
}

////////////////////////////////////////////////////////////////////////////////
/// Set 4-momentum of system
/// \param energy Energy of beam
/// \param frame string specifying frame / units of beam energy

void Set4Mom(Double_t energy, const char* frame)
{
  if (frame = "CM"){
    fS = pow(fM1*fM1 + fM2*fM2 + energy, 2);
  } else if (frame = "Lab"){
    fS = pow(fM1*fM1 + fM2*fM2,2) + 2*fM2*energy;
  } else if (frame = "Target"){
    fS = pow(fM1*fM1 + fM2*fM2,2) + 2*fM1*energy;
  } else if (frame = "LabA"){
    fS = pow(fM1*fM1 + fM2*fM2,2) + 2*fM2*fA1*energy;
  } else if (frame = "V2"){
    fS = pow(fM1*fM1 + fM2*fM2,2) + 2*fM2*fM1amu*energy;
  } else if (frame = "Excitation"){
    fS = pow(fM1*fM1 + fM2*fM2 + energy-fQ, 2);
  } else if (frame = "Brho"){
    fS = pow(fM1*fM1 + fM2*fM2 + energy, 2);
  } else {
    cout << "__Error!__: Invalid frame string!\n";
  }
}


////////////////////////////////////////////////////////////////////////////////
/// Get maximum cone half angle for ejectile or recoil
/// \param which index of particle

Double_t dragon::Kin2Body::GetMaxAngle(Int_t which)
{
  if (which == 3){
    return asin(fPprime / (fM3*sinh(fChi)));
  }else if (which == 4){
    return asin(fPprime / (fM4*sinh(fChi)));
  } else {
    cout << "__Error!__: Invalid particle index (must be between 1 and 4).\n";
    return 0;
  }
}
