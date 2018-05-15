////////////////////////////////////////////////////////////////////////////////
/// \file Kin2Body.cxx
/// \author D. Connolly
/// \brief Implements Kin2Body.hxx
///
// #include <stdio.h>
#include <vector>
#include <string>
#include <sstream>

#include <TMath.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TSystem.h>
#include <TThread.h>

#include "ErrorDragon.hxx"
#include "RootAnalysis.hxx"
#include "TAtomicMass.h"

#include "Kin2Body.hxx"

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
                           const char* recoil, double energy, const char* frame, Int_t qb)
{
  Init(projectile, target, ejectile, recoil, energy, frame, qb);
}

////////////////////////////////////////////////////////////////////////////////
/// Initialize variables
/// Ctor helper for initializing kinematic quantities

void dragon::Kin2Body::Init(const char* projectile, const char* target, const char* ejectile,
                            const char* recoil, double energy, const char* frame, Int_t qb)
{
  TAtomicMassTable mt; // AME16
  const TAtomicMassTable::Nucleus_t *proj = mt.GetNucleus(projectile);
  const TAtomicMassTable::Nucleus_t *tgt  = mt.GetNucleus(target);
  const TAtomicMassTable::Nucleus_t *ej   = mt.GetNucleus(ejectile);
  const TAtomicMassTable::Nucleus_t *rec  = mt.GetNucleus(recoil);
  fA1      = proj->fA;
  fM1      = mt.IonMass(proj->fZ, proj->fA, qb) / 1.e3;
  fM1amu   = mt.IonMassAMU(proj->fZ, proj->fA, qb);
  fM2      = mt.IonMass(tgt->fZ, tgt->fA, 0) / 1.e3;
  fM3      = mt.NuclearMass(ej->fZ, ej->fA) / 1.e3;
  fM4      = mt.NuclearMass(rec->fZ, rec->fA) / 1.e3;
  fqb      = qb;
  fQrxn    = mt.QValue(projectile, target, ejectile, false);
  Set4Mom(energy, frame);
  fPcm     = sqrt( (pow(fS - fM1*fM1 - fM2*fM2, 2) - 4*pow(fM1*fM2, 2) ) / (4*fS) );
  fPprime  = sqrt( (pow(fS - fM3*fM3 - fM4*fM4, 2) - 4*pow(fM3*fM4, 2) ) / (4*fS) );
  fPb      = sqrt(pow(fTb,2) + 2*fTb*fM1);
  fChi     = log( (fPcm + sqrt(fM1*fM1+fPcm*fPcm) ) / fM1 );
  fEcm     = sqrt(fS) - fM1 - fM2;
  fEx      = fEcm + fQrxn;
  fTb      = ( fS - pow(fM1 + fM2,2) ) / (2*fM2);
  fTtgt    = ( fS - pow(fM1 + fM2,2) ) / (2*fM1);
  fTbA     = fTb / proj->fA;
  fV2b     = fTb / fM1amu;
  if (fqb != 0){
    fBrho  = ( 3.3356 / (1.e3*fqb) )*sqrt( ( (fS - pow(fM1+fM2,2)) / (2*fM2) )*( (fS - pow(fM1+fM2,2)) / (2*fM2) + 2*fM1) );
  } else {
    fBrho  = 0;
  }
}

////////////////////////////////////////////////////////////////////////////////
/// Set 4-momentum of system
/// \param energy Energy of beam
/// \param frame string specifying frame / units of beam energy

void dragon::Kin2Body::Set4Mom(double energy, const char* frame)
{
  if (strncmp(frame, "CM", 2) == 0){
    fS = pow(fM1 + fM2 + energy, 2);
  } else if (strncmp(frame, "Lab", 3) == 0){
    fS  = pow(fM1 + fM2,2) + 2*fM2*energy;
  } else if (strncmp(frame, "Target", 6) == 0){
    fS  = pow(fM1*fM1 + fM2*fM2,2) + 2*fM1*energy;
  } else if (strncmp(frame, "LabA", 4) == 0){
    fS  = pow(fM1*fM1 + fM2*fM2,2) + 2*fM2*fA1*energy;
  } else if (strncmp(frame, "V2", 2) == 0){
    fS  = pow(fM1*fM1 + fM2*fM2,2) + 2*fM2*fM1amu*energy;
  } else if (strncmp(frame, "Excitation", 10) == 0){
    fS  = pow(fM1*fM1 + fM2*fM2 + energy-fQrxn, 2);
  } else if (strncmp(frame, "Brho",4) == 0){
    fTb = (-2*fM1 + sqrt(4*(fM1*fM1 + pow(fqb*energy / 3.3356,2) ) ) ) / 2;
    fS  = pow(fM1 + fM2,2) + 2*fM2*fTb;
  } else {
    dutils::Error("Kin2Body::Set4Mom", __FILE__, __LINE__) << "frame string" << frame << "invalid; must match one of \"CM\", \"Lab\", \"Target\", \"LabA\", \"V2\", \"Excitation\", \"Brho\".\n";
  }
}


////////////////////////////////////////////////////////////////////////////////
/// Get maximum cone half angle for ejectile or recoil
/// \param which index of particle

double dragon::Kin2Body::GetMaxAngle(Int_t which)
{
  if (which == 3){
    return asin(fPprime / (fM3*sinh(fChi)));
  }else if (which == 4){
    return asin(fPprime / (fM4*sinh(fChi)));
  } else {
    dutils::Error("Kin2Body::GetMaxAngle", __FILE__, __LINE__) << "particle index" << which << "invalid; must be 3 or 4.\n";
    return 0;
  }
}
