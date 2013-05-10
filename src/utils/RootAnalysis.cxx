///
/// \file RootAnalysis.cxx
/// \author G. Christian
/// \brief Implements RootAnalysis.hxx
///
#include <vector>
#include <string>
#include <cassert>
#include <sstream>
#include <algorithm>

#include <TMath.h>
#include <TClass.h>
#include <TGraph.h>
#include <TString.h>
#include <TSystem.h>
#include <TThread.h>
#include <TDataMember.h>
#include <TTreeFormula.h>

#include "midas/Database.hxx"
#include "Dragon.hxx"
#include "Constants.hxx"
#include "ErrorDragon.hxx"
#include "RootAnalysis.hxx"


template <class T> void Zap(T*& t) 
{
	if(!t) return;
	delete t;
	t = 0;
}


// ============ dragon::MakeChains ============ //

void dragon::MakeChains(const Int_t* runnumbers, Int_t nruns, const char* format)
{
	///
	/// \param [in] runnumbers Pointer to a valid array of desired run numbers to chain together.
	/// \param [in] nruns Length of _runnumbers_.
	/// \param format String specifying the file name format, uses _printf_ syntax.
	/// 
	/// \note Does not return anything, but creates `new` heap-allocated TChains that become part of
	/// the present ROOT directory; these must be deleted by the user.
	TChain* chain[] = {
		new TChain("t1", "Head singles event."),
		new TChain("t2", "Head scaler event."),
		new TChain("t3", "Tail singles event."),
		new TChain("t4", "Tail scaler event."),
		new TChain("t5", "Coincidence event."),
		new TChain("t20", "Epics event."),
		new TChain("t6", "Timestamp diagnostics."),
		new TChain("t7", "Glocal run parameters.")
	};
	Int_t nchains = sizeof(chain) / sizeof(TChain*);

	for(Int_t i=0; i< nruns; ++i) {
		char fname[4096];
		sprintf(fname, format, runnumbers[i]);
		{
			TFile file(fname);
			if(file.IsZombie()) {
				dragon::utils::Warning("MakeChains")
					<< "Skipping run " << runnumbers[i] << ", couldn't find file " << fname;
			}
		}

		for(int j=0; j< nchains; ++j) {
			chain[j]->AddFile(fname);
		}
	}
}

void dragon::MakeChains(const std::vector<Int_t>& runnumbers, const char* format)
{
	MakeChains(&runnumbers[0], runnumbers.size(), format);
}


// ============ Class dragon::TTreeFilter ============ //

dragon::TTreeFilter::TTreeFilter(const char* filename, const char* option, const char* ftitle, Int_t compress):
	fRunThreaded(kTRUE)
{
	/// Create a new TFile to be used for filter output.
	/// For explanation of parameters, see the <a href = http://root.cern.ch/root/html/TFile.html#TFile:TFile@2>
	/// TFile constructor documentation</a>.
	TDirectory* current = gDirectory;
	fFileOwner = kTRUE;
	fDirectory = new TFile(filename, option, ftitle, compress);
	if(IsZombie()) { Zap(fDirectory);	}
	if(current) current->cd();
}

dragon::TTreeFilter::TTreeFilter(TDirectory* output):
	fRunThreaded(kTRUE)
{
	/// Create a filter into a currently existing directory (file).
	/// \param output Currently existing directory in which to place the output tree.
	/// \note Takes no ownership of _output_.
	fFileOwner = kFALSE;
	fDirectory = output;
}

dragon::TTreeFilter::~TTreeFilter()
{
	///
	/// Delete fDirectory if we own it
	if(fFileOwner) Zap(fDirectory);
}

Bool_t dragon::TTreeFilter::IsZombie() const
{
	if(!fDirectory) return kTRUE;
	return fDirectory->IsZombie();
}

Bool_t dragon::TTreeFilter::CheckCondition(TTree* tree) const
{
	utils::ChangeErrorIgnore errIgnore(10001);
	const char* condition = GetFilterCondition(tree);
	if(!condition) return kFALSE;

	Long64_t N = tree->Draw("1", condition, "goff", 1);
	return N >= 0;
}

void dragon::TTreeFilter::SetOutDir(TDirectory* directory)
{
	if(IsFileOwner()) Zap(fDirectory);
	fDirectory = directory;
	fFileOwner = kFALSE;
}

void dragon::TTreeFilter::Close()
{
	if(!IsFileOwner()) {
		utils::Warning("TTreeFilter::CloseOutDir")
			<< "Not the owner of the directory at " << fDirectory << ", unable to close.";
		return;
	}

	for(Map_t::iterator i = fInputs.begin(); i != fInputs.end(); ++i) {
		if(i->second.fTree) i->second.fTree->AutoSave();
	}
	Zap(fDirectory);
	for(Map_t::iterator i = fInputs.begin(); i != fInputs.end(); ++i) {
		i->second.fTree = 0;
	}
}

void dragon::TTreeFilter::SetFilterCondition(TTree* tree, const char* condition)
{
	///
	/// \param tree The input TTree to be filtered
	/// \param condition String specifying the filter condition. Should be a boolean
	///  expresion involving valid parameters of _tree_. See ROOT's 
	///  <a href=http://root.cern.ch/root/html/TTree.html#TTree:Draw@2> ocumentation on
	///  TTree::Draw()</a> for more information.
	/// \note If _tree_ has already been set as an input, the effect of this method is
	///  to reset the filter condition to _condition_.
	Out_t out = { 0, condition };
	std::pair<Map_t::iterator, bool> insrt =
		fInputs.insert(std::make_pair(tree, out));
	if(insrt.second == false) // key already existed, update condition
		insrt.first->second = out;
}

const char* dragon::TTreeFilter::GetFilterCondition(TTree* tree) const
{
	Map_t::const_iterator i = fInputs.find(tree);
	return i != fInputs.end() ? i->second.fCondition.Data() : 0;
}

namespace {
struct ThreadArgs_t {
	TTree*  fIn;  // input TTree
	TTree** fOut; // output TTree
	TDirectory* fOutDir;    // output directory
	const char* fCondition; // filter condition
};

void * run_thread(void* input)
{
	//
	// NOTE: input must point to a valid ThreadArgs_t struct
	Long64_t *nfiltered = new Long64_t(0);
	TDirectory* current = 0;
	ThreadArgs_t* args = (ThreadArgs_t*)input;
	{
		TThread::Lock();
		current = gDirectory;
		args->fOutDir->cd();
		TThread::UnLock();
	}
	*(args->fOut) = args->fIn->CopyTree(args->fCondition);
	(*(args->fOut))->AutoSave();
	*nfiltered = (*(args->fOut))->GetEntries();
	{
		TThread::Lock();
		current->cd();
		TThread::UnLock();
	}
	return (void*)nfiltered;
} }

Int_t dragon::TTreeFilter::Run()
{
	if(IsZombie()) {
		utils::Error("TTreeFilter::Run") << "Zombie output directory.";
		return -1;
	}
	if(fInputs.empty()) {
		utils::Error("TTreeFilter::Run") << "No inputs to filter.";
		return -1;
	}

	std::vector<std::pair<TThread*, ThreadArgs_t*> > threads;
	threads.reserve(fInputs.size());

	for(Map_t::iterator i = fInputs.begin(); i != fInputs.end(); ++i) {
		if(CheckCondition(i->first) == kFALSE) {
			utils::Warning("TTreeFilter::Run") 
				<< "Invalid filter condition: \"" << GetFilterCondition(i->first)
				<< "\" for TTree at " << i->first << ", skipping.";
			continue;
		}
		
		// Construct arguments
		ThreadArgs_t *args = new ThreadArgs_t();
		args->fIn     = i->first;
		args->fOut    = &(i->second.fTree);
		args->fOutDir = GetOutDir();
		args->fCondition = i->second.fCondition;

		// Construct thread and add to list
		TThread *thread =	GetThreaded() ? new TThread(run_thread, args) : 0;
		threads.push_back(std::make_pair(thread, args));
	}
	
	//
	// Initial message
	std::cout 
		<< "Running the following filters:\n"
		<< "\t<tree name>, <num events>, <filter condition>\n";
	for(size_t i=0; i< threads.size(); ++i)
		std::cout << "\t" << threads[i].second->fIn->GetName()    << ", "
							<<         threads[i].second->fIn->GetEntries() << ", \""
							<<         threads[i].second->fCondition << "\"\n";
	std::cout << "\nIf there are many events, this may take a while...\n\n";

	//
	// Run all threads
	std::vector<void*> nout(threads.size(), 0);
	for(size_t i=0; i< threads.size(); ++i) {
		if(GetThreaded())
			threads[i].first->Run();
		else
			nout[i] = run_thread(threads[i].second);
	}
	//
	// Join all threads
	if(GetThreaded()) {
		for(size_t i=0; i< threads.size(); ++i) {
			threads[i].first->Join( &(nout[i]) );
		}
	}
	//
	// Message & cleanup
	std::cout << "Done!\nNumber of events written:\n"
						<< "\t<tree name>, <num events>\n";
	for(size_t i=0; i< threads.size(); ++i) {
		Long64_t* nout64 = (Long64_t*) nout[i];
		Long64_t nnn = nout64 ? *nout64 : 0;
		std::cout << "\t" << threads[i].second->fIn->GetName() << ", " << nnn << "\n";

		if(GetThreaded()) 
			delete threads[i].first;
		if(1)
			delete threads[i].second;
		if(nout64)
			delete nout64;
	}

	return 0;
}


// ============ Class dragon::RossumData ============ //

dragon::RossumData::RossumData():
	fFile(0)
{
	SetCups();
}

dragon::RossumData::RossumData(const char* filename):
	fFile(0)
{
	SetCups();
	bool opened = OpenFile(filename, kTRUE);
	if(!opened) CloseFile();
}

dragon::RossumData::~RossumData()
{
	for(std::map<Int_t, TTree*>::iterator it = fTrees.begin();
			it != fTrees.end(); ++it) {
		if(it->second)
			it->second->Delete();
	}
}

void dragon::RossumData::SetCups()
{
	fWhichCup["fc4"]  = 0;
	fWhichCup["fc1"]  = 1;
	fWhichCup["fcch"] = 2;
	fWhichCup["fcm"]  = 3;
	fWhichCup["fcf"]  = 4;
}

void dragon::RossumData::CloseFile()
{
	fFile.reset(0);
}

Bool_t dragon::RossumData::OpenFile(const char* filename, Bool_t parse)
{
	/// \param filename Path to the rossum file
	/// \param parse If true, go ahead and attempt parsing the specified rossum file;
	///  If false, a separate call to ParseFile() will be needed.
	TString filename2 = filename;
	Bool_t expand = gSystem->ExpandPathName(filename2);
	if(!expand)
		fFile.reset(new ifstream(filename2));
	else
		std::cerr << "Invalid file: " << filename2 << "\n";

	Bool_t retval = fFile.get() && fFile->good();
	if(!retval) CloseFile();
	else if(parse) ParseFile();
	return retval;
}

TTree* dragon::RossumData::MakeTree()
{
	TTree* tree = new TTree("tcup", "");
	tree->Branch("cup", &fCup, "cup/I");
	tree->Branch("iteration", &fIteration, "iteration/I");
	tree->Branch("time", &fTime, "time/D");
	tree->Branch("current", &fCurrent, "current/D");
	return tree;
}

Bool_t dragon::RossumData::ParseFile()
{
	if(!fFile.get()) {
		std::cerr << "Error: No rossum data file open.\n";
		return kFALSE;
	}

	Bool_t retval = kTRUE;
	fFile->seekg(0);
	while (1) {
		std::string line0;
		std::getline(*fFile, line0);
		if(!fFile->good())
			break;
		if(line0.substr(0, 5) == "START") {
			TTree* tree = MakeTree();
			tree->SetDirectory(0);
			std::map<Int_t, Int_t> cupIteration;
			Bool_t fullRun = kFALSE;

			while(1) {
				std::getline(*fFile, line0);
				if(!fFile->good())
					break;
				else if(line0.substr(0, 4) == "STOP")
					break;
				else if(line0.substr(0, 5) == "begin") {
					size_t curpos = line0.find("Current");
					if(curpos > line0.size()) {
						std::cerr << "Error: couldn't parse cup name from line: "
											<< line0 << "\n";
						retval = kFALSE;
						continue;
					}
					std::string which = line0.substr(6, curpos-6);
					std::map<std::string, Int_t>::iterator itWhich = fWhichCup.find(which);
					if(itWhich == fWhichCup.end()) {
						std::cerr << "Error: couldn't parse cup name from line: "
											<< line0 << "\n";
						retval = kFALSE;
						continue;
					}
					{
						fCup = itWhich->second;
						std::map<Int_t, Int_t>::iterator itCupIteration = cupIteration.find(fCup);
						if(itCupIteration == cupIteration.end())
							cupIteration.insert(std::make_pair(fCup, 0));
						else
							++(itCupIteration->second);
						itCupIteration = cupIteration.find(fCup);
						fIteration = itCupIteration->second;
					}
					while(1) {
						std::getline(*fFile, line0);
						TString line(line0);
						if(!fFile->good())
							break;
						else if(line.Contains("end"))
							break;
						else {
							std::auto_ptr<TObjArray> tok (line.Tokenize("\t"));
							if(!tok.get() || tok->GetEntries() != 2) {
								std::cerr << "Error: invalid current read line: " << line;
								if(tok.get())
									std::cerr << " : ntokens == " << tok->GetEntries() << "\n";
								else
									std::cerr << " : tok.get() == 0\n";
								retval = kFALSE;
								continue;
							}
							fTime =    ((TObjString*)tok->At(0))->GetString().Atof();
							fCurrent = ((TObjString*)tok->At(1))->GetString().Atof();
							tree->Fill();
						}
					}
				}
				else if(line0.find("STARTED midas run") < line0.size()) {
					TString line =
						line0.substr(line0.find("STARTED")+std::string("STARTED midas run ").size()).c_str();

					Int_t runnum = line.Atoi();
					fTrees.insert(std::make_pair(runnum, tree));
					fullRun = kTRUE;
					std::stringstream ssname, sstitle;
					ssname << "tcup" << runnum;
					sstitle << "Farady cup readings proceeding run " << runnum;
					tree->SetNameTitle(ssname.str().c_str(), sstitle.str().c_str());
				}
			}
			tree->ResetBranchAddresses();
			if(!fullRun) tree->Delete();
		}
	}
	return retval;
}


TTree* dragon::RossumData::GetTree(Int_t runnum)
{
	std::map<Int_t, TTree*>::iterator it = fTrees.find(runnum);
	return it == fTrees.end() ? 0 : it->second;
}

Double_t dragon::RossumData::AverageCurrent(Int_t run, Int_t cup, Int_t iteration,
																						Double_t skipBegin, Double_t skipEnd)
{
	///
	/// \param run Run number from which to get cup readings; this will look at the
	///  readings just before starting the specified run number
	/// \param cup Specifies the farady cup from which to read, in order of
	///  upstream -> downstream:
	///     - 0 : FC4
	///     - 1 : FC1
	///     - 2 : FCCH
	///     - 3 : FCM
	///     - 4 : FCF
	/// \param iteration If more than one cup reading was taken, specifices which reading
	///  to take the average of (0 == first iteration).
	/// \param skipBegin Length of time in seconds to skip at the beginning of the cup readings
	/// \param skipEnd Length of time in seconds to skip at the end of the cup readings
	///
	TTree* tree = GetTree(run);
	if(!tree) {
		std::cerr << "Error: invalid run " << run << "\n";
		return 0.;
	}

	tree->SetBranchAddress("cup", &fCup);
	tree->SetBranchAddress("time", &fTime);
	tree->SetBranchAddress("current", &fCurrent);
	tree->SetBranchAddress("iteration", &fIteration); 
	int entry0 = tree->GetEntries() - 1;
	while(entry0 >= 0) {
		tree->GetEntry(entry0);
		--entry0;
		if(fCup == cup) break;
	}
	Double_t tbegin = -1, tlast = fTime;
	std::vector<double> readings;

	for(Long64_t i=0; i< tree->GetEntries(); ++i) {
		tree->GetEntry(i);
		if(fCup != cup) continue;
		if(tbegin < 0) tbegin = fTime;
		if(tbegin > 0 &&
			 fTime > tbegin + skipBegin &&
			 fTime < tlast  - skipEnd   &&
			 fIteration == iteration)
		{
			readings.push_back(fCurrent);
		}
	}

	Double_t retval = 0;
	if(!readings.size()) {
		std::cerr << "No readings for cup " << cup << ", iteration " << iteration << " in run " << run << "\n";
	}
	else {
		retval = TMath::Mean(readings.size(), &readings[0]);
	}

	tree->ResetBranchAddresses();
	return retval;
}

void dragon::RossumData::ListTrees() const
{
	for (std::map<Int_t, TTree*>::const_iterator it = fTrees.begin(); it != fTrees.end(); ++it) {
		std::cout << it->second->GetName() << "\t" << it->second->GetTitle() << "\n";
	}
}


// ============ Class dragon::BeamNorm ============ //

dragon::BeamNorm::BeamNorm(const char* rossumFile):
	fRossum(0)
{
	ChangeRossumFile(rossumFile);
}

void dragon::BeamNorm::ChangeRossumFile(const char* name)
{
	fRossum.reset(new RossumData(name));
}

Int_t dragon::BeamNorm::ReadSbCounts(TFile* datafile, Double_t pkLow0, Double_t pkHigh0,
																		 Double_t pkLow1, Double_t pkHigh1,Double_t time)
{
	if(!HaveRossumFile()) {
		std::cerr << "Error: no rossum file loaded.\n";
		return 0;
	}
	if(!datafile || datafile->IsZombie()) {
		std::cerr << "Invalid datafile: " << datafile << "\n";
		return 0;
	}

	Bool_t haveRunnum = kFALSE;
	Int_t runnum = 0;
	midas::Database* db = static_cast<midas::Database*>(datafile->Get("odbstop"));
	if(db) haveRunnum = db->ReadValue("/Runinfo/Run number", runnum);
	if(!db || !haveRunnum) {		 
		std::cerr << "Error: couldn't read run number from TFile at " << datafile << "\n";
		return 0;
	}

	TTree* t3 = static_cast<TTree*>(datafile->Get("t3"));
	if(t3 == 0 || t3->GetListOfBranches()->At(0) == 0) {
		std::cerr << "Error: no heavy-ion data tree in file" << datafile->GetName() << "\n";
		return 0;
	}

	TTree* t4 = static_cast<TTree*>(datafile->Get("t4"));
	if(t4 == 0 || t4->GetListOfBranches()->At(0) == 0) {
		std::cerr << "Error: no heavy-ion scaler tree in file" << datafile->GetName() << "\n";
		return 0;
	}

	TTree* t20 = static_cast<TTree*>(datafile->Get("t20"));
	if(t20 == 0 || t20->GetListOfBranches()->At(0) == 0) {
		std::cerr << "Error: no EPICS tree in file" << datafile->GetName() << "\n";
		return 0;
	}

	dragon::Tail tail;
	dragon::Tail *pTail = &tail;
	t3->SetBranchAddress(t3->GetListOfBranches()->At(0)->GetName(), &pTail);

	dragon::Scaler scaler;
	dragon::Scaler* pScaler = &scaler;
	t4->SetBranchAddress(t4->GetListOfBranches()->At(0)->GetName(), &pScaler);

	dragon::Epics epics;
	dragon::Epics* pEpics = &epics;
	t20->SetBranchAddress(t20->GetListOfBranches()->At(0)->GetName(), &pEpics);
		
	Long64_t ncounts[dragon::SurfaceBarrier::MAX_CHANNELS];
	Long64_t ncounts_full[dragon::SurfaceBarrier::MAX_CHANNELS];
	for(int i=0; i< dragon::SurfaceBarrier::MAX_CHANNELS; ++i) {
		ncounts[i] = 0;
		ncounts_full[i] = 0;
	}

	t3->GetEntry(0);
	Double_t tstart = pTail->header.fTimeStamp;
	Double_t low[dragon::SurfaceBarrier::MAX_CHANNELS] =  { pkLow0, pkLow1 };
	Double_t high[dragon::SurfaceBarrier::MAX_CHANNELS] =  { pkHigh0, pkHigh1 };

	for(int i=0; i< dragon::SurfaceBarrier::MAX_CHANNELS; ++i) {
		std::stringstream cut;
		cut.precision(20);
		cut << "sb.ecal[" << i << "] > " << low[i] << " && sb.ecal[" << i << "] < " << high[i];
		ncounts_full[i] = t3->GetPlayer()->GetEntries(cut.str().c_str());
		cut << " && header.fTimeStamp - " << tstart << " < " << time;
		ncounts[i] = t3->GetPlayer()->GetEntries(cut.str().c_str());
	}

	if(time < t4->GetEntries())
		t4->GetEntry(time);
	else
		t4->GetEntry(t4->GetEntries() - 1);
	double live = pScaler->sum[13] / double(pScaler->sum[14] + pScaler->sum[15]);

	t4->GetEntry(t4->GetEntries() - 1);
	double live_full = pScaler->sum[13] / double(pScaler->sum[14] + pScaler->sum[15]);


	t20->GetEntry(0);
	Double_t tstart20 = pTail->header.fTimeStamp;
	Int_t t1 = 0;
	std::vector<double> pressure;

	for(Long64_t entry = 0; entry != t20->GetEntries(); ++entry) {
		t20->GetEntry(entry);
		if(pEpics->ch == 0) {
			pressure.push_back(pEpics->val);
			if(pEpics->header.fTimeStamp - tstart < time)
				++t1;
		}
	}

	RunData* rundata = GetRunData(runnum);
	if(!rundata) {
		RunData ddd;
		fRunData.insert(std::make_pair(runnum, ddd));
		rundata = GetRunData(runnum);
	}
	assert(rundata);

	rundata->time = time;
	for(int i=0; i< dragon::SurfaceBarrier::MAX_CHANNELS; ++i) {
		rundata->sb_counts[i] = ncounts[i];
		rundata->sb_counts_full[i] = ncounts_full[i];
	}
	rundata->pressure = TMath::Mean(t1, &pressure[0]);
	rundata->pressure_full = TMath::Mean(pressure.size(), &pressure[0]);
	rundata->live_time = live;
	rundata->live_time_full = live_full;

	t3->ResetBranchAddresses();
	t4->ResetBranchAddresses();
	t20->ResetBranchAddresses();
	return runnum;
}

void dragon::BeamNorm::ReadFC4(Int_t runnum, Double_t skipBegin, Double_t skipEnd)
{
	if(!HaveRossumFile()) {
		std::cerr << "Error: no rossum file loaded!\n";
		return;
	}

	RunData* rundata = GetRunData(runnum);
	if(!rundata) {
		RunData ddd;
		fRunData.insert(std::make_pair(runnum, ddd));
		rundata = GetRunData(runnum);
	}
	assert(rundata);

	for(int i=0; i< 3; ++i) {
		rundata->fc4[i] = fRossum->AverageCurrent(runnum, 0, i, skipBegin, skipEnd);
	}
}

void dragon::BeamNorm::CalculateNorm(Int_t run, Int_t chargeState)
{
	RunData* rundata = GetRunData(run);
	if(!rundata) {
		std::cerr << "No SB or rossum data!\n";
		return;
	}

	for(Int_t i=0; i< dragon::SurfaceBarrier::MAX_CHANNELS; ++i) {
		if(rundata->sb_counts[i] != 0) {
			Double_t fc4avg = TMath::Mean(3, rundata->fc4);
			rundata->sbnorm[i]  = fc4avg * rundata->pressure * rundata->time;
			rundata->sbnorm[i] /= (Constants::ElectronCharge() * chargeState * (rundata->sb_counts[i] / rundata->live_time));
			
			if(rundata->pressure_full != 0) {
				rundata->nbeam[i] = (rundata->sb_counts_full[i] / rundata->live_time_full) * rundata->sbnorm[i] / rundata->pressure_full;
			}
		}
	}
}

dragon::BeamNorm::RunData* dragon::BeamNorm::GetRunData(Int_t runnum)
{
	std::map<Int_t, RunData>::iterator i = fRunData.find(runnum);
	return i == fRunData.end() ? 0 : &(i->second);
}

void dragon::BeamNorm::GetParams(const char* param, std::vector<Double_t> *runnum, std::vector<Double_t> *parval)
{
	runnum->clear();
	parval->clear();
	for(std::map<Int_t, RunData>::iterator it = fRunData.begin(); it != fRunData.end(); ++it) {
		RunData* rundata = &(it->second);

		TTree t("temptree","");
		t.SetCircular(0);
		t.Branch("rundata", "dragon::BeamNorm::RunData", &rundata);
		t.Fill();
		TTreeFormula f("f", param, &t);
		if(f.GetNdim() == 0) {
			std::cerr << "Invalid parameter: \"" << param << "\".\n";
			break;
		}
		runnum->push_back(it->first);
		parval->push_back(f.EvalInstance(0));
		t.ResetBranchAddresses();
	}
}

TGraph* dragon::BeamNorm::Plot(const char* param, Marker_t marker, Color_t markerColor)
{
	/// \param param String specifying the parameter to plot, should be a member of 
	///  dragon::BeamNorm::RunData
	/// \param marker Marker symbol
	/// \param markerColor Marker color
	/// \returns TGraph pointer containing _param_ vs. run number, responsibility is on the
	///  user to delete this. In case of error, returns 0.
	/// 
	///  Also draws the returned TGraph in its own window.

	TGraph* gr = 0;
	std::vector<Double_t> runnum, parval;
	GetParams(param, &runnum, &parval);
	
	if(runnum.size()) {
		gr = new TGraph(runnum.size(), &runnum[0], &parval[0]);
		gr->SetMarkerStyle(marker);
		gr->SetMarkerColor(markerColor);
		gr->Draw("AP");
	}
	return gr;
}	

void dragon::BeamNorm::Print(const char* param1, const char* param2, const char* param3, const char* param4,
														 const char* param5, const char* param6, const char* param7, const char* param8,
														 const char* param9, const char* param10,const char* param11,const char* param12)
{
	std::vector<std::string> par;
	if(param1)  par.push_back(param1);
	if(param2)  par.push_back(param2);
	if(param3)  par.push_back(param3);
	if(param4)  par.push_back(param4);
	if(param5)  par.push_back(param5);
	if(param6)  par.push_back(param6);
	if(param7)  par.push_back(param7);
	if(param8)  par.push_back(param8);
	if(param9)  par.push_back(param9);
	if(param10) par.push_back(param10);
	if(param11) par.push_back(param11);
	if(param12) par.push_back(param12);

	std::vector<Double_t> runnum;
	std::vector<std::vector<Double_t> > parval;
	for(size_t i=0; i< par.size(); ++i) {
		std::vector<Double_t> runnum0, parval0;
		GetParams(par[i].c_str(), &runnum0, &parval0);
		if(runnum0.size()) {
			if(runnum.empty()) runnum = runnum0;
			parval.push_back(parval0);
		}
	}

	for(size_t i=0; i< runnum.size(); ++i) {
		std::cout << runnum.at(i);
		for(size_t j=0; j< parval.size(); ++j) {
			std::cout << "\t" << parval.at(j).at(i);
		}
		std::cout << "\n";
	}
}

void dragon::BeamNorm::CalculateRecoils(TFile* datafile, const char* tree, const char* gate)
{
	if(datafile == 0 || datafile->IsZombie()) {
		std::cerr << "Invalid datafile!\n";
		return;
	}
	TTree* t = (TTree*)datafile->Get(tree);
	if(!t) {
		std::cerr << "Error: no tree named \"" << tree << "\" in the specified file!\n";
		return;
	}

	Bool_t haveRunnum = kFALSE;
	Int_t runnum = 0;
	midas::Database* db = static_cast<midas::Database*>(datafile->Get("odbstop"));
	if(db) haveRunnum = db->ReadValue("/Runinfo/Run number", runnum);
	if(!db || !haveRunnum) {		 
		std::cerr << "Error: couldn't read run number from TFile at " << datafile << "\n";
		return;
	}

	Long64_t nrecoil = t->GetPlayer()->GetEntries(gate);

	RunData* rundata = GetRunData(runnum);
	if(!rundata) {
		RunData ddd;
		fRunData.insert(std::make_pair(runnum, ddd));
		rundata = GetRunData(runnum);
	}
	assert(rundata);
	
	rundata->nrecoil = nrecoil;
	for(Int_t i=0; i< dragon::SurfaceBarrier::MAX_CHANNELS; ++i) {
		if(rundata->nbeam[i] != 0)
			rundata->yield[i] = nrecoil / rundata->nbeam[i];
	}
}

void dragon::BeamNorm::BatchCalculate(TChain* chain, Int_t chargeBeam, Double_t pkLow0, Double_t pkHigh0,
																			Double_t pkLow1, Double_t pkHigh1,
																			const char* recoilGate,
																			Double_t time, Double_t skipBegin, Double_t skipEnd)
{
	std::cout << "Calculating normalization for runs... ";
	std::flush(std::cout);
	TObjArray* flist = chain->GetListOfFiles();
	for(Int_t i=0; i< flist->GetEntries(); ++i) {
		std::auto_ptr<TFile> file (TFile::Open(flist->At(i)->GetTitle()));
		Int_t runnum = ReadSbCounts(file.get(), pkLow0, pkHigh0, pkLow1, pkHigh1, time);
		if(!runnum) continue;
		ReadFC4(runnum, skipBegin, skipEnd);
		CalculateNorm(runnum, chargeBeam);
		if(recoilGate) {
			CalculateRecoils(file.get(), chain->GetName(), recoilGate);
		}
		std::cout << runnum << "... ";
		std::flush(std::cout);
	}
	std::cout << "\n";
}
