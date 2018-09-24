////////////////////////////////////////////////////////////////////////////////
/// \file Calibration.hxx
/// \author G. Christian and D. Connolly
/// \brief Contains classes to help with calibrating DRAGON detectors
////////////////////////////////////////////////////////////////////////////////
#ifndef DRAGON_CALIB_HEADER
#define DRAGON_CALIB_HEADER
#include <vector>
#include <Rtypes.h>
#include "Dragon.hxx"

class TH1;
class TH2;
class TH1D;
class TH2D;
class TTree;

namespace midas { class Database; }


namespace dragon {
  namespace utils {
    ///////////////////// Class dragon::utils::DsssdCalibrator /////////////////////
    class DsssdCalibrator {
    public:
      /// Simple stuct to hold linear fit params
      struct Param_t { Double_t slope; Double_t offset; Double_t inl; };
    public:
      Double_t fMaxSlope, fMinChan;
      TH2D* fHdcal;
      TH1D* fFrontcal;
    public:
      DsssdCalibrator(TTree* t, midas::Database* db);
      void DrawSummary(Option_t* opt = "") const;
      void DrawSummaryCal(Option_t* opt = "");
      void DrawFrontCal(Option_t* opt = "");
      std::vector<Double_t> FindPeaks(TH1* hst, Double_t sigma = 2, Double_t threshold = 0.05) const;
      void GainMatch();
      Double_t GetPeak(Int_t channel, Int_t peak) const;
      Param_t GetParams(Int_t channel) const;
      Param_t GetOldParams(Int_t channel) const;
      void FitPeaks(Int_t ch, Bool_t grid = kTRUE);
      Int_t Run(Int_t nbins = 835, Double_t pklow = 500, Double_t pkhigh = 3840, Double_t sigma = 4, Double_t threshold = 0.15, Bool_t grid = kTRUE);
      void PrintResults(const char* outfile = 0);
      void PrintOdb(const char* outfile = 0);
      void WriteJson(const char* outfile = "$DH/../calibration/dsssdcal.json");
#ifdef MIDASSYS // Enable WriteOdb method which contains calls to MIDAS methods
      void WriteOdb(Bool_t json = kTRUE, Bool_t xml = kTRUE);
#endif
      void WriteXml(const char* outfile = "$DH/../calibration/dsssdcal.xml");
    private:
      TTree* fTree;
      midas::Database* fDb;
      Double_t fPeaks[dragon::Dsssd::MAX_CHANNELS][3];
      Param_t fParams[dragon::Dsssd::MAX_CHANNELS];
      Param_t fOldParams[dragon::Dsssd::MAX_CHANNELS];
    };

    ////////////////////// Class dragon::utils::BGOCalibrator //////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    /// \todo write `dutils::BgoCalibrator` class to use &gamma; singles data to
    ///       calibrate the BGO array using the room background lines from
    ///       <sup>40</sup>K (<em>E<sub>&gamma;</sub></em> = 1.461 MeV) and
    ///       <sup>208</sup>Tl (<em>E<sub>&gamma;</sub></em> = 2.614</em> MeV).
    class BgoCalibrator {
    // public:
    //   /// Simple stuct to hold linear fit params
    //   struct Param_t { Double_t slope; Double_t offset;};
    // public:
    //   Double_t fMaxSlope, fMinChan;
    //   TH2D* fSummary;
    //   TH1D* fEsort0;
    // public:
    //   BgoCalibrator(TTree* t, midas::Database* db);
    //   BgoCalibrator(TChain* chain, midas::Database* db);
    //   void DrawSummary(Option_t* opt = "") const;
    //   void DrawSummaryCal(Option_t* opt = "");
    //   std::vector<Double_t> FindPeaks(TH1* hst, Double_t sigma = 2, Double_t threshold = 0.05) const;
    //   void GainMatch();
    //   Double_t GetPeak(Int_t channel, Int_t peak) const;
    //   Param_t GetParams(Int_t channel) const;
    //   Param_t GetOldParams(Int_t channel) const;
    //   void FitPeaks(Int_t ch, Bool_t grid = kTRUE);
    //   Int_t Run(Int_t nbins = 512, Double_t pklow = 0, Double_t pkhigh = 8.5, Double_t sigma = 2, Double_t threshold = 0.15);
    //   void PrintResults(const char* outfile = 0);
    //   void PrintOdb(const char* outfile = 0);
    //   void WriteJson(const char* outfile = "$DH/../calibration/dsssdcal.json");
    //   void WriteXml(const char* outfile = "$DH/../calibration/dsssdcal.xml");
    // private:
    //   TTree* fTree;
    //   midas::Database* fDb;
    //   Double_t fPeaks[dragon::Bgo::MAX_CHANNELS][2];
    //   Param_t fParams[dragon::Bgo::MAX_CHANNELS];
    //   Param_t fOldParams[dragon::Bgo::MAX_CHANNELS];
    };

  } //utils
} //dragon

#endif
