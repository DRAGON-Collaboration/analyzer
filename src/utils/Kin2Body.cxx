////////////////////////////////////////////////////////////////////////////////
/// \file   Kin2Body.cxx
/// \author D. Connolly
/// \brief  Implements Kin2Body.hxx
////////////////////////////////////////////////////////////////////////////////

// #include <stdio.h>
#include <vector>
#include <string>
#include <sstream>

#include <TMath.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TGraphErrors.h>
#include <TSystem.h>
#include <TThread.h>
#include <TLegend.h>
#include <TCanvas.h>

#include "ErrorDragon.hxx"
#include "RootAnalysis.hxx"
#include "TAtomicMass.h"

#include "Kin2Body.hxx"

namespace dutils = dragon::utils;

//////////////////////// Class dragon::Kin2Body ////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// Default ctor for radiative capture
/// Sets beam and target masses from AME16 compilation.
/// \param projectile string specifying projectile nucleus (e.g. - "7Be", "22Ne", etc.)
/// \param target string specifying projectile nucleus (e.g. - "p", "1H", "4He", "alpha", etc.)
/// \param projectile string specifying ejectile nucleus (e.g. - "p", "d", etc.)
/// \param energy beam energy in units corresponding to specified frame string
/// \param frame string specifying frame / units of beam energy
/// \param qb charge state of beam (default = 0)
dragon::Kin2Body::Kin2Body(const char* projectile, const char* target,
                           Double_t energy, const char* frame, Int_t qb)
{
  Init(projectile, target, energy, frame, qb);
}

////////////////////////////////////////////////////////////////////////////////
/// Overloaded Ctor for particle ejectiles
/// Sets beam and target masses from AME16 compilation.
/// \param projectile string specifying projectile nucleus (e.g. - "7Be", "22Ne", etc.)
/// \param target string specifying projectile nucleus (e.g. - "p", "1H", "4He", "alpha", etc.)
/// \param projectile string specifying ejectile nucleus (e.g. - "p", "d", etc.)
/// \param energy beam energy in units corresponding to specified frame string
/// \param frame string specifying frame / units of beam energy
/// \param qb charge state of beam (default = 0)
dragon::Kin2Body::Kin2Body(const char* projectile, const char* target, const char* ejectile,
                           Double_t energy, const char* frame, Int_t qb)
{
  Init(projectile, target, ejectile, energy, frame, qb);
}

////////////////////////////////////////////////////////////////////////////////
/// Ctor helper for initializing radiative capture kinematics variables
void dragon::Kin2Body::Init(const char* projectile, const char* target,
                            Double_t energy, const char* frame, Int_t qb)
{
  TAtomicMassTable mt; // AME16
  fProj    = mt.GetNucleus(projectile);
  fTgt     = mt.GetNucleus(target);
  fRec     = mt.GetNucleus(fProj->fZ+fTgt->fZ, fProj->fA+fTgt->fA);
  fRxnString  = Form("{}^{%i}%s(^{%i}%s,#gamma)^{%i}%s",fTgt->fA,fTgt->fSymbol,fProj->fA,fProj->fSymbol, fRec->fA, fRec->fSymbol);
  fProjString = Form("{}^{%i}%s",fProj->fA,fProj->fSymbol);
  fTgtString  = Form("{}^{%i}%s",fTgt->fA,fTgt->fSymbol);
  fEjString   = "#gamma";
  fRecString  = Form("{}^{%i}%s",fRec->fA,fRec->fSymbol);
  fM1      = mt.IonMass(fProj->fZ, fProj->fA, 0) / 1.e3;
  fM1amu   = mt.IonMassAMU(fProj->fZ, fProj->fA, 0);
  fM2      = mt.IonMass(fTgt->fZ, fTgt->fA, 0) / 1.e3;
  fM3      = 0;
  fM4      = mt.IonMass(fRec->fZ, fRec->fA, 0) / 1.e3;
  fqb      = qb;
  fQrxn    = mt.QValue(projectile, target, "g", false) / 1.e3;
  Set4Mom(energy, frame);
  fPcm     = sqrt( (pow(fS - fM1*fM1 - fM2*fM2, 2) - 4*pow(fM1*fM2, 2) ) / (4*fS) );
  fPprime  = sqrt( (pow(fS - fM4*fM4, 2) ) / (4*fS) );
  fChi     = log( (fPcm + sqrt(fM2*fM2 + fPcm*fPcm) ) / fM2 );
  fEcm     = sqrt(fS) - fM1 - fM2;
  fEx      = fEcm + fQrxn;
  fTb      = ( fS - pow(fM1 + fM2,2) ) / (2*fM2);
  fPb      = sqrt(pow(fTb,2) + 2*fTb*fM1);
  fTtgt    = ( fS - pow(fM1 + fM2,2) ) / (2*fM1);
  fTbA     = fTb / fProj->fA;
  fV2b     = fTb / fM1amu;
  if (fqb != 0){
    fBrho  = ( 3.3356 / (1.e3*fqb) )*sqrt( ( (fS - pow(fM1+fM2,2)) / (2*fM2) )*( (fS - pow(fM1+fM2,2)) / (2*fM2) + 2*fM1) );
  } else {
    fBrho  = 0;
  }
  fEb           = fTb + fM1;
  fGamma_b      = fEb / fM1;
  fBeta_b       = sqrt(1 - 1/pow(fGamma_b,2));
  fMcpTofb      = dragon::Constants::Lmcp() / (fBeta_b*TMath::C())*1.0e9;
  fSepTofb      = dragon::Constants::Ldra() / (fBeta_b*TMath::C())*1.0e6;
  fE90rec       = sqrt(fPb*fPb + fM4*fM4);
  fGamma90rec   = fE90rec / fM4;
  fBeta90rec    = sqrt(1 - 1/pow(fGamma90rec,2));
  fMcpTofrec90  = dragon::Constants::Lmcp() / (fBeta90rec*TMath::C())*1.0e9;
  fSepTofrec90  = dragon::Constants::Ldra() / (fBeta90rec*TMath::C())*1.0e6;
  fE0rec        = sqrt( pow(fPb*(1 - fEx / sqrt(fEb*fEb - fM1*fM1)),2) + fM4*fM4 );
  fGamma0rec    = fE0rec / fM4;
  fBeta0rec     = sqrt(1 - 1/pow(fGamma0rec,2));
  fMcpTofrec0   = dragon::Constants::Lmcp() / (fBeta0rec*TMath::C())*1.0e9;
  fSepTofrec0   = dragon::Constants::Ldra() / (fBeta0rec*TMath::C())*1.0e6;
  fE180rec      = sqrt( pow(fPb*(1 + fEx / sqrt(fEb*fEb - fM1*fM1)),2) + fM4*fM4 );
  fGamma180rec  = fE180rec / fM4;
  fBeta180rec   = sqrt(1 - 1/pow(fGamma180rec,2));
  fMcpTofrec180 = dragon::Constants::Lmcp() / (fBeta180rec*TMath::C())*1.0e9;
  fSepTofrec180 = dragon::Constants::Ldra() / (fBeta180rec*TMath::C())*1.0e6;
}

////////////////////////////////////////////////////////////////////////////////
/// Ctor helper for initializing kinematics variables (particle ejectiles)
void dragon::Kin2Body::Init(const char* projectile, const char* target, const char* ejectile,
                            Double_t energy, const char* frame, Int_t qb)
{
  TAtomicMassTable mt; // AME16
  fProj = mt.GetNucleus(projectile);
  fTgt  = mt.GetNucleus(target);
  fEj   = mt.GetNucleus(ejectile);
  fRec  = mt.GetNucleus(fProj->fZ+fTgt->fZ-fEj->fZ, fProj->fA+fTgt->fA-fEj->fA);
  if ( strncmp(projectile, ejectile, 5) == 0 ){ // elastic scattering
    std::cout << "Elastic scattering!\n";
    fM1      = mt.IonMass(fProj->fZ, fProj->fA, 0) / 1.e3;
    fM1amu   = mt.IonMassAMU(fProj->fZ, fProj->fA, 0);
    fM2      = mt.IonMass(fTgt->fZ, fTgt->fA, 0) / 1.e3;
    fM3      = fM1;
    fM4      = fM2;
  } else {
    fM1      = mt.IonMass(fProj->fZ, fProj->fA, 0) / 1.e3;
    fM1amu   = mt.IonMassAMU(fProj->fZ, fProj->fA, 0);
    fM2      = mt.IonMass(fTgt->fZ, fTgt->fA, 0) / 1.e3;
    fM3      = mt.IonMass(fEj->fZ, fEj->fA, 0) / 1.e3;
    fM4      = mt.IonMass(fRec->fZ, fRec->fA, 0) / 1.e3;
  }
  fRxnString = Form("{}^{%i}%s(^{%i}%s,{}^{%i}%s)^{%i}%s",fTgt->fA,fTgt->fSymbol,fProj->fA,fProj->fSymbol,fEj->fA,fEj->fSymbol, fRec->fA, fRec->fSymbol);
  fProjString = Form("{}^{%i}%s",fProj->fA,fProj->fSymbol);
  fTgtString = Form("{}^{%i}%s",fTgt->fA,fTgt->fSymbol);
  fEjString = Form("{}^{%i}%s",fEj->fA,fEj->fSymbol);
  fRecString = Form("{}^{%i}%s",fRec->fA,fRec->fSymbol);
  fqb      = qb;
  fQrxn    = mt.QValue(projectile, target, ejectile, false) / 1.e3;
  Set4Mom(energy, frame);
  fPcm     = sqrt( (pow(fS - fM1*fM1 - fM2*fM2, 2) - 4*pow(fM1*fM2, 2) ) / (4*fS) );
  fPprime  = sqrt( (pow(fS - fM3*fM3 - fM4*fM4, 2) - 4*pow(fM3*fM4, 2) ) / (4*fS) );
  fPb      = sqrt(pow(fTb,2) + 2*fTb*fM1);
  fChi     = log( (fPcm + sqrt(fM2*fM2 + fPcm*fPcm) ) / fM2 );
  fEcm     = sqrt(fS) - fM1 - fM2;
  fEx      = fEcm + fQrxn;
  fTb      = ( fS - pow(fM1 + fM2,2) ) / (2*fM2);
  fTtgt    = ( fS - pow(fM1 + fM2,2) ) / (2*fM1);
  fTbA     = fTb / fProj->fA;
  fV2b     = fTb / fM1amu;
  if (fqb != 0){
    fBrho  = ( 3.3356 / (1.e3*fqb) )*sqrt( ( (fS - pow(fM1+fM2,2)) / (2*fM2) )*( (fS - pow(fM1+fM2,2)) / (2*fM2) + 2*fM1) );
  } else {
    fBrho  = 0;
  }
}

////////////////////////////////////////////////////////////////////////////////
/// Calculate the energy of the ejectile as a function of angle
/// \param theta angle of ejectile / recoil in degrees
/// \param which ejectile or recoil
Double_t dragon::Kin2Body::CalcTLabTheta(Double_t theta, const char* which, Bool_t negative)
{
  Double_t mass;
  if (strncmp(which, "recoil", 6) == 0){
    mass = fM4;
  } else {
    mass = fM3;
  }

  if (theta == 90.0){
    if (mass*sinh(fChi) / fPprime > 1){
      return 0.0;
    } else {
      Double_t p = fPprime*sqrt(1 - pow(mass*sinh(fChi) / fPprime,2) / (1 + pow(sinh(fChi),2)) );
      Double_t T = sqrt(p*p + mass*mass) - mass;
      if (T < 0.001) {
        return 0;
      } else {
        return T;
      }
    }
  }

  if (theta >= GetMaxAngle(which)) {
    theta = GetMaxAngle(which);
    theta /= 180.;
    theta *= TMath::Pi();
    Double_t pe = cos(theta)*sinh(fChi)*sqrt(mass*mass + fPprime*fPprime);
    pe /= ( 1+pow(sin(theta)*sinh(fChi),2) );
    return sqrt(pe*pe + mass*mass) - mass;
  } else if (!negative){
    theta /= 180.;
    theta *= TMath::Pi();
    Double_t pe = cos(theta)*sinh(fChi)*sqrt(mass*mass + fPprime*fPprime);
    pe += cosh(fChi)*sqrt( fPprime*fPprime - pow(mass*sin(theta)*sinh(fChi),2) );
    pe /= ( 1+pow(sin(theta)*sinh(fChi),2) );
    return sqrt(pe*pe + mass*mass) - mass;
  } else {
    theta /= 180.;
    theta *= TMath::Pi();
    Double_t pe = cos(theta)*sinh(fChi)*sqrt(mass*mass + fPprime*fPprime);
    pe -= cosh(fChi)*sqrt( fPprime*fPprime - pow(mass*sin(theta)*sinh(fChi),2) );
    pe /= ( 1+pow(sin(theta)*sinh(fChi),2) );
    return sqrt(pe*pe + mass*mass) - mass;
  }
}

////////////////////////////////////////////////////////////////////////////////
/// Set 4-momentum of system
/// \param energy Energy of beam
/// \param frame string specifying frame / units of beam energy
void dragon::Kin2Body::Set4Mom(Double_t energy, const char* frame)
{
  if (strncmp(frame, "CM", 2) == 0){
    fS = pow(fM1 + fM2 + energy, 2);
  } else if (strncmp(frame, "Lab", 3) == 0){
    fS  = pow(fM1 + fM2,2) + 2*fM2*energy;
  } else if (strncmp(frame, "Target", 6) == 0){
    fS  = pow(fM1 + fM2,2) + 2*fM1*energy;
  } else if (strncmp(frame, "ALab", 4) == 0){
    fS  = pow(fM1 + fM2,2) + 2*fM2*fProj->fA*energy;
  } else if (strncmp(frame, "V2", 2) == 0){
    fS  = pow(fM1 + fM2,2) + 2*fM2*fM1amu*energy;
  } else if (strncmp(frame, "Excitation", 10) == 0){
    fS  = pow(fM1 + fM2 + energy-fQrxn, 2);
  } else if (strncmp(frame, "Brho",4) == 0){
    fTb = (-2*fM1 + sqrt(4*(fM1*fM1 + pow(fqb*energy / 3.3356,2) ) ) ) / 2;
    fS  = pow(fM1 + fM2,2) + 2*fM2*fTb;
  } else {
    dutils::Error("Kin2Body::Set4Mom", __FILE__, __LINE__) << "frame string \"" << frame << "\" invalid; must match one of \"CM\", \"Lab\", \"Target\", \"LabA\", \"V2\", \"Excitation\", \"Brho\".\n";
  }
}

////////////////////////////////////////////////////////////////////////////////
/// Get maximum cone half angle for ejectile or recoil
/// \param which index of particle
Double_t dragon::Kin2Body::GetMaxAngle(const char* which)
{
  if (strncmp(which, "ejectile", 8) == 0){
    if ( fabs( fPprime / (fM3*sinh(fChi)) ) >= 1){
      if ( CalcTLabTheta(90, which) > 0){
        return 180.;
      } else {
        return 90.;
      }
    } else {
      return 180*asin(fPprime / (fM3*sinh(fChi))) / TMath::Pi();
    }
  } else if (strncmp(which, "recoil", 6) == 0){
    if ( fabs( fPprime / (fM4*sinh(fChi)) ) >= 1){
      if ( CalcTLabTheta(90, which) > 0){
        return 180.;
      } else {
        return 90.;
      }
    } else {
      return 180*asin(fPprime / (fM4*sinh(fChi))) / TMath::Pi();
    }
  } else if (strncmp(which, "residue", 7) == 0) {
    if ( fabs( fPprime / (fM4*sinh(fChi)) ) >= 1){
      if ( CalcTLabTheta(90, "recoil") > 0){
        return 180.;
      } else {
        return 90.;
      }
    } else {
      return 180*asin(fPprime / (fM4*sinh(fChi))) / TMath::Pi();
    }
  } else {
    dutils::Error("Kin2Body::GetMaxAngle", __FILE__, __LINE__) << "particle string " << which << " invalid; must one of \"ejectile\", \"recoil\", or \"residue.\"\n";
    return 0;
  }
}

TMultiGraph* dragon::Kin2Body::PlotTLabvsThetaLab(Option_t *option_e, Option_t *option_r)
{
  Double_t maxtheta;
  Double_t max_e   = GetMaxAngle("ejectile");
  Double_t max_rec = GetMaxAngle("recoil");
  Int_t npoints = 90;

  if (max_e > 90 || max_rec > 90){
    maxtheta = 180;
  } else {
    maxtheta = 90;
  }

  std::vector<double> theta_e, theta_en, theta_r, theta_rn, Te, Te_n, Trec, Trec_n;

  Double_t x = 0;
  Double_t dx = max_rec / npoints;

  for (Int_t i = 0; i <= npoints; i++){
    theta_r.push_back(x);
    Trec.push_back(CalcTLabTheta(x, "recoil"));
    x += dx;
  }

  if(max_rec < 90) {
    x = 0;
    for (Int_t i = 0; i <= npoints; i++){
      theta_rn.push_back(x);
      Trec_n.push_back(CalcTLabTheta(x, "recoil", kTRUE));
      x += dx;
    }
  }

  x = 0;
  dx = max_e / npoints;
  for (Int_t i = 0; i <= npoints; i++){
    theta_e.push_back(x);
    Te.push_back(CalcTLabTheta(x, "ejectile"));
    x += dx;
  }

  if(max_e < 90) {
    x = 0;
    for (Int_t i = 0; i <= npoints; i++){
      theta_en.push_back(x);
      Te_n.push_back(CalcTLabTheta(x, "ejectile", kTRUE));
      x += dx;
    }
  }

  // TMultiGraph* mg = 0;
  if(theta_e.size()){
    TGraph *ge  = new TGraph(theta_e.size(), &theta_e[0], &Te[0]);
    TGraph *grec = new TGraph(theta_r.size(), &theta_r[0], &Trec[0]);
    ge->SetLineColor(4);
    ge->SetMarkerColor(4);
    ge->SetMarkerStyle(27);
    grec->SetLineColor(2);
    grec->SetMarkerColor(2);
    grec->SetMarkerStyle(26);

    TMultiGraph *mg = new TMultiGraph();
    mg->Add(grec, option_r); mg->Add(ge, option_e);

    if (theta_en.size()){
      TGraph* gen  = new TGraph(theta_en.size(), &theta_en[0], &Te_n[0]);
      gen->SetLineColor(4);
      gen->SetMarkerColor(4);
      gen->SetMarkerStyle(27);
      mg->Add(gen, option_e);
    }

    if (theta_en.size()){
      TGraph* grec_n  = new TGraph(theta_rn.size(), &theta_rn[0], &Trec_n[0]);
      grec_n->SetLineColor(2);
      grec_n->SetMarkerColor(2);
      grec_n->SetMarkerStyle(26);
      mg->Add(grec_n, option_r);
    }

    TLegend* leg = new TLegend(0.6,0.4,0.88,0.6);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->AddEntry(ge, fEjString, "L");
    leg->AddEntry(grec, fRecString, "L");

    TCanvas *c0 = new TCanvas();
    mg->SetTitle(Form("%s #it{T}_{b} = %0.3f; #it{#theta}_{lab}; Lab Frame Kinetic Energy [MeV]", fRxnString, fTb));
    mg->Draw("al");
    leg->Draw("same");
    mg->GetXaxis()->CenterTitle(); mg->GetYaxis()->CenterTitle();
    mg->GetXaxis()->SetRangeUser(0, maxtheta);
    c0->Modified(); c0->Update();

    return mg;

  } else {
    return 0;
  }
}
