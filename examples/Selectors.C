///
/// \file Selectors.C
/// \author G. Christian
/// \brief Contains example code for using the DRAGON selector classes
/// \details To run (from within a ROOT session):
/// \code
///  .include "$DRAGONSYS/src"
///  .x Selector.C+
/// \endcode
///
#include <TH1F.h>
#include "utils/RootAnalysis.hxx"


/// Example selector class for head singles events
class ExampleHeadSelector : public dragon::HeadSelector
{
public:
	/// Histogram to view analysis results
	TH1F* fHist;

public:
	/// Initialize fHist pointer to NULL
	ExampleHeadSelector():	fHist(0) { }
	/// Set fHist to a new histogram
	void Begin(TTree*)
	{
		fHist = new TH1F("esort0", "", 320, 0, 16);
	}
	/// Read the current entry from the chain, fill fHist with `bgo.esort[0]`
	/// if it passes a cut condition.
	Bool_t Process(Long64_t entry)
	{
		if(!fChain) Abort("fChain == 0");
		if(!fHist)  Abort("fHist == 0");

		// this->GetEntry(entry); // slower, reads all branches
		b_gamma_bgo_esort->GetEntry(entry); // faster, reads only the branch we want
		if(bgo_esort[0] > 0) {
			fHist->Fill(bgo_esort[0]);
		}
		return kTRUE; // (ignored)
	}
	/// Draw the results of the analysis
	void Terminate()
	{
		if(fHist) fHist->Draw();
	}
	/// Free fHist memory
	~ExampleHeadSelector()
	{
		if(fHist) delete fHist;
	}
};

/// Example selector class for scaler events
/**
 *  Fills a histogram with the SB 0 counts, then calculates the
 *  scaler sum in two different ways.
 */
class ExampleScalerSelector : public dragon::ScalerSelector
{
public:
	/// 1D histogram of SB 0 rate
	TH1* fSb0;

public:
	/// Initialize fSb0 pointer to NULL
	ExampleHeadSelector():	fSb0(0) { }

	/// Called at the beginning of a loop
	void Begin(TTree*)
	{
		/// Set fSb0 to a new histogram
		if(fSb0) delete fSb0;
		fSb0 = new TH1I("sb0_count", "SB 0 counts per readout", 3600, 0, 3600);
	}

	/// Called event-by-event in a loop
	Bool_t Process(Long64_t entry)
	{
		///
		/// Reads event data, increments histogram bin

		if(!fChain) Abort("fChain == 0");
		if(!fSb0)  Abort("fSb0 == 0");

		this->GetEntry(entry); // reads all branches

		/// \note Two "gotchas" involved in filling the histogram
		///   1. The bin number should be `entry + 1`, not `entry`. This is
		///      because bin 0 is for underflow counts. In other words, ROOT
		///      histograms start counting from 1.
		///   2. We set the bin content to `count[10]` and not `rate[10]`. This is
		///      so that the integral of the histogram will match the actual sum
		///      of counts. Had we used `rate` instead, we would have to correct the
		///      integral for the difference bewteen the frontend CPU clock (which
		///      triggers a scaler read every second) and the IO32 clock (which calculates
		///      a more precise time in between scaler reads and uses this to calculate
		///      the `rate` parameter).
		///
		fSb0->SetBinContent(entry + 1, count[10]); // [10] == SB 0

		return kTRUE; // (ignored)
	}

	/// Called at the end of processing
	void Terminate()
	{
		/// Draw the SB 0 count histogram, calculate the sum both from it's
		/// integral and from the last entry of `sum[10]`.
		fSb0->Draw();

		fChain->GetEntry(fChain->GetEntries() - 1);
		std::cout << "\nSB 0 sum:\nMethod 1 (integral of 'count[10]'): "
				  << fSb0->Integral() << "\nMethod 2 (`sum[10] for last event): "
				  << sum[10] << std::endl;
	}

	/// Deltes fSb0
	~ExampleHeadSelector()
	{
		if(fSb0) delete fSb0;
	}
};

/// Example routine using the DRAGON scaler selector to loop over a tree.
void RunScalerSelector()
{
	ExampleScalerSelector *selector1 = new ExampleScalerSelector();
	TFile* _file = TFile::Open("$DH/rootfiles/run399.root");
	TTree* t4 = (TTree*)_file->Get("t4");
	t4->Process(selector1);
}

/// Example routine using the DRAGON selector classes to loop over a chain
/// of files and fill histograms.
void RunHeadSelector()
{
	ExampleHeadSelector *selector1 = new ExampleHeadSelector();
	TChain ch1("t1");
	ch1.AddFile("$DH/rootfiles/run399.root");
	//	ch1.AddFile("$DH/rootfiles/run169.root");
	ch1.Process(selector1);
}
