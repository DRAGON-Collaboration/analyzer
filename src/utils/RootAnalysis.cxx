///
/// \file RootAnalysis.cxx
/// \author G. Christian
/// \brief Implements RootAnalysis.hxx
///
#include <vector>
#include <TThread.h>
#include "ErrorDragon.hxx"
#include "RootAnalysis.hxx"


template <class T> void Zap(T*& t) 
{
	if(!t) return;
	delete t;
	t = 0;
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

