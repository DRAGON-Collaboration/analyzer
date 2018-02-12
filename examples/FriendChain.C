///
/// \file FriendChain.C
/// \brief Example of how to add a set of files as friends to an existing chain
/// \details
/// \details Here is an example of how to friend a set of files that contain only calibrated
///  DSSSD data. It assumes that yoy have already created the dssssd files using `DsssdCalibrate.C`
/// \code
/// .include "/path/to/dragon/analyzer/src"
/// gSystem->Load("/path/to/dragon/analyzer/lib/libDragon.so");
///
/// .x FriendChain.C
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
/// When you have successfully executed this file, you will have full access to the "friended" branches,
/// as if they were part of the initial tree. You can, for example, draw the calibrated DSSSD front strip
/// energy vs. the uncalibrated one:
/// \code
/// t3->Draw("recal.dsssd.efront:dsssd.efront>>hefront(100,0,3000,100,0,300)"); // (singles)
/// t5->Draw("recal.dsssd.efront:tail.dsssd.efront>>hefront_coinc(100,0,3000,100,0,300)"); // (coincidences)
/// \endcode
///

/// \cond HIDDEN
{
	// This line executes the code in `DragonChain.C`, i.e. it creates the initial, base chain to which
	// you want to add friends.
	gROOT->ProcessLine(".x DragonChain.C");

	// This line adds calibrated singles data as friends to the chains created in DragonChain.C
	// The file formats should correspond to the actual location and naming scheme of your real files
	// (note that the `%d` gets substituted with the actual run number).
	dragon::FriendChain(t3, "t3_dsssdcal", "recal",
						"~/data/dragon/DAQ_test/data/rootfiles/run%d.root",
						"~/data/dragon/DAQ_test/data/rootfiles/run%d_dsssd_recal.root");

	// This line does the same as above, except for coincidences
	dragon::FriendChain(t5, "t5_dsssdcal", "recal",
						"~/data/dragon/DAQ_test/data/rootfiles/run%d.root",
						"~/data/dragon/DAQ_test/data/rootfiles/run%d_dsssd_recal.root");
}
/// \endcond
