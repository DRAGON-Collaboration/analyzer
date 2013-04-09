///
/// \file Calibration.cxx
/// \brief Implements Calibration.hxx
///
#include <cstdio>
#include <cstring>
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
#include "Calibration.hxx"

namespace { const Int_t NDSSSD = dragon::Dsssd::MAX_CHANNELS; }

namespace dutils = dragon::utils;

dutils::DsssdCalibrator::DsssdCalibrator(TTree* t, midas::Database* db):
	fTree(t), fDb(db)
{
	/// \param t Pointer to a TTree containing heavy-ion singles data
	/// \param db Pointer to a database containing the variables with which
	///  the DSSSD ecal data in _t_ were calculated.
	if(!fDb) return;
	Double_t slopes[32], offsets[32];
	db->ReadArray("/dragon/dsssd/variables/adc/slope",  slopes, NDSSSD);
	db->ReadArray("/dragon/dsssd/variables/adc/offset", offsets, NDSSSD);
	for(Int_t i=0; i< NDSSSD; ++i) {
		fOldParams[i].slope = slopes[i];
		fOldParams[i].offset = offsets[i];
	}
}

void dutils::DsssdCalibrator::DrawSummary(Option_t* opt) const
{
	/// \param Opt Drawing option for the displayed histogram
	if(!fTree) return;
	if(gDirectory->Get("hdsssd"))
		gDirectory->Get("hdsssd")->Delete();

	TH2F* hdsssd = new TH2F("hdsssd", "", NDSSSD,0,NDSSSD,256,0,4096);
	dragon::Tail* tail = new dragon::Tail();
	fTree->SetBranchAddress("hi", &tail);
	for(Long_t evt = 0; evt < fTree->GetEntries(); ++evt) {
		fTree->GetEntry(evt);
		for(Int_t i=0; i< NDSSSD; ++i) {
			Double_t val = (tail->dsssd.ecal[i] - fOldParams[i].offset) / fOldParams[i].slope;
			hdsssd->Fill(i, val);
		}
	}
	hdsssd->Draw(opt);
}

dutils::DsssdCalibrator::Param_t dutils::DsssdCalibrator::GetParams(Int_t channel) const
{
	if((UInt_t)channel > 31) {
		std::cerr << "Invalid channel " << channel << ", valid range is [0, 31].\n";
		Param_t junk = {0,0};
		return junk;
	}
	return fParams[channel];
}

Double_t dutils::DsssdCalibrator::GetPeak(Int_t channel, Int_t peak) const
{
	if((UInt_t)channel > 31) {
		std::cerr << "Invalid channel " << channel << ", valid range is [0, 31].\n";
		return 0;
	}
	if((UInt_t)peak > 3) {
		std::cerr << "Invalid peak " << peak << ", valid range is [0, 3].\n";
		return 0;
	}
	return fPeaks[channel][peak];
}


std::vector<Double_t> dutils::DsssdCalibrator::FindPeaks(TH1* hst, Double_t sigma, Double_t threshold) const
{
	std::vector<Double_t> peaks;
	TSpectrum spectrum;
	spectrum.Search(hst, sigma, "goff", threshold);
	Int_t npeaks = spectrum.GetNPeaks();
	std::vector<int> indx(npeaks);
	TMath::Sort(npeaks, spectrum.GetPositionX(), &indx[0], kFALSE);
	for(Int_t i=0; i< npeaks; ++i) {
		peaks.push_back(*(spectrum.GetPositionX() + indx[i]));
	}
	return peaks;
}

Int_t dutils::DsssdCalibrator::Run(Double_t pklow, Double_t pkhigh, Double_t sigma, Double_t threshold)
{
	/// \param pklow Low edge (in uncalibrated channel number) of the region where the peak finding algorithm
	///  should search for triple alpha peaks
	/// \param pkhigh  High edge (in uncalibrated channel number) of the region where the peak finding
	///  algorithm should search for triple alpha peaks
	/// \param Sigma Minimum width of peaks to be found by the searching algorithm
	/// \param threshold Minimum peak height for the peak searching algorithm
	/// \note See TSpectrum::Search in ROOT's class documentation for more info on the _sigma_ and
	/// _threshold_ parameters
	///
	if(!fTree) return 0;
	Int_t nbins = pkhigh - pklow;
	if(nbins<0) return 0;
	nbins /= 10;
	TH1F hpeaks("hpeaks", "", nbins, pklow, pkhigh);
	Int_t retval = 0;
	for(Int_t i=0; i< 32; ++i) {
		char buf[256];
		sprintf(buf, "dsssd.ecal[%d]", i);
		fTree->Project("hpeaks", buf, "", "goff");
		std::vector<Double_t> peaks = FindPeaks(&hpeaks, sigma, threshold);
		if(peaks.size() != 3) {
			std::cerr << "Number of peaks found for channel " << i << ": " << peaks.size() << " != 3, skipping!\n";
			continue;
		} else {
			++retval;
		}
		for(int j=0; j<3; ++j)
			fPeaks[i][j] = peaks[j];
		
		FitPeaks(i);
	}

	return retval;
}

void dutils::DsssdCalibrator::FitPeaks(Int_t ch)
{
	if((UInt_t)ch > 31) return;
	const Double_t alphaEnergies[3] = {5.15659,5.48556,5.80477}; /* Alpha energies in MeV. */
	const Double_t dLayer = 0.0005; /* Deadlayer thickness in mm. */ 
	const Double_t aEloss[3] = {138.080,133.060,128.546}; /* Stopping Powers in MeV/mm. */

	Double_t aEnergy[3]; /* Actual energy deposited */																											 
	for (int i=0; i<3; i++)
		aEnergy[i] = alphaEnergies[i] - aEloss[i]*dLayer;

	TGraph gr(3);
	for(Int_t i=0; i< gr.GetN(); ++i) {
		gr.SetPoint(i, GetPeak(ch, i), aEnergy[i]);
	}
	TFitResultPtr result = gr.Fit("pol1", "qns");
	fParams[ch].slope  = result->Value(1);
	fParams[ch].offset = result->Value(0);
}

void dutils::DsssdCalibrator::PrintResults(const char* outfile)
{
	/// \param outfile String specifying a file path for output, default (0) prints to stdout.
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

	fprintf(f, "Channel\tSlope\tOffset\n");
	for(Int_t i=0; i< 32; ++i) {
		fprintf(f, "%2d\t%.6g\t%.6g\n", i, GetParams(i).slope, GetParams(i).offset);
	}

	if(f != stdout) fclose(f);
}

void dutils::DsssdCalibrator::PrintOdb(const char* outfile)
{
	/// \param outfile String specifying a file path for output, default (0) prints to stdout.
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

	for(Int_t i=0; i< 32; ++i) {
		fprintf(f, "odbedit -c \"set /dragon/dsssd/variables/adc/slope[%d] %.6g\"\n", i, GetParams(i).slope);
		fprintf(f, "odbedit -c \"set /dragon/dsssd/variables/adc/offset[%d] %.6g\"\n", i, GetParams(i).offset);
	}

	if(f != stdout) fclose(f);
}

