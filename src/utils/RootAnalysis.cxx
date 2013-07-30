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
#include <TFitResult.h>
#include <TDataMember.h>
#include <TTreeFormula.h>

#include "midas/Database.hxx"
#include "Dragon.hxx"
#include "Constants.hxx"
#include "ErrorDragon.hxx"
#include "LinearFitter.hxx"
#include "TAtomicMass.h"
#include "RootAnalysis.hxx"


namespace { const Int_t NSB = dragon::SurfaceBarrier::MAX_CHANNELS; }

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

dragon::RossumData::RossumData(const char* name, const char* filename):
	fFile(0)
{
	SetNameTitle(name, filename);
	SetCups();
	bool opened = OpenFile(filename, kTRUE);
	if(!opened) CloseFile();
}

dragon::RossumData::~RossumData()
{
	for(TreeMap_t::iterator it = fTrees.begin(); it != fTrees.end(); ++it) {
		if(GetTree_(it)) GetTree_(it)->Delete();
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

	Int_t runnum = 0;
	Bool_t retval = kTRUE;
	fFile->seekg(0);
	while (1) {
		std::string line0;
		std::getline(*fFile, line0);
		if(!fFile->good())
			break;
		if(line0.substr(0, 5) == "START") {
			std::string strTime = "Thu Jan  1 00:00:00 1970";
			size_t posTime = line0.rfind("\t");
			if(posTime+1 < line0.size()) strTime = line0.substr(posTime+1);

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
					runnum = line.Atoi();
				}
			}
			fTrees.insert(std::make_pair(runnum, std::make_pair(tree, strTime)));
			fullRun = kTRUE;
			std::stringstream ssname, sstitle;
			ssname << "tcup" << runnum;
			if(runnum == 0) {
				static int i = 0;
				ssname << "_" << i++ ;
			}
			sstitle << "Farady cup readings proceeding run " << runnum;
			tree->SetNameTitle(ssname.str().c_str(), sstitle.str().c_str());

			tree->ResetBranchAddresses();
			if(!fullRun) tree->Delete();
		}
	}
	return retval;
}


TTree* dragon::RossumData::GetTree(Int_t runnum, const char* time) const
{
	std::pair<TreeMap_t::const_iterator, TreeMap_t::const_iterator>
		range = fTrees.equal_range(runnum);

	if(range.first == range.second) return 0;
	if(!time) return GetTree_(range.first);

	for(TreeMap_t::const_iterator it = range.first; it != range.second; ++it) {
		if ( GetTime_(it) == std::string(time) )
			return GetTree_(it);
	}
	return 0;

	// std::map<Int_t, TTree*>::const_iterator it = fTrees.find(runnum);
	// return it == fTrees.end() ? 0 : it->second;
}

UDouble_t dragon::RossumData::AverageCurrent(Int_t run, Int_t cup, Int_t iteration,
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
		return UDouble_t(0);
	}

	char gate[4096];
	sprintf(gate, "cup == %i && iteration == %i", cup, iteration);
	Long64_t nval = tree->Draw("current:time", gate, "goff");
	if(nval < 1) return UDouble_t(0);


	Double_t* time = tree->GetV2();
	Double_t* current = tree->GetV1();

	Double_t t0 = time[0];
	Double_t t1 = time[nval-1];
	Long64_t i0 = -1, i1 = -1;
	for(Long64_t i=0; i< nval; ++i) {
		if(i0 < 0 && time[i] - t0 > skipBegin)
			i0 = i;
		if(i1 < 0 && t1 - time[i] < skipEnd)
			i1 = i;
	}

	Double_t avg = TMath::Mean(i1-i0, current+i0);
	Double_t rms = TMath::RMS (i1-i0, current+i0);

	return UDouble_t(avg, rms);
}

void dragon::RossumData::ListTrees() const
{
	for (TreeMap_t::const_iterator it = fTrees.begin(); it != fTrees.end(); ++it) {
		std::cout << GetTree_(it)->GetName() << "\t" << GetTree_(it)->GetTitle() << ", DATIME: " << GetTime_(it) << "\n";
	}
}

TGraph* dragon::RossumData::PlotTransmission(Int_t* runs, Int_t nruns)
{
	std::vector<Double_t>  gruns;
	std::vector<UDouble_t> gtrans;
	for(Int_t i=0; i< nruns; ++i) {
		if(!GetTree(runs[i])) {
			std::cout << "No data for run " << runs[i] << ", skipping.\n";
			continue;
		}
		UDouble_t fc4 = AverageCurrent(runs[i], 0);
		UDouble_t fc1 = AverageCurrent(runs[i], 1);
		gruns.push_back(runs[i]);
		gtrans.push_back(fc1 / fc4);
	}

	TGraph* out = PlotUncertainties(gruns.size(), &gruns[0], &gtrans[0]);
	out->SetMarkerStyle(21);
	out->Draw("AP");
	return out;
}



// ============ Class dragon::BeamNorm ============ //


dragon::BeamNorm::BeamNorm():
	fRossum(0)
{
	;
}

dragon::BeamNorm::BeamNorm(const char* name, const char* rossumFile):
	fRossum(0)
{
	SetNameTitle(name, rossumFile);
	ChangeRossumFile(rossumFile);
}

void dragon::BeamNorm::ChangeRossumFile(const char* name)
{
	SetTitle(name);
	std::string rossumName = GetName();
	rossumName += "_rossum";
	fRossum.reset(new RossumData(rossumName.c_str(), name));
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
		
	Long64_t ncounts[NSB];
	Long64_t ncounts_full[NSB];
	for(int i=0; i< NSB; ++i) {
		ncounts[i] = 0;
		ncounts_full[i] = 0;
	}

	t3->GetEntry(0);
	Double_t tstart = pTail->header.fTimeStamp;
	Double_t low[NSB] =  { pkLow0, pkLow1 };
	Double_t high[NSB] =  { pkHigh0, pkHigh1 };

	for(int i=0; i< NSB; ++i) {
		std::stringstream cut;
		cut.precision(20);
		cut << "sb.ecal[" << i << "] > " << low[i] << " && sb.ecal[" << i << "] < " << high[i];
		ncounts_full[i] = t3->GetPlayer()->GetEntries(cut.str().c_str());
		cut << " && header.fTimeStamp - " << tstart << " < " << time;
		ncounts[i] = t3->GetPlayer()->GetEntries(cut.str().c_str());
	}

#if 0
	if(time < t4->GetEntries())
		t4->GetEntry(time);
	else
		t4->GetEntry(t4->GetEntries() - 1);
	UDouble_t live = UDouble_t(pScaler->sum[13]) / UDouble_t(pScaler->sum[14] + pScaler->sum[15]);

	t4->GetEntry(t4->GetEntries() - 1);
	UDouble_t live_full = UDouble_t(pScaler->sum[13]) / UDouble_t(double(pScaler->sum[14] + pScaler->sum[15]));
#else
	UDouble_t live, live_full;
	{
		dragon::LiveTimeCalculator ltc;
		ltc.SetFile(datafile);
		ltc.CalculateSub(0, time);
		live = ltc.GetLivetime("tail");
		ltc.Calculate();
		live_full = ltc.GetLivetime("tail");
	}		
#endif

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
	for(int i=0; i< NSB; ++i) {
		rundata->sb_counts[i] = UDouble_t(ncounts[i]);
		rundata->sb_counts_full[i] = UDouble_t(ncounts_full[i]);
	}
	rundata->pressure =
		UDouble_t (TMath::Mean(t1, &pressure[0]), TMath::RMS(t1, &pressure[0]));
	rundata->pressure_full =
		UDouble_t (TMath::Mean(pressure.size(), &pressure[0]), TMath::RMS(pressure.size(), &pressure[0]));
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
	
	rundata->fc1 = fRossum->AverageCurrent(runnum, 1, 0, skipBegin, skipEnd);
}

void dragon::BeamNorm::CalculateNorm(Int_t run, Int_t chargeState)
{
	RunData* rundata = GetRunData(run);
	if(!rundata) {
		std::cerr << "No SB or rossum data!\n";
		return;
	}

	for(Int_t i=0; i< NSB; ++i) {
		if(rundata->sb_counts[i].GetNominal() != 0) {

			UDouble_t fc4avg = UDouble_t::Mean(3, rundata->fc4);

			UDouble_t qe (TMath::Qe(), TMath::QeUncertainty());
			rundata->sbnorm[i]  = fc4avg * rundata->pressure * rundata->time;
			rundata->sbnorm[i] /= (qe * (double)chargeState * (rundata->sb_counts[i] / rundata->live_time));
			
			if(rundata->pressure_full.GetNominal() != 0) {
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

std::vector<Int_t>& dragon::BeamNorm::GetRuns() const
{
	static std::vector<Int_t> runs;
	runs.clear();
	for(std::map<Int_t, RunData>::const_iterator it = fRunData.begin(); it != fRunData.end(); ++it) {
		runs.push_back(it->first);
	}
	return runs;
}


namespace {
void parse_param(const std::string& param, std::string& par0, int& indx)
{
	size_t fpos = param.find("[");
	par0 = fpos < param.size() ? param.substr(0, fpos) : param;
	indx = fpos < param.size() ? atoi(param.substr(fpos+1).c_str()) : 0;
} }


void dragon::BeamNorm::GetParams(const char* param, std::vector<Double_t> *runnum, std::vector<UDouble_t> *parval)
{
	runnum->clear();
	parval->clear();
	for(std::map<Int_t, RunData>::iterator it = fRunData.begin(); it != fRunData.end(); ++it) {
		RunData* rundata = &(it->second);
		
		std::string par0;
		int indx;
		parse_param(param, par0, indx);
		TDataMember* member = TClass::GetClass("dragon::BeamNorm::RunData")->GetDataMember(par0.c_str());
		if(!member || param == "time") {
			std::cerr << "Invalid parameter: \"" << param << "\".\n";
			break;
		}

		Long_t offset = member->GetOffset();
		offset += indx*sizeof(UDouble_t);
		offset += (Long_t)rundata;
		UDouble_t* pdata = (UDouble_t*)offset;

		runnum->push_back(it->first);
		parval->push_back(*pdata);
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

	TGraphAsymmErrors* gr = 0;
	std::vector<Double_t>  runnum;
	std::vector<UDouble_t> parval;
	GetParams(param, &runnum, &parval);
	
	if(runnum.size()) {
		gr = PlotUncertainties(runnum.size(), &runnum[0], &parval[0]);
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
	// std::vector<std::string> par;
	// if(param1)  par.push_back(param1);
	// if(param2)  par.push_back(param2);
	// if(param3)  par.push_back(param3);
	// if(param4)  par.push_back(param4);
	// if(param5)  par.push_back(param5);
	// if(param6)  par.push_back(param6);
	// if(param7)  par.push_back(param7);
	// if(param8)  par.push_back(param8);
	// if(param9)  par.push_back(param9);
	// if(param10) par.push_back(param10);
	// if(param11) par.push_back(param11);
	// if(param12) par.push_back(param12);

	// std::vector<Double_t> runnum;
	// std::vector<std::vector<UDouble_t> > parval;
	// for(size_t i=0; i< par.size(); ++i) {
	// 	std::vector<Double_t> runnum0, parval0;
	// 	GetParams(par[i].c_str(), &runnum0, &parval0);
	// 	if(runnum0.size()) {
	// 		if(runnum.empty()) runnum = runnum0;
	// 		parval.push_back(parval0);
	// 	}
	// }

	// for(size_t i=0; i< runnum.size(); ++i) {
	// 	std::cout << runnum.at(i);
	// 	for(size_t j=0; j< parval.size(); ++j) {
	// 		std::cout << "\t" << parval.at(j).at(i);
	// 	}
	// 	std::cout << "\n";
	// }
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

	// Copy aliases from chain
	TChain* chain = (TChain*)gROOT->GetListOfSpecials()->FindObject(tree);
	if(chain && chain->InheritsFrom(TChain::Class())) {
		for(Int_t i=0; i< chain->GetListOfAliases()->GetEntries(); ++i) {
			TObject* alias = chain->GetListOfAliases()->At(i);
			t->SetAlias(alias->GetName(), alias->GetTitle());
		}
	}


	Bool_t haveRunnum = kFALSE;
	Int_t runnum = 0;
	midas::Database* db = static_cast<midas::Database*>(datafile->Get("odbstop"));
	if(db) haveRunnum = db->ReadValue("/Runinfo/Run number", runnum);
	if(!db || !haveRunnum) {		 
		std::cerr << "Error: couldn't read run number from TFile at " << datafile << "\n";
		return;
	}

	UDouble_t nrecoil (t->GetPlayer()->GetEntries(gate));

	RunData* rundata = GetRunData(runnum);
	if(!rundata) {
		RunData ddd;
		fRunData.insert(std::make_pair(runnum, ddd));
		rundata = GetRunData(runnum);
	}
	assert(rundata);
	
	rundata->nrecoil = nrecoil;
	for(Int_t i=0; i< NSB; ++i) {
		UDouble_t eff = CalculateEfficiency(kFALSE);
		if(rundata->nbeam[i].GetNominal() != 0 && eff.GetNominal() != 0) {
			rundata->yield[i] =
				nrecoil / rundata->nbeam[i] / eff / rundata->live_time_full / rundata->trans_corr;
		}
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


void dragon::BeamNorm::CorrectTransmission(Int_t reference)
{
	RunData* refData = GetRunData(reference);
	if(!refData) {
		std::cerr << "Coundn't find run data for reference run " << reference << "\n";
		return;
	}
	UDouble_t transRef = refData->fc1 / UDouble_t::Mean(3, refData->fc4);

	for(std::map<Int_t, RunData>::iterator it = fRunData.begin(); it != fRunData.end(); ++it) {
		RunData& thisData = it->second;
		UDouble_t transThis = thisData.fc1 / UDouble_t::Mean(3, thisData.fc4);
		if(it->first != reference)
			thisData.trans_corr = (transThis / transRef);
		else
			thisData.trans_corr = UDouble_t(1, 0);

		for(int i=0; i< NSB; ++i) {
			thisData.yield[i] /= thisData.trans_corr;
			thisData.nbeam[i] /= thisData.trans_corr;
		}
	}
}

UDouble_t dragon::BeamNorm::CalculateEfficiency(Bool_t print)
{
	UDouble_t eff (1, 0);
	for(std::map<std::string, UDouble_t>::iterator it = fEfficiencies.begin(); it != fEfficiencies.end(); ++it) {
		eff *= it->second;
	}	
	if(print) {
		std::cout << eff*100. << " %\n";
	}
	return eff;
}

UDouble_t dragon::BeamNorm::CalculateYield(Int_t whichSb, Bool_t print)
{
	if(whichSb < 0 || whichSb >= NSB) {
		std::cerr << "Invalid sb index << " << whichSb << ", valid options are 0 -> " << NSB-1 << "\n";
		return UDouble_t(0,0);
	}

	UDouble_t beam(0,0), recoil(0,0), recoilCounted(0,0), recoilTrans(0,0);
	std::vector<Double_t> recoilV, liveV;
	for(std::map<Int_t, RunData>::iterator it = fRunData.begin(); it != fRunData.end(); ++it) {
		RunData& thisData = it->second;
		beam += thisData.nbeam[whichSb];
		recoil += (thisData.nrecoil / thisData.trans_corr / thisData.live_time_full);
		recoilCounted += thisData.nrecoil;
		recoilTrans   += thisData.nrecoil / thisData.trans_corr;

		// for recoil-weighted livetime
		recoilV.push_back(thisData.nrecoil.GetNominal());
		liveV.push_back(thisData.live_time_full.GetNominal());
	}

	UDouble_t eff = CalculateEfficiency(kFALSE);
	UDouble_t out = recoil / beam / eff;

	Double_t liveAvg = TMath::Mean(liveV.size(), &liveV[0], &recoilV[0]);

	if(print) {
		std::cout << "Beam:            \t" << beam   << "\n"
							<< "Recoil:          \t" << recoil << "\n"
							<< "Recoil (counted):\t" << recoilCounted << "\n"
							<< "Recoil (trans corr):\t" << recoilTrans << "\n"
							<< "Avg. Livetime (nrecoil weighted):\t" << liveAvg << "\n"
							<< "Efficiency:      \t" << eff    << "\n"
							<< "Yield:           \t" << out    << "\n";
	}

	return out;
}



// ================ Class LiveTimeCalculator ================ //

dragon::LiveTimeCalculator::LiveTimeCalculator():
	fFile(0)
{
	std::fill_n(fRuntime,  2, 0.);
	std::fill_n(fBusytime, 2, 0.);
	std::fill_n(fLivetime, 3, 1.);
}

dragon::LiveTimeCalculator::LiveTimeCalculator(TFile* file): 
	fFile(file)
{
	Calculate();
}

Double_t dragon::LiveTimeCalculator::GetBusytime(const char* which) const
{
	int indx = -1;
	if     (!strcmp(which, "head")) indx = 0;
	else if(!strcmp(which, "tail")) indx = 1;
	if(indx >= 0) return fBusytime[indx];
	std::cerr << "Error: invalid specification \"" << which
						<< "\", please specify \"head\" or \"tail\"\n";
	return 0;
}

Double_t dragon::LiveTimeCalculator::GetRuntime(const char*  which) const
{
	int indx = -1;
	if     (!strcmp(which, "head")) indx = 0;
	else if(!strcmp(which, "tail")) indx = 1;
	if(indx >= 0) return fRuntime[indx];
	std::cerr << "Error: invalid specification \"" << which
						<< "\", please specify \"head\" or \"tail\"\n";
	return 0;
}

Double_t dragon::LiveTimeCalculator::GetLivetime(const char* which) const
{
	int indx = -1;
	if     (!strcmp(which, "head"))  indx = 0;
	else if(!strcmp(which, "tail"))  indx = 1;
	else if(!strcmp(which, "coinc")) indx = 2;
	if(indx >= 0) return fLivetime[indx];
	std::cerr << "Error: invalid specification \"" << which
						<< "\", please specify \"head\", \"tail\", or \"coinc\"\n";
	return 0;
}

Bool_t dragon::LiveTimeCalculator::CheckFile()
{
	if(!fFile || fFile->IsZombie()) {
		std::cerr << "Error: Invalid or no file loaded.\n";
		return kFALSE;
	}
	Bool_t okay = kTRUE;
	if(okay) okay = fFile->Get("t1") && fFile->Get("t1")->InheritsFrom(TTree::Class());
	if(okay) okay = fFile->Get("t3") && fFile->Get("t1")->InheritsFrom(TTree::Class());
	if(okay) okay = fFile->Get("t4") && fFile->Get("t1")->InheritsFrom(TTree::Class());
	if(!okay) {
		std::cerr << "Error: missing necessary trees in loaded file\n";
		return kFALSE;
	}

	if(okay) okay = ((TTree*)fFile->Get("t1"))->GetLeaf("io32.busy_time");
	if(okay) okay = ((TTree*)fFile->Get("t3"))->GetLeaf("io32.busy_time");
	if(!okay) {
		std::cerr << "Error: missing leaf \"io32.busy_time\" in either \"t1\" or \"t3\"\n";
		return kFALSE;
	}

	okay = fFile->Get("odbstop") && fFile->Get("odbstop")->InheritsFrom(midas::Database::Class());
	if(!okay) {
		std::cerr << "Error: Loaded file is missing database \"odbstop\"\n";
		return kFALSE;
	}

	return kTRUE;
}

void dragon::LiveTimeCalculator::DoCalculate(Double_t tbegin, Double_t tend)
{
	Bool_t isFull = (tbegin < 0 || tend < 0);
	if (isFull) {
		if(tbegin > 0 || tend > 0) {
			std::cerr << "Error: invalid parameters to LiveTimeCalculator::DoCalculate(): "
								<< "tbegin = " << tbegin <<  ", tend = " << tend << "\n";
			return;
		}
	}

	TTree *t1 = 0, *t3 = 0, *t5 = 0;
	t1 = (TTree*)fFile->Get("t1");
	t3 = (TTree*)fFile->Get("t3");
	t5 = (TTree*)fFile->Get("t5");
	
	TTree* trees[3] = { t1, t3, t5 };

	Int_t time0, time1, tclock;
	midas::Database* db = (midas::Database*)fFile->Get("odbstop");
	db->ReadValue("/Runinfo/Start time binary", time0);
	db->ReadValue("/Runinfo/Stop time binary",  time1);
	tclock = time1 - time0;
	
	double rolltime = 0xffffffff / 20e6; // 32-bit clock rollover
	int nroll = tclock / rolltime;
	double trigStart[2], trigStop[2];
	db->ReadArray("/Experiment/Run Parameters/TSC_TriggerStart", trigStart, 2);
	db->ReadArray("/Experiment/Run Parameters/TSC_TriggerStop",  trigStop,  2);

	for(int i=0; i< 2; ++i) {
		Long_t n = 0;

		std::stringstream cut;
		cut.precision(20);

		if(isFull)
			cut << "";
		else {
			ULong64_t rollCorrect = 0;
			n = trees[i]->Draw("io32.tsc4.trig_time", "", "goff", 1);
			if(n>0) {
				if( trees[i]->GetV1()[0] >= ((ULong64_t)1<<36)/20. ) {
					rollCorrect = ((ULong64_t)1 << 36)/20.;
				}
			}

			cut << "    (io32.tsc4.trig_time - " << rollCorrect << ") - " << trigStart[i] << " > " << tbegin*1e6
					<< " && (io32.tsc4.trig_time - " << rollCorrect << ") - " << trigStart[i] << " < " << tend*1e6;
		}

		// std::cout << cut.str() << "\n";

		n = trees[i]->Draw("io32.busy_time", cut.str().c_str(), "goff");
		fBusytime[i] = TMath::Mean(n, trees[i]->GetV1()) * n / 20e6;

		double stoptime = trigStop[i] + nroll*rolltime;
		fRuntime[i] = isFull ? stoptime - trigStart[i] : tend - tbegin;

		fLivetime[i] = (fRuntime[i] - fBusytime[i]) / fRuntime[i];
	}
}

void dragon::LiveTimeCalculator::Calculate()
{
	if(!CheckFile()) return;
	DoCalculate(-1, -1);
}


void dragon::LiveTimeCalculator::CalculateSub(Double_t tbegin, Double_t tend)
{
	if(!CheckFile()) return;
	DoCalculate(tbegin, tend);
}

void dragon::LiveTimeCalculator::CalculateChain(TChain* chain)
{
	TFile* file0 = GetFile();
	Double_t sumbusy[2] = {0,0}, sumrun[2] = {0,0};

	for (Int_t i=0; i< chain->GetListOfFiles()->GetEntries(); ++i) {
		TFile* f = TFile::Open ( chain->GetListOfFiles()->At(i)->GetTitle() );
		if(f->IsZombie()) { f->Close(); continue; }

		SetFile(f);
		Calculate();
		sumrun[0] += GetRuntime("head");
		sumrun[1] += GetRuntime("tail");
		sumbusy[0] += GetBusytime("head");
		sumbusy[1] += GetBusytime("tail");

		f->Close();
	}

	for(int i=0; i< 2; ++i) {
		fBusytime[i] = sumbusy[i];
		fRuntime[i]  = sumrun[i];
		fLivetime[i] = (sumrun[i] - sumbusy[i]) / sumrun[i];
	}
}

// ================ class dragon::StoppingPowerCalculator ================ //

Double_t dragon::StoppingPowerCalculator::TorrCgs(Double_t torr)
{
	///
	/// \param torr Pressure in `torr`
	/// \returns Pressure in `dyn/cm^2`

	return 10 * torr * 101325 / 760;
}

UDouble_t dragon::StoppingPowerCalculator::TorrCgs(UDouble_t torr)
{
	///
	/// \param torr Pressure in `torr` (with uncertainty)
	/// \returns Pressure in `dyn/cm^2` (with uncertainty)

	return 10. * torr * 101325. / 760.;
}

Double_t dragon::StoppingPowerCalculator::CalculateDensity(Double_t pressure, Double_t length,
																													 Int_t nmol, Double_t temp)
{
	///
	/// \param pressure Gas pressure in torr
	/// \param length Effective target length in cm
	/// \param nmol Number of atoms per molecule in the gas
	/// \param temp Temperature in kelvin
	/// \returns Density in atoms / cm^2

	return nmol * TorrCgs(pressure) * length / TMath::Kcgs() / temp;
}

UDouble_t dragon::StoppingPowerCalculator::CalculateDensity(UDouble_t pressure, UDouble_t length,
																														Int_t nmol, Double_t temp)
{
	///
	/// \param pressure Gas pressure in torr (with uncertainty)
	/// \param length Effective target length in cm (with uncertainty)
	/// \param nmol Number of atoms per molecule in the gas
	/// \param temp Temperature in kelvin
	/// \returns Density in atoms / cm^2 (with uncertainty)

	return nmol * TorrCgs(pressure) * length / TMath::Kcgs() / temp;
}

UDouble_t dragon::StoppingPowerCalculator::CalculateEnergy(Double_t md1, Double_t md1Err, Int_t q, Double_t m,
																													 Double_t cmag, Double_t cmagErr)
{
	///
	/// Equation used is from Dave H's NIM paper on BGO z-position (includes relativity):
	/// `E/m = cmag * (qB/m)^2 - [1/(2*u*c^2)] * (E/m)^2`
	///
	/// \param md1 MD1 field in Gauss
	/// \param md1Err Absolute error on MD1 field
	/// \param q Beam charge state
	/// \param m Beam mass in AMU
	/// \param cmag "Magnetic constant" for MD1 in (keV/u)*G^2
	/// \param cmagErr Absolute error on _cmag_
	///
	/// \returns Beam energy in keV/u

	UDouble_t ucmag (cmag, cmagErr);

	Double_t  a = 1/(2*dragon::Constants::AMU());
	Double_t  b = 1.;
	UDouble_t c = -1 * ucmag * UDouble_t::pow((Double_t)q * UDouble_t(md1, md1Err) / m, 2);

	return (-b + UDouble_t::sqrt(b*b - 4*a*c)) / (2*a);
}

dragon::StoppingPowerCalculator::StoppingPowerCalculator(Int_t beamCharge, Double_t beamMass, Int_t nmol,
																												 Double_t targetLen, Double_t targetLenErr,
																												 Double_t cmd1, Double_t cmd1Err,
																												 Double_t temp):
	fBeamMass(beamMass),
	fBeamCharge(beamCharge),
	fNmol(nmol),
	fTargetLength(targetLen, targetLenErr),
	fTemp(temp),
	fMd1Constant(cmd1, cmd1Err)
{
	///
	/// \param beamCharge Beam charge state
	/// \param beamMass Beam mass in AMU
	/// \param nmol Number of atoms per molecule in the target
	/// \param targetLen Effective gas target length in cm
	/// \param targetLenErr Absolute error on targetLen
	/// \param cmd1 "Magnetic constant" for MD1 in (keV/u)*G^2
	/// \param cmd1Err Absolute error on _cmd1_
	/// \param temp Ambient temperature in kelvin
	;
}

void dragon::StoppingPowerCalculator::AddMeasurement(Double_t pressure, Double_t pressureErr,
																										 Double_t md1, Double_t md1Err)
{
	/// \param pressure Measured pressure in torr
	/// \param pressureErr Error (absolute) on the pressure measurement
	/// \param md1 Measured MD1 field
	/// \param md1Err Error (absolute) on measured MD1 field

	UDouble_t upressure (pressure, pressureErr);
	UDouble_t energy = CalculateEnergy(md1, md1Err, fBeamCharge, fBeamMass,
																		 fMd1Constant.GetNominal(), fMd1Constant.GetErrLow());

	UDouble_t density = CalculateDensity(upressure, fTargetLength, fNmol, fTemp);

	fEnergies.push_back(energy);
	fDensities.push_back(density);
	fPressures.push_back(upressure);

	UDouble_t umd1(md1, md1Err);
	fMd1.push_back(umd1);
}

dragon::StoppingPowerCalculator::Measurement_t
dragon::StoppingPowerCalculator::GetMeasurement(Int_t index) const
{
	/// \param index Which measurement to return

	Measurement_t retval = {0, 0, 0, 0};
	size_t index0 = index;
	if(index0 < fEnergies.size()) {
		retval.pressure = fPressures.at(index0);
		retval.energy = fEnergies.at(index0);
		retval.density = fDensities.at(index0);
		retval.md1 = fMd1.at(index0);
	} else {
		std::cerr << "Error invalid index " << index
							<< ", valid options are [0, ) " << fEnergies.size() << "\n";
	}
	
	return retval;
}

void dragon::StoppingPowerCalculator::RemoveMeasurement(Int_t index)
{
	/// \param index Which measurement to remove

	size_t index0 = index;
	if(index0 < fEnergies.size()) {
		fPressures.erase(fPressures.begin() + index0);
		fEnergies.erase (fPressures.begin() + index0);
	} else {
		std::cerr << "Error invalid index " << index
							<< ", valid options are [0, ) " << fEnergies.size() << "\n";
	}
}

TGraph* dragon::StoppingPowerCalculator::PlotMeasurements(XAxisType_t xaxis, YAxisType_t yaxis, Bool_t draw) const
{
	///
	/// \param xaxis Specify the x axis. Valid options are dragon::StoppingPowerCalculator::kPRESSURE (== 0)
	///  to plot pressure (torr), or dragon::StoppingPowerCalculator::kDENSITY (== 1) to plot
	///  density in atoms/cm^2.
	/// \param xaxis Specify the y axis. Valid options are dragon::StoppingPowerCalculator::kMD1 (== 0)
	///  to plot MD1 field (gauss) or dragon::StoppingPowerCalculator::kENERGY (== 1) to plot energy
	///  in keV/u.
	/// \param draw Argument `true` plots the graph with option `"AP"` in addition to
	///  returning it.
	/// \returns TGraph of pressure vs. energy, with asymmetric error bars on
	///  both axes. Responsibility is on the user to delete the returned graph.

	TGraph* out = 0;
	if(!fEnergies.empty()) {
		
		const UDouble_t* x = xaxis == kDENSITY ? &fDensities[0] : &fPressures[0];
		const UDouble_t* y = yaxis == kMD1     ? &fMd1[0]       : &fEnergies[0];

		out = PlotUncertainties(fEnergies.size(), x, y);
	}
	if(out) {
		out->SetMarkerStyle(21);
		if(draw) out->Draw("AP");
	}
	return out;
}

UDouble_t dragon::StoppingPowerCalculator::CalculateEbeam(TGraph** plot)
{
	///
	/// \param [out] plot Address of a pointer to a TGraph that will contain
	///  a plot of energy vs. pressure upon successful return. Passing a non-NULL
	///  plot argument also causes the plot to be drawn with option "AP". The default
	///  argument (NULL) bypasses any plotting and simply calculates the epsilon parameter.
	/// \returns The beam energy at zero pressure from a linear fit of E vs. P.

	UDouble_t out(0,0);
	std::auto_ptr<TGraph> g(new TGraphAsymmErrors(GetNmeasurements()));
	
	//
	// calculate density, energy w/ random errors only
	std::vector<UDouble_t> pres(GetNmeasurements()), energy(GetNmeasurements());
	for(Int_t i=0; i< GetNmeasurements(); ++i) {

		double md1err = fMd1[i].GetErrLow() < fMd1[i].GetErrHigh() ?
			fMd1[i].GetErrHigh() : fMd1[i].GetErrLow();

		pres[i] = fPressures[i];
		energy[i] = CalculateEnergy(fMd1[i].GetNominal(), md1err, fBeamCharge, fBeamMass, fMd1Constant.GetNominal(), 0);
	}
	//
	// plot and fit to get dE/d[n/A]
	g.reset(PlotUncertainties(GetNmeasurements(), &pres[0], &energy[0]));
	dragon::LinearFitter fit;
	fit.Fit(g.get());
	out = fit.GetOffset();

	//
  // plot if requested
	if(plot) {
		*plot = (TGraph*)g->Clone();
		(*plot)->SetMarkerStyle(21);
		(*plot)->SetTitle(Form("Beam energy: %.2f +/- %f keV/u;Presure [T];E [keV/u]", out.GetNominal(), out.GetErrLow()));
		(*plot)->Draw("AP");
		if(fit.GetFunction()) {
			TF1* ffit2 = (TF1*)fit.GetFunction()->Clone();
			ffit2->Draw("SAME");
		}
		else std::cerr << "No function!\n";
	}

	return out;
	
}

UDouble_t dragon::StoppingPowerCalculator::CalculateEpsilon(TGraph** plot)
{
	///
	/// \param [out] plot Address of a pointer to a TGraph that will contain
	///  a plot of energy vs. density upon successful return. Passing a non-NULL
	///  plot argument also causes the plot to be drawn with option "AP". The default
	///  argument (NULL) bypasses any plotting and simply calculates the epsilon parameter.
	/// \returns The `epslion` parameter used in calculations of omega-gamma, in units of
	///  eV / [atoms / cm^2]
	///
	/// \note For treatment of errors, we should only include random errors in the plot to
	///  be fit. Systematic errors (i.e target pressure and c_md1) will pull all points in
	///  the same direction and should only be added back after the linear fit is done.

	UDouble_t out(0,0);
	std::auto_ptr<TGraph> g(new TGraphAsymmErrors(GetNmeasurements()));
	
	//
	// calculate density, energy w/ random errors only
	std::vector<UDouble_t> dens(GetNmeasurements()), energy(GetNmeasurements());
	for(Int_t i=0; i< GetNmeasurements(); ++i) {

		double md1err = fMd1[i].GetErrLow() < fMd1[i].GetErrHigh() ?
			fMd1[i].GetErrHigh() : fMd1[i].GetErrLow();

		dens[i] = CalculateDensity(fPressures[i], UDouble_t(fTargetLength.GetNominal(), 0), fNmol, fTemp);
		energy[i] = CalculateEnergy(fMd1[i].GetNominal(), md1err, fBeamCharge, fBeamMass, fMd1Constant.GetNominal(), 0);
	}
	//
	// plot and fit to get dE/d[n/A]
	g.reset(PlotUncertainties(GetNmeasurements(), &dens[0], &energy[0]));
	dragon::LinearFitter fit;
	fit.Fit(g.get());
	out = -1*fit.GetSlope();
	out *= fBeamMass; // keV/u -> keV
	out *= 1e3; // keV -> eV
	//
	// add back systematic errors
	out *= fMd1Constant / fMd1Constant.GetNominal();
	out *= fTargetLength / fTargetLength.GetNominal();
	//
  // plot if requested
	if(plot) {
		*plot = (TGraph*)g->Clone();
		(*plot)->SetMarkerStyle(21);
		char buf[256];
		if(out.GetErrHigh() != out.GetErrLow()) {
			sprintf(buf, "Stopping power: %g ^{+%g}_{-%g} eV cm^{2} / atom;Density [atoms/cm^{2}];Beam energy [keV/u]",
							out.GetNominal(), out.GetErrLow(), out.GetErrHigh());
		}
		else {
			sprintf(buf, "Stopping power: %g +/-%g eV cm^{2} / atom;Density [atoms/cm^{2}];Beam energy [keV/u]",
							out.GetNominal(), out.GetErrLow());
		}
		(*plot)->SetTitle(buf);
		(*plot)->Draw("AP");
		if(fit.GetFunction()) {
			TF1* ffit2 = (TF1*)fit.GetFunction()->Clone();
			ffit2->Draw("SAME");
		}
		else std::cerr << "No function!\n";
	}

	return out;
}



// ================ Class dragon::ResonanceStrengthCalculator ================ //


dragon::ResonanceStrengthCalculator::ResonanceStrengthCalculator(Double_t eres, Double_t mbeam, Double_t mtarget,
																																 dragon::BeamNorm* beamNorm, UDouble_t epsilon):
	fBeamNorm(beamNorm), fEpsilon(epsilon), fBeamMass(mbeam), fTargetMass(mtarget), fResonanceEnergy(eres)
{
	///
	/// \param eres Resonance energy in keV (center of mass)
	/// \param mbeam Beam mass in amu
	/// \param mtarget Target mass in amu
	/// \param beamNorm Pointer to a constructed dragon::BeamNorm object (takes no ownership)
	/// \param epsilon "Stopping power" in e*cm^2/atom (see dragon::StoppingPowerCalculator)
}

UDouble_t dragon::ResonanceStrengthCalculator::CalculateResonanceStrength(Int_t whichSb, Bool_t print)
{
	/// \param whichSb Specify the surface barrier detector to use
	///  for normalization
	/// \returns Resonance strenght in eV.
	
	if(!fBeamNorm) return UDouble_t(0,0);
	UDouble_t yield = fBeamNorm->CalculateYield(whichSb, print);
	UDouble_t wavelength = CalculateWavelength(fResonanceEnergy, fBeamMass, fTargetMass);
	UDouble_t wg = CalculateResonanceStrength(yield, fEpsilon, wavelength, fBeamMass, fTargetMass);
	if(print) std::cout << "Resonance Strength [eV]: " << wg << "\n";
	return wg;
}

TGraph* dragon::ResonanceStrengthCalculator::PlotResonanceStrength(Int_t whichSb)
{
	if((unsigned)whichSb > 1) {
		std::cerr << "Error: invalid SB: " << whichSb << ", valid are 0 or 1\n";
		return 0;
	}

	std::vector<Int_t>& runs = fBeamNorm->GetRuns();
	int nruns = runs.size();

	std::vector<Double_t> runnum(nruns);
	for(int i=0; i< nruns; ++i)
		runnum[i] = runs[i];

	UDouble_t wavelength = CalculateWavelength(fResonanceEnergy, fBeamMass, fTargetMass);
	std::vector<UDouble_t> strengths(nruns);
	for(int i=0; i< nruns; ++i) {
		UDouble_t yield = fBeamNorm->GetRunData(runs[i])->yield[whichSb];
		UDouble_t wg = CalculateResonanceStrength(yield, fEpsilon, wavelength, fBeamMass, fTargetMass);
		strengths[i] = wg;
	}

	TGraph* out = PlotUncertainties(nruns, &runnum[0], &strengths[0]);
	out->SetTitle(";Run number;#omega#gamma [eV]");
	out->SetMarkerStyle(21);
	out->Draw("AP");
	return out;
}


UDouble_t dragon::ResonanceStrengthCalculator::CalculateWavelength(UDouble_t eres, Double_t mbeam, Double_t mtarget)
{
	/// \param eres Resonance energy in center-of-mass [keV].
	/// \param mbeam Mass of the beam in AMU
	/// \param mtarget Mass of the target in AMU
	/// \returns DeBroglie wavelength in cm.

	Double_t mu = 1000*dragon::Constants::AMU()*mbeam*mtarget / (mbeam + mtarget); // reduced mass in eV/c^2
	eres *= 1e3; // keV -> eV
	UDouble_t pc = UDouble_t::sqrt(eres*eres + 2.*eres*mu);

	Double_t hc = TMath::HC() * 1e9 / TMath::Qe(); // eV*nm
	UDouble_t lambda = hc / pc; // nm
	lambda /= 1e7; // cm
	return lambda;
}

UDouble_t dragon::ResonanceStrengthCalculator::CalculateResonanceStrength(UDouble_t yield, UDouble_t epsilon,
																																					UDouble_t wavelength,
																																					Double_t mbeam, Double_t mtarget)
{
	UDouble_t wg = 2.*epsilon*yield / (wavelength*wavelength);
	wg *= (mtarget / (mbeam+mtarget));
	return wg;
}


// ================ class dragon::LabCM ================ //

dragon::LabCM::LabCM(int Zbeam, int Abeam, int Ztarget, int Atarget)
{
	/// Sets beam and target masses from AME12 compilation.
	/// Energies are initially unset, use SetE*() functions to specify an
	/// energy.
	/// \param Zbeam Beam charge
	/// \param Abeam Beam mass number
	/// \param Ztarget Target charge
	/// \param Atarget Target mass number
	/// \attention Uses "nuclear" (fully ionized) masses.
	
	Init(Zbeam, Abeam, Ztarget, Atarget, 0.);
}


dragon::LabCM::LabCM(int Zbeam, int Abeam, int Ztarget, int Atarget, double ecm)
{
	/// Sets beam and target masses from AME12 compilation.
	/// Energies are initially unset, use SetE*() functions to specify an
	/// energy.
	/// \param Zbeam Beam charge
	/// \param Abeam Beam mass number
	/// \param Ztarget Target charge
	/// \param Atarget Target mass number
	/// \param ecm Center-of-mass kinetic energy in keV
	/// \attention Uses "nuclear" (fully ionized) masses.
	
	Init(Zbeam, Abeam, Ztarget, Atarget, ecm);
}

dragon::LabCM::LabCM(double mbeam, double mtarget, double ecm)
{
	/// Uses specified masses directly
	/// \param mbeam Beam mass in AMU
	/// \param mtarget Taret mass in AMU
	/// \param ecm Center of mass energy in keV
	SetM1(mbeam);
	SetM2(mtarget);
	fTcm = ecm;
}

void dragon::LabCM::Init(int Zbeam, int Abeam, int Ztarget, int Atarget, double ecm)
{
	TAtomicMassTable mt; // AME12
	fM1 = mt.NuclearMass(Zbeam, Abeam);
	fM2 = mt.NuclearMass(Ztarget, Atarget);
	fTcm = ecm;
}

void dragon::LabCM::SetEcm(double ecm)
{
	/// \param ecm Center-of-mass kinetic energy in keV
	fTcm = ecm;
}

void dragon::LabCM::SetEbeam(double ebeam)
{
	/// \param ebeam Beam energy in keV
	
	double E1tot = ebeam + fM1; // total energy
	double Ecmtot = sqrt(fM1*fM1 + fM2*fM2 + 2*fM2*E1tot);
	fTcm = Ecmtot - fM1 - fM2;
}

void dragon::LabCM::SetV2beam(double ebeam)
{
	/// \param ebeam Beam energy in keV/u
	
	SetEbeam(ebeam*fM1/dragon::Constants::AMU()); // keV/u -> keV
}

void dragon::LabCM::SetEtarget(double etarget)
{
	/// \param etarget Target energy in keV
	
	double E2tot = etarget + fM2; // total energy
	double Ecmtot = sqrt(fM1*fM1 + fM2*fM2 + 2*fM1*E2tot);
	fTcm = Ecmtot - fM1 - fM2;
}

void dragon::LabCM::SetV2target(double etarget)
{
	/// \param etarget Target energy in keV/u
	
	SetEbeam(etarget*fM2/dragon::Constants::AMU()); // keV/u -> keV
}

double dragon::LabCM::GetV2beam() const
{
	return GetEbeam() / (fM1/dragon::Constants::AMU());
}

double dragon::LabCM::GetEbeam() const
{
	double Ecm = fTcm + fM1 + fM2; // total energy
	double E1 = (Ecm*Ecm - fM1*fM1 - fM2*fM2) / (2*fM2); // total energy
	double T1 = E1 - fM1; // kinetic energy
	return T1;
}

double dragon::LabCM::GetV2target() const
{
	return GetEtarget() / (fM2/dragon::Constants::AMU());
}

double dragon::LabCM::GetEtarget() const
{
	double Ecm = fTcm + fM1 + fM2; // total energy
	double E2 = (Ecm*Ecm - fM1*fM1 - fM2*fM2) / (2*fM1); // total energy
	double T2 = E2 - fM2; // kinetic energy
	return T2;
}
