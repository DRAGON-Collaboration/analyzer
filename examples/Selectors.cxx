///
/// \file Selectors.cxx
/// \author G. Christian
/// \brief Contains example code for using the DRAGON selector classes
/// \details To run (from within a ROOT session):
/// \code
///  .include "$DRAGONSYS/src"
///  .x Selector.cxx+
/// \endcode
///
#include <TH1F.h>
#include "utils/RootAnalysis.hxx"


/// Example selector class for head singles events
class MyHeadSelector : public dragon::HeadSelector
{
public:
	/// Histogram to view analysis results
	TH1F* fHist;

public:
	/// Initialize fHist pointer to NULL
	MyHeadSelector():	fHist(0) { }
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
	~MyHeadSelector()
		{
			if(fHist) delete fHist;
		}
};

/// Example routine using the DRAGON selector classes to loop over a chain
/// of files and fill histograms.
void Selectors()
{
	MyHeadSelector *selector1 = new MyHeadSelector();
	TChain ch1("t1");
	ch1.AddFile("$DH/rootfiles/run399.root");
//	ch1.AddFile("$DH/rootfiles/run169.root");
	ch1.Process(selector1);
}
