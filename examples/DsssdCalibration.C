///
/// \file DsssdCalibration.C
/// \brief Example dsssd calibration
/// \details Example of how to use the included selectors to
/// calibrate DSSSD energies for a chain of runs (singles and coincidences)
/// To run the file from within a ROOT session, do:
/// \code
/// .include "/path/to/dragon/analyzer/src"
/// gSystem->Load("/path/to/dragon/analyzer/lib/libDragon.so")
/// .x DsssdCalibration.C
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
/// The result of successful running of this script is a series of new ROOT files, with the
/// same names as the initial ones in the chain, except with ".root" replaced by "_dsssd_recal.root"
/// These files contain two Trees: `t3_dsssdcal` and `t5_dsssdcal`. The former contains the calibrated
/// DSSSD singles data and the latter the calibrated DSSSD coincidence data. See the file `FriendChain.C`
/// for an example of how to add the calibrated data as friends to the original tree.
///

/// \cond HIDDEN
{
	// First create a chain of the files we want to calibrate.
	gROOT->ProcessLine(".x DragonChain.C");

	// Note that this could have also been done manually as such:
	//   TChain* ch3 = new TChain("t3");
	//   ch3->AddFile("~/data/dragon/DAQ_test/data/rootfiles/run397.root");
	//   ch3->AddFile("~/data/dragon/DAQ_test/data/rootfiles/run398.root");
	//   ch3->AddFile("~/data/dragon/DAQ_test/data/rootfiles/run399.root");

	// Create an instance of the `dragon::DsssdCalibrate` class. This will use the
	// slopes and offsets contained in the specified XML file. To change them, edit
	// the fields `<keyarray name="slope" type="DOUBLE" num_values="32">` and
	// `<keyarray name="offset" type="DOUBLE" num_values="32">`
	// You could also do this "manually" without the XML file by passing arrays 
	// to the constructor (see commented region for an example).
	dragon::DsssdCalibrate dsssdcal("dsssd_variables.xml");

	/* "Manual" option
		 const double slopes[] = {
		 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1,
		 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1,
		 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1,
		 0.1, 0.1
		 };
		 const double offsets[] = {
		 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
		 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
		 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
		 0., 0.
		 };

		 dragon::DsssdCalibrate dsssdcal(slopes, offsets);
	 */

	// Run the calibration on all of the events in the chains you created
	t3->Process(&dsssdcal);
}
/// \endcond
