///
/// \file DragonChain.C
/// \brief Example of how to chain a series of runs together.
/// \details You can run this macro in a ROOT session as follows:
/// \code
/// .include "/path/to/dragon/analyzer/src"
/// gSystem->Load("/path/to/dragon/analyzer/lib/libDragon.so");
/// 
/// .x DragonChain.C
/// \endcode
/// Note that the first two lines only have to be called once in a ROOT session. You may want
/// to create a file in your home directory called `.rootlogon.C` and add those lines to it so that
/// you don't have to worry about them every time you start ROOT. Since the first command is a CINT
/// literal, if you want to include it in a macro you will need to wrap it inside a call to
/// `gROOT->ProcessLine()`, i.e.
/// \code
///  gROOT->ProcessLine(".include \"/path/to/dragon/analyzer/src\"");
/// \endcode
///
/// If you have successfully loaded this file as shown above, you will then have access to the
/// following chains:
///   - t1    Head singles events
///   - t2    Head scaler events
///   - t3    Tail singles events
///   - t4    Tail scaler events
///   - t5    Coincidence events
///   - t6    Timestamp diagnostics
///   - t7    Global run parameters
///   - t20   Epics events
///
/// You can use them just as you would a single `TTree`, for example to histogram
/// the dsssd front strip energy
/// \code
/// t3->Draw("dsssd.efront>>hefront(256, 0, 4096)");
/// \endcode
///

/// \cond HIDDEN
{
	// Fill this array with the run numbers that you want to chain together.
	Int_t runnumbers[] = {
		397, 398, 399
	};
	// This line figures out how many runs are in the array `runnumbers`.
	Int_t nruns = sizeof(runnumbers) / sizeof(Int_t);

	// This line uses the dragon::MakeChains() function to create chains for all
	// all trees in the ROOT files corresponding to the desired run numbers. In your
	// case you should change the last argument to correspond to the actual location
	// and naming format of your ROOT files. The last argument means that we expect
	// the file names to expand to ~/data/dragon/DAQ_test/data/rootfiles/run397.root, etc.
	dragon::MakeChains(runnumbers, nruns, "~/data/dragon/DAQ_test/data/rootfiles/run%d.root");	
}
/// \endcond
