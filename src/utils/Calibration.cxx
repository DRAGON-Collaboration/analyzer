////////////////////////////////////////////////////////////////////////////////
/// \file Calibration.cxx
/// \author G. Christian and D. Connolly
/// \brief Implements Calibration.hxx
////////////////////////////////////////////////////////////////////////////////

#include <cstdio>
#include <cstring>
#include <sstream>
#include <fstream>
#include <TPolyMarker.h>
#include <TDirectory.h>
#include <TFitResult.h>
#include <TSpectrum.h>
#include <TSystem.h>
#include <TString.h>
#include <TGraph.h>
#include <TList.h>
#include <TMath.h>
#include <TTree.h>
#include <TF1.h>
#include <TH1F.h>
#include <TH2F.h>
#include "midas/Database.hxx"
#include "midas/Odb.hxx"
#include "Calibration.hxx"

static HNDLE hDB;

namespace { const Int_t NDSSSD = dragon::Dsssd::MAX_CHANNELS; }

namespace dutils = dragon::utils;

///////////////////// Class dragon::utils::DsssdCalibrator /////////////////////

////////////////////////////////////////////////////////////////////////////////
/// Construct from tree with heavy ion singles data & database containing
/// variables
/// \param t Pointer to a `TTree` containing heavy-ion singles data
/// \param db Pointer to a database containing the variables with which the
///        `dsssd.ecal[]` data in _t_ were calculated.
dutils::DsssdCalibrator::DsssdCalibrator(TTree* t, midas::Database* db):
  fTree(t), fDb(db)
{
  if(!fDb) return;
  Double_t slopes[NDSSSD], offsets[NDSSSD];
  db->ReadArray("/dragon/dsssd/variables/adc/slope",  slopes, NDSSSD);
  db->ReadArray("/dragon/dsssd/variables/adc/offset", offsets, NDSSSD);
  for(Int_t i=0; i< NDSSSD; ++i) {
    fOldParams[i].slope = slopes[i];
    fOldParams[i].offset = offsets[i];
  }
}

////////////////////////////////////////////////////////////////////////////////
/// Draw a summary of DSSSD energies (w/ no calibration)
/// \param Opt Drawing option for the displayed histogram
void dutils::DsssdCalibrator::DrawSummary(Option_t* opt) const
{
  if(!fTree) return;
  if(gDirectory->Get("hdsssd"))
    gDirectory->Get("hdsssd")->Delete();

  TH2F* hdsssd = new TH2F("hdsssd", "", NDSSSD, 0, NDSSSD, 4096, 0, 4095);
  dragon::Tail* tail = new dragon::Tail();
  fTree->SetBranchAddress("tail", &tail);
  for(Long_t evt = 0; evt < fTree->GetEntries(); ++evt) {
    fTree->GetEntry(evt);
    for(Int_t i=0; i< NDSSSD; ++i){
      Double_t val = (tail->dsssd.ecal[i] - fOldParams[i].offset) / fOldParams[i].slope;
      hdsssd->Fill(i, val);
    }
  }
  hdsssd->Draw(opt);
}

////////////////////////////////////////////////////////////////////////////////
/// Draw a summary of DSSSD energies (w/ calibration)
/// \param Opt Drawing option for the displayed histogram
void dutils::DsssdCalibrator::DrawSummaryCal(Option_t* opt)
{
  if(!fTree) return;
  if(gDirectory->Get("fHdcal"))
    gDirectory->Get("fHdcal")->Delete();
  fHdcal = new TH2D("fHdcal", "", NDSSSD, 0, NDSSSD, 4096, 0, 4095);
  dragon::Tail* tail = new dragon::Tail();
  fTree->SetBranchAddress("tail", &tail);
  for(Long_t evt = 0; evt < fTree->GetEntries(); ++evt) {
    fTree->GetEntry(evt);
    for(Int_t i = 0; i < NDSSSD; ++i) {
      Double_t val = tail->dsssd.ecal[i]*fParams[i].slope + fParams[i].offset;
      fHdcal->Fill(i, val);
    }
  }
  fHdcal->Draw(opt);
}

////////////////////////////////////////////////////////////////////////////////
/// Draw calibrated DSSSD energy spectrum of the front strips
/// \param Opt Drawing option for the displayed histogram
void dutils::DsssdCalibrator::DrawFrontCal(Option_t* opt)
{
  if(!fTree) return;
  if(!(gDirectory->Get("fHdcal")))
    DrawSummaryCal("goff");
  fFrontcal = (TH1D *)fHdcal->ProjectionY("frontcal",0,15,opt);
  fFrontcal->Draw(opt);
}

////////////////////////////////////////////////////////////////////////////////
/// Get the slope and offset for a given channel
dutils::DsssdCalibrator::Param_t dutils::DsssdCalibrator::GetParams(Int_t channel) const
{
  if((UInt_t)channel > NDSSSD - 1) {
    std::cerr << "Invalid channel " << channel << ", valid range is [0, 31].\n";
    Param_t junk = {0, 0, 0};
    return junk;
  }
  return fParams[channel];
}

////////////////////////////////////////////////////////////////////////////////
/// Get the slope and offset for a given channel
dutils::DsssdCalibrator::Param_t dutils::DsssdCalibrator::GetOldParams(Int_t channel) const
{
  if((UInt_t)channel > NDSSSD - 1) {
    std::cerr << "Invalid channel " << channel << ", valid range is [0, 31].\n";
    Param_t junk = {0, 0, 0};
    return junk;
  }
  return fOldParams[channel];
}

////////////////////////////////////////////////////////////////////////////////
/// Get the value of a particular peak as found by FindPeaks()
Double_t dutils::DsssdCalibrator::GetPeak(Int_t channel, Int_t peak) const
{
  if((UInt_t)channel > NDSSSD - 1) {
    std::cerr << "Invalid channel " << channel << ", valid range is [0, 31].\n";
    return 0;
  }
  if((UInt_t)peak > 3) {
    std::cerr << "Invalid peak " << peak << ", valid range is [0, 3].\n";
    return 0;
  }
  return fPeaks[channel][peak];
}

////////////////////////////////////////////////////////////////////////////////
/// Helper routine to find triple alpha peaks in a spectrum
std::vector<Double_t> dutils::DsssdCalibrator::FindPeaks(TH1* hst,
                                                         Double_t sigma,
                                                         Double_t threshold) const
{
  std::vector<Double_t> peaks;
  TSpectrum spectrum;
  spectrum.Search(hst, sigma, "goff", threshold);
  Int_t npeaks = spectrum.GetNPeaks();
  std::vector<int> indx(npeaks);
  TMath::Sort(npeaks, spectrum.GetPositionX(), &indx[0], kFALSE);
  for(Int_t i = 0; i < npeaks; ++i) {
    peaks.push_back(*(spectrum.GetPositionX() + indx[i]));
  }
  return peaks;
}

////////////////////////////////////////////////////////////////////////////////
/// Helper routine to fit alpha energy vs. ADC channel
/// \param pklow     Low edge (in uncalibrated channel number) of the region
///        where the peak finding algorithm should search for triple alpha peaks
/// \param pkhigh    High edge (in uncalibrated channel number) of the region
///        where the peak finding algorithm should search for triple alpha peaks
/// \param Sigma     Minimum width of peaks to be found by the searching algorithm
/// \param threshold Minimum peak height for the peak searching algorithm
/// \note See `TSpectrum::Search` in ROOT's class documentation for more info on
///       the _sigma_ and _threshold_ parameters
Int_t dutils::DsssdCalibrator::Run(Int_t nbins, Double_t pklow, Double_t pkhigh,
                                   Double_t sigma, Double_t threshold, Bool_t grid)
{
  if(!fTree) return 0;
  TH1F hpeaks("hpeaks", "", nbins, pklow, pkhigh);
  Int_t retval = 0;
  for(Int_t i = 0; i < NDSSSD; ++i) {
    char buf[16];
    // Double_t slope = GetOldParams(i).slope, offset = GetOldParams(i).offset;
    // sprintf(buf, "(dsssd.ecal[%d] - %f) / %f", i, offset, slope);
    sprintf(buf, "dsssd.ecal[%d]", i);
    fTree->Project("hpeaks", buf, "", "goff");
    std::vector<Double_t> peaks = FindPeaks(&hpeaks, sigma, threshold);
    if(peaks.size() != 3) {
      std::cerr << "Number of peaks found for channel " << i << ": " << peaks.size()
                << " != 3, skipping!\n";
      fParams[i].slope  = -1.99999;
      fParams[i].offset = 0.0;
      continue;
    } else {
      ++retval;
    }
    for(int j = 0; j < 3; ++j)
      fPeaks[i][j] = peaks[j];

    FitPeaks(i, grid);
  }
  GainMatch();
  return retval;
}

////////////////////////////////////////////////////////////////////////////////
/// Gainmatch the dsssd channels and find energy per bin
void dutils::DsssdCalibrator::GainMatch()
{
  fMaxSlope = 0;
  for (Int_t i = 0; i < NDSSSD; ++i){
    if(fParams[i].slope > fMaxSlope){
      fMaxSlope = fParams[i].slope;
      fMinChan = i;
      // cout << fMinChan << endl;
      // cout << fMaxSlope << endl;
    }
  }
  for(Int_t i = 0; i < NDSSSD; i++){
    if ( i == fMinChan){
      fParams[i].slope = 1.0;
    }
    else if ( fParams[i].slope < 0 ) {
      continue;
    }
    else{
      fParams[i].slope /= fMaxSlope;
    }
    fParams[i].offset /= fMaxSlope;
  }
}

////////////////////////////////////////////////////////////////////////////////
/// Run the full calibration
/// \param ch DSSSD channel
/// \param grid boolean specifying whether detector is Tengblad or standard
///        design
void dutils::DsssdCalibrator::FitPeaks(Int_t ch, Bool_t grid)
{
  if((UInt_t)ch > NDSSSD - 1) return;
  const Double_t alphaEnergies[3] = {5.15659,5.48556,5.80477}; /* primary alpha energies of triple alpha source in MeV. */
  const Double_t dLayer           = 374.0e-5; /* dead layer thickness (Al equivalent) in mm (from C. Wrede's thesis). */
  const Double_t dLayer_grid      = 5.0e-5; /* dead layer thickness in mm (Si) of Tengblad design (girdded) DSSSD */
  const Double_t dEdx_Al[3]       = {160.1167,154.1106,148.9053}; // stopping powers (MeV/mm) of alphas in Al corresponding to above energies according to SRIM 2008
  const Double_t dEdx_Si[3]       = {139.006,133.701,129.296}; // stopping powers (MeV/mm) of alphas in Si corresponding to above energies according to SRIM 2008
  // const Double_t aEloss[3] = {138.080,133.060,128.546}; /* Stopping Powers in MeV/mm. */
  Double_t aEnergy[3]; /* Actual energy deposited */
  if (grid) {
    for (int i = 0; i < 3; i++)
      aEnergy[i] = alphaEnergies[i] - dEdx_Si[i]*dLayer_grid;
  }
  else {
    for (int i = 0; i < 3; i++)
      aEnergy[i] = alphaEnergies[i] - dEdx_Al[i]*dLayer;
  }
  TGraph gr(3);
  for(Int_t i = 0; i < gr.GetN(); ++i) {
    gr.SetPoint(i, GetPeak(ch, i), aEnergy[i]);
  }
  TFitResultPtr result = gr.Fit("pol1", "qns");
  fParams[ch].slope  = result->Value(1);
  fParams[ch].offset = result->Value(0);
}

////////////////////////////////////////////////////////////////////////////////
/// Print a summary of the calibration results
/// \param outfile String specifying a file path for output, default (0) prints
///        to stdout.
void dutils::DsssdCalibrator::PrintResults(const char* outfile)
{
  FILE* f = stdout;
  if(outfile) {
    TString fname = outfile;
    gSystem->ExpandPathName(fname);
    f = fopen(fname, "w");
    if(!f) {
      std::cerr << "Invalid file path " << outfile << "\n";
      f = stdout;
    }
  }
  fprintf(f, "\n%-64s", "================================================================\n");
  fprintf(f, "Calibration constant: \t c(1) = %-6f MeV / bin", fMaxSlope);
  fprintf(f, "\n%-64s", "================================================================\n");
  fprintf(f, "\n\n%-7s \t %-8s \t %-7s \t %-7s\n","Channel","Offset","Gain","INL");
  fprintf(f, "%-7s \t %-8s \t %-7s \t %-7s\n","=======","======","======","======");
  // fprintf(f, "Channel\tSlope\tOffset\n");
  for(Int_t i = 0; i < NDSSSD; ++i) {
    fprintf(f, "%7i \t %-6g \t %-6g \t %-6g\n", i, GetParams(i).offset, GetParams(i).slope, 0.0);
    // fprintf(f, "%7i \t %-6g \t %-6g \t %-6g\n", i, GetParams(i).offset, GetParams(i).slope, GetParams(i).inl);
    // fprintf(f, "%2d\t%.6g\t%.6g\n", i, GetParams(i).slope, GetParams(i).offset);
  }
  if(f != stdout) fclose(f);
}

////////////////////////////////////////////////////////////////////////////////
/// Print the calibration results (file or stdout) in a format that can be input
/// into odbedit to update the calibration -- deprecated
/// \param outfile String specifying a file path for output, default (0) prints
///        to stdout.
void dutils::DsssdCalibrator::PrintOdb(const char* outfile)
{
  FILE* f = stdout;
  if(outfile) {
    TString fname = outfile;
    gSystem->ExpandPathName(fname);
    f = fopen(fname, "w");
    if(!f) {
      std::cerr << "Invalid file path " << outfile << "\n";
      f = stdout;
    }
  }
  for(Int_t i = 0; i < NDSSSD; ++i) {
    fprintf(f, "odbedit -c \"set /dragon/dsssd/variables/adc/slope[%d] %.6g\"\n", i, GetParams(i).slope);
    fprintf(f, "odbedit -c \"set /dragon/dsssd/variables/adc/offset[%d] %.6g\"\n", i, GetParams(i).offset);
  }
  if(f != stdout) fclose(f);
}

////////////////////////////////////////////////////////////////////////////////
/// Write calibration parameters to json file
/// \param outfile String specifying a file path for output;
///        default `$DH/../calibration/dsssdcal.json`
void dutils::DsssdCalibrator::WriteJson(const char* outfile)
{
  std::ofstream ofs(gSystem->ExpandPathName(outfile));
  ofs << "{\n";
  ofs << "  \"/MIDAS version\" : \"2.1\",\n";
  ofs << "  \"/MIDAS git revision\" : \"Fri Oct 13 11:11:03 2017 -0700 - 3dd7f52\",\n";
  ofs << "  \"/filename\" : \"" << outfile << "\",\n";
  ofs << "  \"/ODB path\" : \"/dragon/dsssd/variables/adc\",\n\n";
  ofs << "  \"channel/key\" : { \"type\" : 7, \"num_values\" : 32, \"access_mode\" : 7, \"last_written\" : 1507880655 },\n";
  ofs << "  \"channel\" : [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 ],\n";
  ofs << "  \"module/key\" : { \"type\" : 7, \"num_values\" : 32, \"access_mode\" : 7, \"last_written\" : 1507880655 },\n";
  ofs << "  \"module\" : [ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ],\n";
  ofs << "  \"slope/key\" : { \"type\" : 10, \"num_values\" : 32, \"access_mode\" : 7, \"last_written\" : 1508890161 },\n";
  ofs << "  \"slope\" : [ ";
  for(Int_t i = 0; i < NDSSSD; i++){
    ofs << GetParams(i).slope << ", ";
  }
  ofs << "],\n";
  ofs << "  \"offset/key\" : { \"type\" : 10, \"num_values\" : 32, \"access_mode\" : 7, \"last_written\" : 1508890161 },\n";
  ofs << "  \"offset\" : [ ";
  for(Int_t i = 0; i < NDSSSD; i++){
    ofs << GetParams(i).offset << ", ";
  }
  ofs << "]\n";
  ofs << "}\n";
  ofs.close();
  std::cout << "ATTENTION: Current odb state saved as " << outfile << "\n";
}

#ifdef MIDASSYS // Enable WriteOdb method which contains calls to MIDAS methods
////////////////////////////////////////////////////////////////////////////////
/// Write calibration parameters to MIDAS ODB
/// \param json flag to save ODB as a `.json` file
/// \param json flag to save ODB as a `.xml` file
void dutils::DsssdCalibrator::WriteOdb(Bool_t json, Bool_t xml)
{
  INT status;

  TString slopeKey = "/dragon/dsssd/variables/adc/slope";
  TString offKey   = "/dragon/dsssd/variables/adc/offset";
  TString calKey   = "/dragon/dsssd/variables/adc/calibration constant";

  Double_t slopes[NDSSSD], offsets[NDSSSD];

  for(INT i = 0; i < NDSSSD; i++){
    slopes[i]  = GetParams(i).slope;
    offsets[i] = GetParams(i).offset;
  }

  // connect to midas experiment
  status = cm_connect_experiment ("", "dragon", "Calibration.cxx", 0);
  if (status != CM_SUCCESS) {
    printf("Error connecting to experiment, status = %d.\n", status);
  }

  status = cm_get_experiment_database (&hDB, 0);
  if(status != CM_SUCCESS) {
    cm_msg(MERROR, "Calibration.cxx", "Couldn't get database handle, status = %d\n", status);
    cm_disconnect_experiment();
  }

  // Write offsets to odb
  status = db_set_value(hDB, 0, offKey.Data(), &offsets[0], sizeof(offsets),
                        sizeof(offsets) / sizeof(double), TID_DOUBLE);
  if(status != CM_SUCCESS){
    cm_msg(MERROR, "Calibration.cxx", "Couldn't write %s, status = %d\n", offKey.Data(), status);
    cm_disconnect_experiment();
  }
  // Write slopes to odb
  status = db_set_value(hDB, 0, slopeKey.Data(), &slopes[0], sizeof(slopes),
                        sizeof(slopes) / sizeof(double), TID_DOUBLE);
  if(status != CM_SUCCESS){
    cm_msg(MERROR, "Calibration.cxx", "Couldn't write %s, status = %d\n", slopeKey.Data(), status);
    cm_disconnect_experiment();
  }

  /* disconnect from experiment */
  cm_disconnect_experiment();

  std::cout << "ATTENTION: gains and offsets written to odb!\n";
  // Check if $DH is set
  if(!(gSystem->Getenv("DH"))) gSystem->Setenv("DH","PWD");
  // Save current odb state to xml file
  if(xml){
    gSystem->Exec("odbedit -d /dragon/dsssd/variables/adc -c 'save -x dsssdCal_full.xml'"); // save calibration as xml file in pwd
    gSystem->Exec("if [ ! -d $DH/../calibration ]; then mkdir -p $DH/../calibration; fi");
    gSystem->Exec("mv -f ./dsssdcal_full.xml ${DH}/../calibration/"); // move xml file to $DH/../calibration/
    std::cout << "ATTENTION: Current odb state saved to dsssdcal.xml in ${DH}/../calibration/ !\n";
  }
  if(json){
    gSystem->Exec("odbedit -d /dragon/dsssd/variables/adc -c 'save -j dsssdCal_full.json'"); // save calibration as json file in pwd
    gSystem->Exec("if [ ! -d $DH/../calibration ]; then mkdir -p $DH/../calibration; fi");
    gSystem->Exec("mv -f ./dsssdcal_full.json ${DH}/../calibration/"); // move json file to $DH/../calibration/
    std::cout << "ATTENTION: Current odb state saved to dsssdcal.json in ${DH}/../calibration/ !\n";
  }
}
#endif

////////////////////////////////////////////////////////////////////////////////
/// Write calibration parameters to xml file
/// \param outfile String specifying a file path for output;
///        default `$DH/../calibration/dsssdcal.xml`
void dutils::DsssdCalibrator::WriteXml(const char* outfile)
{
  std::ofstream ofs(gSystem->ExpandPathName(outfile));
  ofs << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
  ofs << "<odb root=\"/\" filename=\"" <<
    outfile << "\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"/Users/dragon/packages/midas/odb.xsd\">\n";
  ofs << "  <dir name=\"dragon\">\n";
  ofs << "    <dir name=\"dsssd\">\n";
  ofs << "      <dir name=\"variables\">\n";
  ofs << "        <dir name=\"adc\">\n";
  ofs << "          <keyarray name=\"channel\" type=\"INT\" num_values=\"32\">\n";
  for(int i = 0; i < NDSSSD; ++i) {
    ofs << "            <value index=\"" << i << "\">" << i << "</value>\n";
  }
  ofs << "          </keyarray>\n";
  ofs << "          <keyarray name=\"module\" type=\"INT\" num_values=\"32\">\n";
  for(int i = 0; i< NDSSSD; ++i) {
    ofs << "            <value index=\"" << i << "\">1</value>\n";
  }
  ofs << "          </keyarray>\n";
  ofs << "          <keyarray name=\"slope\" type=\"DOUBLE\" num_values=\"32\">\n";
  for(int i = 0; i< NDSSSD; ++i) {
    ofs << "            <value index=\"" << i << "\">" << GetParams(i).slope << "</value>\n";
  }
  ofs << "          </keyarray>\n";
  ofs << "          <keyarray name=\"offset\" type=\"DOUBLE\" num_values=\"32\">\n";
  for(int i = 0; i< NDSSSD; ++i) {
    ofs << "            <value index=\"" << i << "\">" << GetParams(i).offset << "</value>\n";
  }
  ofs << "          </keyarray>\n";
  ofs << "        </dir>\n";
  ofs << "        <dir name=\"tdc_front\">\n";
  ofs << "          <key name=\"channel\" type=\"INT\">4</key>\n";
  ofs << "          <key name=\"slope\" type=\"DOUBLE\">0.1</key>\n";
  ofs << "          <key name=\"offset\" type=\"DOUBLE\">0</key>\n";
  ofs << "        </dir>\n";
  ofs << "        <dir name=\"tdc_back\">\n";
  ofs << "          <key name=\"channel\" type=\"INT\">5</key>\n";
  ofs << "          <key name=\"slope\" type=\"DOUBLE\">0.1</key>\n";
  ofs << "          <key name=\"offset\" type=\"DOUBLE\">0</key>\n";
  ofs << "        </dir>\n";
  ofs << "      </dir>\n";
  ofs << "    </dir>\n";
  ofs << "  </dir>\n";
  ofs << "</odb>\n";
  ofs.close();
  std::cout << "ATTENTION: Current odb state saved as " << outfile << "\n";
}

////////////////////// Class dragon::utils::BGOCalibrator //////////////////////
////////////////////////////////////////////////////////////////////////////////
/// Construct from tree with heavy ion singles data & database containing
/// variables
/// \param t Pointer to a `TTree` containing heavy-ion singles data
/// \param db Pointer to a database containing the variables with which the
///        `bgo.esort[0]` data in _t_ were calculated.
/// \todo write BGOCalibrator class to use &gamma; singles data to calibrate the
///       BGO array using the room background lines from <sup>40</sup>K
///       (<em>E<sub>&gamma;</sub> = 1.461</em> MeV) and <sup>208</sup>Tl
///       (<em>E<sub>&gamma;</sub> = 2.614</em>).
// dutils::BgoCalibrator::BgoCalibrator(TTree* t, midas::Database* db):
//   fTree(t), fDb(db)
// {
//   if(!fDb) return;
//   Double_t slopes[NBGO], offsets[NBGO];
//   db->ReadArray("/dragon/bog/variables/adc/slope",  slopes, NBGO);
//   db->ReadArray("/dragon/bgo/variables/adc/offset", offsets, NBGO);
//   for(Int_t i=0; i< NBGO; ++i) {
//     fOldParams[i].slope = slopes[i];
//     fOldParams[i].offset = offsets[i];
//   }
// }
