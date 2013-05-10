///
/// \file Calibration.hxx
/// \author G. Christian
/// \brief Contains classes to help with calibrating DRAGON detectors
///
#ifndef DRAGON_CALIB_HEADER
#define DRAGON_CALIB_HEADER
#include <vector>
#include <RTypes.h>
#include "Dragon.hxx"

class TH1;
class TTree;

namespace midas { class Database; }


namespace dragon {
namespace utils {

///
/// DSSSD calibration class
class DsssdCalibrator {
public:
	/// Simple stuct to hold linear fit params
	struct Param_t { Double_t slope; Double_t offset; };
public:
	/// Construct from tree w/ heavy ion singles data & database containing variables
	DsssdCalibrator(TTree* t, midas::Database* db);
	/// Draw a summary of DSSSD energies (w/ no calibration)
	void DrawSummary(Option_t* opt = "") const;
	/// Helper routine to find triple alpha peaks in a spectrum
	std::vector<Double_t> FindPeaks(TH1* hst, Double_t sigma = 2, Double_t threshold = 0.05) const;
	/// Get the value of a particular peak as found by FindPeaks()
	Double_t GetPeak(Int_t channel, Int_t peak) const;
	/// Get the slope and offset for a given channel
	Param_t GetParams(Int_t channel) const;
	/// Helper routine to fit alpha energy vs. ADC channel
	void FitPeaks(Int_t ch);
	/// Run the full calibration
	Int_t Run(Double_t pklow = 500, Double_t pkhigh = 4100, Double_t sigma = 2, Double_t threshold = 0.05);
	/// Print a summary of the calibration results
	void PrintResults(const char* outfile = 0);
	/// Print the calibration results in a format that can be input into odbedit to update the calibration
	void PrintOdb(const char* outfile = 0);
private:
	TTree* fTree;
	midas::Database* fDb;
	Double_t fPeaks[dragon::Dsssd::MAX_CHANNELS][3];
	Param_t fParams[dragon::Dsssd::MAX_CHANNELS];
	Param_t fOldParams[dragon::Dsssd::MAX_CHANNELS];
};

} }

#endif
