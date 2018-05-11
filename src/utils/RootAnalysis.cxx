///
/// \file RootAnalysis.cxx
/// \author G. Christian
/// \brief Implements RootAnalysis.hxx
///
#include <vector>
#include <string>
#include <cassert>
#include <sstream>
#include <numeric>
#include <algorithm>

#include <TMath.h>
#include <TClass.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TString.h>
#include <TSystem.h>
#include <TThread.h>
#include <TRegexp.h>
#include <TFitResult.h>
#include <TDataMember.h>
#include <TTreeFormula.h>

#include "midas/Database.hxx"
#include "utils/Functions.hxx"
#include "Dragon.hxx"
#include "Constants.hxx"
#include "ErrorDragon.hxx"
#include "LinearFitter.hxx"
#include "TAtomicMass.h"
#include "RootAnalysis.hxx"

namespace dutils = dragon::utils;

namespace {
  //
  // Alias for surface barrier max channels
  const Int_t NSB = dragon::SurfaceBarrier::MAX_CHANNELS;

  //
  // Get a branch from a tree and print an error message if input is bad
  template <typename T>
  TBranch* get_branch(TTree* t, T& valref, const char* name, const char* funcname = "") {
	TBranch* branch;
	Int_t code = t->SetBranchAddress(name, &valref, &branch);
	if(code < 0) {
      dutils::Error(funcname, __FILE__, __LINE__)
        << "Couldn't set Branch \"" << name << "\" in TTree \"" << t->GetName() << "\"";
      return 0;
	}
	return branch;
  }

  //
  // RAII class to change the MakeClass() option of a tree and reset it when done
  class SetMakeClass_t {
	TTree* fTree;
	Int_t fMakeClass;
  public:
	SetMakeClass_t(TTree* tree, Int_t make): fTree(tree)
    {
      if(fTree) {
        fMakeClass = fTree->GetMakeClass();
        fTree->SetMakeClass(make);
      }
    }
	~SetMakeClass_t() { if(fTree) fTree->SetMakeClass(fMakeClass); }
  };

  //
  // Class to automatially reset TTree branch addresses
  class AutoResetBranchAddresses {
	TObjArray fTrees;
  public:
	AutoResetBranchAddresses(TTree* t) { fTrees.Add(t); }
	AutoResetBranchAddresses(TTree** tarr, Int_t n) {
      for(Int_t i=0; i< n; ++i) fTrees.Add(tarr[i]);
	}
	~AutoResetBranchAddresses() {
      for(Int_t i=0; i< fTrees.GetEntries(); ++i) {
        TTree* tree = static_cast<TTree*>(fTrees.At(i));
        if(tree) tree->ResetBranchAddresses();
      }
	}
  };

  //
  // Delete a pointer and reset to NULL
  template <class T> void Zap(T*& t)
  {
	if(!t) return;
	delete t;
	t = 0;
  }

} // namespace


// ============ namespace dragon Free Functions ============ //

void dragon::MakeChains(const Int_t* runnumbers, Int_t nruns, const char* format, Bool_t sonik)
{
  MakeChains("t", runnumbers, nruns, format, sonik);
}

void dragon::MakeChains(const char* prefix, const Int_t* runnumbers, Int_t nruns, const char* format, Bool_t sonik)
{
  ///
  /// \param [in] runnumbers Pointer to a valid array of desired run numbers to chain together.
  /// \param [in] nruns Length of _runnumbers_.
  /// \param format String specifying the file name format, uses _printf_ syntax.
  ///
  /// \note Does not return anything, but creates `new` heap-allocated TChains that become part of
  /// the present ROOT directory; these must be deleted by the user.
  if ( !(sonik) ){
    TChain* chain[] = {
      new TChain(Form("%s1",  "t"), "Head singles event."),
      new TChain(Form("%s2",  "t"), "Head scaler event."),
      new TChain(Form("%s3",  "t"), "Tail singles event."),
      new TChain(Form("%s4",  "t"), "Tail scaler event."),
      new TChain(Form("%s5",  "t"), "Coincidence event."),
      new TChain(Form("%s20", "t"), "Epics event."),
      new TChain(Form("%s6",  "t"), "Timestamp diagnostics."),
      new TChain(Form("%s7",  "t"), "Glocal run parameters.")
    };
    Int_t nchains = sizeof(chain) / sizeof(TChain*);

    for(Int_t i=0; i< nruns; ++i) {
      char fname[4096];
      sprintf(fname, format, runnumbers[i]);
      {
        TFile file(fname);
        if(file.IsZombie()) {
          dutils::Warning("MakeChains", __FILE__, __LINE__)
            << "Skipping run " << runnumbers[i] << ", couldn't find file " << fname;
        }
      }

      for(int j=0; j< nchains; ++j) {
        chain[j]->AddFile(fname);
      }
    }

    chain[0]->SetName(Form("%s1",  prefix));
    chain[1]->SetName(Form("%s2",  prefix));
    chain[2]->SetName(Form("%s3",  prefix));
    chain[3]->SetName(Form("%s4",  prefix));
    chain[4]->SetName(Form("%s5",  prefix));
    chain[5]->SetName(Form("%s20", prefix));
    chain[6]->SetName(Form("%s6",  prefix));
    chain[7]->SetName(Form("%s7",  prefix));
  }
  else{
    TChain* chain[] = {
      new TChain(Form("%s0",  "t"), "SONIK event."),
      // new TChain(Form("%s1",  "t"), "Head singles event."), Head DAQ not used with SONIK
      // new TChain(Form("%s2",  "t"), "Head scaler event."),
      new TChain(Form("%s3",  "t"), "Tail singles event."),
      new TChain(Form("%s4",  "t"), "Tail scaler event."),
      // new TChain(Form("%s5",  "t"), "Coincidence event."), Coincidence events N/A for SONIK
      new TChain(Form("%s20", "t"), "Epics event."),
      new TChain(Form("%s6",  "t"), "Timestamp diagnostics."),
      new TChain(Form("%s7",  "t"), "Glocal run parameters.")
    };
    Int_t nchains = sizeof(chain) / sizeof(TChain*);
    for(Int_t i=0; i< nruns; ++i) {
      char fname[4096];
      sprintf(fname, format, runnumbers[i]);
      {
        TFile file(fname);
        if(file.IsZombie()) {
          dutils::Warning("MakeChains", __FILE__, __LINE__)
            << "Skipping run " << runnumbers[i] << ", couldn't find file " << fname;
        }
      }

      for(int j=0; j< nchains; ++j) {
        chain[j]->AddFile(fname);
      }
    }

    chain[0]->SetName(Form("%s0",  prefix));
    chain[1]->SetName(Form("%s3",  prefix));
    chain[2]->SetName(Form("%s4",  prefix));
    chain[3]->SetName(Form("%s20", prefix));
    chain[4]->SetName(Form("%s6",  prefix));
    chain[5]->SetName(Form("%s7",  prefix));
  }
}

void dragon::MakeChains(const char* prefix, const std::vector<Int_t>& runnumbers, const char* format, Bool_t sonik)
{
  MakeChains(prefix, &runnumbers[0], runnumbers.size(), format, sonik);
}

void dragon::MakeChains(const std::vector<Int_t>& runnumbers, const char* format, Bool_t sonik)
{
  ///
  /// \param runnumbers vector of desired run numbers
  /// \param format same as the other version
  ///
  MakeChains(&runnumbers[0], runnumbers.size(), format, sonik);
}

void dragon::FriendChain(TChain* chain, const char* friend_name, const char* friend_alias,
                         const char* format, const char* friend_format)
{
  ///
  /// \param chain Initial chain, the one to which we are adding friends.
  /// \param friend_name Name of the TTree inside the friend files that you want to friend with
  ///  _chain_
  /// \param friend_alias Desired alias that you want to give to the friend tree
  /// \param format printf style format for the _original_ file names
  /// \param friend_format printf style format for the _friend_ file names
  ///
  Int_t runnum;
  TChain* friendchain = new TChain(friend_name);

  TString format0(format);
  gSystem->ExpandPathName(format0);
  for(Int_t i=0; i< chain->GetNtrees(); ++i) {
    TString filename0 = chain->GetListOfFiles()->At(i)->GetTitle();
    gSystem->ExpandPathName(filename0);

    sscanf(filename0, format0, &runnum);
    TString filename1 = Form(friend_format, runnum);
    gSystem->ExpandPathName(filename1);
    friendchain->AddFile(filename1);
  }

  chain->AddFriend(friendchain, friend_alias, kTRUE);
}

TFile* dragon::OpenRun(int runnum, const char* format)
{
  TFile* f = TFile::Open(Form(format, runnum));
  if(f && f->IsZombie() == kFALSE)
    gROOT->ProcessLine(Form("TFile* frun%d = (TFile*)%p;", runnum, f));
  else if (f) { f->Close(); f = 0; }
  return f;
}


// ============ class dragon::MetricPrefix ============ //

Double_t dragon::MetricPrefix::Get(const char* prefix)
{
  ///
  /// ::
  ///
  static std::map<std::string, Double_t> m;
  if(m.empty()) {
    m["Y"]   = 1e+24; // yotta
    m["Z"]   = 1e+21; // zetta
    m["E"]   = 1e+18; // exa
    m["P"]   = 1e+15; // peta
    m["T"]   = 1e+12; // tera
    m["G"]   = 1e+9;  // giga
    m["M"]   = 1e+6;  // mega
    m["k"]   = 1e+3;  // kilo
    m["h"]   = 1e+2;  // hecto
    m["da"]  = 1e+1;  // deca

    m[""]    = 1e+0;  // none

    m["y"]   = 1e-24; // yocto
    m["z"]   = 1e-21; // zepto
    m["a"]   = 1e-18; // atto
    m["f"]   = 1e-15; // femto
    m["p"]   = 1e-12; // pico
    m["n"]   = 1e-9;  // nano
    m["mu"]  = 1e-6;  // micro
    m["u"]   = 1e-6;  // micro (alternate)
    m["m"]   = 1e-3;  // milli
    m["c"]   = 1e-2;  // centi
    m["d"]   = 1e-1;  // deci
  }

  std::map<std::string, Double_t>::iterator i = m.find(prefix);
  return i != m.end() ? i->second : 0;
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
    utils::Warning("TTreeFilter::CloseOutDir", __FILE__, __LINE__)
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
    utils::Error("TTreeFilter::Run", __FILE__, __LINE__) << "Zombie output directory.";
    return -1;
  }
  if(fInputs.empty()) {
    utils::Error("TTreeFilter::Run", __FILE__, __LINE__) << "No inputs to filter.";
    return -1;
  }

  std::vector<std::pair<TThread*, ThreadArgs_t*> > threads;
  threads.reserve(fInputs.size());

  for(Map_t::iterator i = fInputs.begin(); i != fInputs.end(); ++i) {
    if(CheckCondition(i->first) == kFALSE) {
      utils::Warning("TTreeFilter::Run", __FILE__, __LINE__)
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
    fFile.reset(new std::ifstream(filename2));
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
    dutils::Error("RossumData::ParseFile", __FILE__, __LINE__)
      << "No rossum data file open.";
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
      AutoResetBranchAddresses Rst_(tree);
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
          runnum = 0;
          size_t curpos = line0.find("Current");
          if(curpos > line0.size()) {
            dutils::Error("RossumData::ParseFile", __FILE__, __LINE__)
              << "couldn't parse cup name from line: " << line0;
            retval = kFALSE;
            continue;
          }
          std::string which = line0.substr(6, curpos-6);
          std::map<std::string, Int_t>::iterator itWhich = fWhichCup.find(which);
          if(itWhich == fWhichCup.end()) {
            dutils::Error("RossumData::ParseFile", __FILE__, __LINE__)
              << "couldn't parse cup name from line: " << line0;
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
                {
                  dutils::Error err("RossumData::ParseFile", __FILE__, __LINE__);
                  err << "invalid current read line: " << line;
                  if(tok.get())
                    err << " : ntokens == " << tok->GetEntries() << "\n";
                  else
                    err << " : tok.get() == 0\n";
                }
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
      sstitle << "Farady cup readings preceding run " << runnum;
      tree->SetNameTitle(ssname.str().c_str(), sstitle.str().c_str());

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
  /// \returns Average current value for the parameters specified. This is returned as a class
  /// that holds both the nominal average value and the 1-sigma error. The 1-sigma error is
  /// calculated as the standard deviation of each individual cup reading over the specified
  /// diration. Note that to see the mean & error as part of a ROOT session, you can do
  /// \code
  /// rossumData->AverageCurrent(1000, 0, 0)->Print();
  /// \endcode
  ///
  TTree* tree = GetTree(run);
  if(!tree) {
    utils::Error("RossumData::AverageCurrent", __FILE__, __LINE__)
      << "Invalid run " << run;
    return UDouble_t(0);
  }

  Double_t time_, current_;
  Int_t cup_, iteration_, success;
  success = tree->SetBranchAddress("current", &current_);
  success = tree->SetBranchAddress("time", &time_);
  success = tree->SetBranchAddress("cup", &cup_);
  success = tree->SetBranchAddress("iteration", &iteration_);
  if(success) {
    utils::Error("RossumData::AverageCurrent", __FILE__, __LINE__)
      << "Failure to set branch addresses";
    return UDouble_t(0);
  }
  AutoResetBranchAddresses Rst_(tree);

  Double_t t0 = -1;
  std::vector<Double_t> current, time;
  for(Long64_t entry = 0; entry< tree->GetEntries(); ++entry) {
    tree->GetEntry(entry);

    if(cup_ != cup || iteration_ != iteration)
      continue; // failed cut

    assert(time_ >= 0);
    if(t0 < 0) t0 = time_;

    if(time_ - t0 > skipBegin) {
      if(!time.empty() && !(time_ >= time.back())) {
        utils::Error("RossumData::AverageCurrent", __FILE__, __LINE__)
          << "Non sequential current readings: " << "this, last = "
          << time_ << ", " << time.back() << "\n";
        return UDouble_t(0, 0);
      }

      current.push_back(current_);
      time.push_back(time_);
    }
  }

  //
  // Find last time
  const std::vector<Double_t>::iterator::difference_type diffLast =
    std::lower_bound(time.begin(), time.end(), time.back() - skipEnd, std::less_equal<Double_t>()) - time.begin();

  const std::vector<Double_t>::iterator lastCurrent =
    current.begin() + diffLast;

  Double_t avg = utils::calculate_mean(current.begin(), lastCurrent);
  Double_t stddev = utils::calculate_stddev(current.begin(), lastCurrent, avg);
  return UDouble_t(avg, stddev);
}

void dragon::RossumData::ListTrees() const
{
  for (TreeMap_t::const_iterator it = fTrees.begin(); it != fTrees.end(); ++it) {
    std::cout << GetTree_(it)->GetName() << "\t" << GetTree_(it)->GetTitle() << ", DATIME: " << GetTime_(it) << "\n";
  }
}

std::vector<Int_t>& dragon::RossumData::GetRunsVector() const
{
  static std::vector<Int_t> runs;
  runs.clear();

  for (TreeMap_t::const_iterator it = fTrees.begin(); it != fTrees.end(); ++it) {
    Bool_t skip = kFALSE;
    std::string tname = GetTree_(it)->GetName();
    std::string run = tname.substr(4);
    std::string::const_iterator it1 = run.begin();
    while (it1 != run.end()) {
      if (!(std::isdigit(*it1))) {
        skip = kTRUE;
        break;
      } else ++it1;
    }
    if (skip) continue;
    runs.push_back(it->first);
  }
  return runs;
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

/////////////// Class dragon::BeamNorm ///////////////

////////////////////////////////////////////////////////////////////////////////
/// Default constructor

dragon::BeamNorm::BeamNorm():
  fRunDataTree("t_rundata", ""), fRossum(0)
{
  fRunDataTree.SetMarkerStyle(21);
  fRunDataTree.Branch("rundata", "dragon::BeamNorm::RunData", &fRunDataBranchAddr);
}

////////////////////////////////////////////////////////////////////////////////
/// Construct from rossum file

dragon::BeamNorm::BeamNorm(const char* name, const char* rossumFile):
  fRunDataTree("t_rundata", ""), fRossum(0)
{
  SetNameTitle(name, rossumFile);
  ChangeRossumFile(rossumFile);

  fRunDataTree.SetMarkerStyle(21);
  fRunDataTree.Branch("rundata", "dragon::BeamNorm::RunData", &fRunDataBranchAddr);
}

////////////////////////////////////////////////////////////////////////////////
/// Switch to a new rossum file

void dragon::BeamNorm::ChangeRossumFile(const char* name)
{
  SetTitle(name);
  std::string rossumName = GetName();
  rossumName += "_rossum";
  fRossum.reset(new RossumData(rossumName.c_str(), name));
}

////////////////////////////////////////////////////////////////////////////////
/// Integrate the surface barrier counts at the beginning and end of a run
///
/// Calculates the "R-value" to normalize SB readings to Faraday cup data.
/// Stores the result in fRunData, they can be accessed by using GetRunData()
///
/// \param datafile Pointer to the run's ROOT file
/// \param pkLow0 Low end of the SB0 good peak
/// \param pkHigh0 High end of the SB0 good peak
/// \param pkLow1 Low end of the SB1 good peak
/// \param pkHigh1 High end of the SB1 good peak
/// \param time Number of seconds at the beginning of the run to use for
///  calculating the normalization
///
/// \returns The run number of the specified file.

Int_t dragon::BeamNorm::ReadSbCounts(TFile* datafile, Double_t pkLow0, Double_t pkHigh0,
                                     Double_t pkLow1, Double_t pkHigh1,Double_t time)
{
  if(!HaveRossumFile()) {
    std::cout << "\n";
    dutils::Error("BeamNorm::ReadSbCounts", __FILE__, __LINE__)
      << "no rossum file loaded.";
    return 0;
  }
  if(!datafile || datafile->IsZombie()) {
    std::cout << "\n";
    dutils::Error("BeamNorm::ReadSbCounts", __FILE__, __LINE__)
      << "Invalid datafile: " << datafile;
    return 0;
  }

  Bool_t haveRunnum = kFALSE;
  Int_t runnum = 0;
  midas::Database* db = static_cast<midas::Database*>(datafile->Get("odbstop"));

  if (db) haveRunnum = db->ReadValue("/Runinfo/Run number", runnum);
  else {
    std::cout << "\n";
    dutils::Warning("BeamNorm::ReadSbCounts", __FILE__, __LINE__)
      << "MIDAS database at run stop time missing from file " << datafile->GetName() << "\n";
    midas::Database* dbstart = static_cast<midas::Database*>(datafile->Get("odbstart"));
    if (dbstart) haveRunnum = dbstart->ReadValue("/Runinfo/Run number", runnum);
    else {
      std::cout << "\n";
      dutils::Error("BeamNorm::ReadSbCounts", __FILE__, __LINE__)
        << "MIDAS database at run start time missing from file " << datafile->GetName() << "\n";
      return 0;
    }
  }
  if(!haveRunnum) {
    std::cout << "\n";
    dutils::Error("BeamNorm::ReadSbCounts", __FILE__, __LINE__)
      << "couldn't read run number from file " << datafile->GetName();
    return 0;
  }

  TTree* t3 = static_cast<TTree*>(datafile->Get("t3"));
  if(t3 == 0 || t3->GetListOfBranches()->At(0) == 0) {
    std::cout << "\n";
    dutils::Error("BeamNorm::ReadSbCounts", __FILE__, __LINE__)
      << "no heavy-ion data tree in file" << datafile->GetName();
    return 0;
  }

  TTree* t20 = static_cast<TTree*>(datafile->Get("t20"));
  if(t20 == 0 || t20->GetListOfBranches()->At(0) == 0) {
    dutils::Error("BeamNorm::ReadSbCounts", __FILE__, __LINE__)
      << "no EPICS tree in file" << datafile->GetName();
    return 0;
  }

  dragon::Tail tail;
  dragon::Tail *pTail = &tail;
  t3->SetBranchAddress(t3->GetListOfBranches()->At(0)->GetName(), &pTail);

  dragon::Epics epics;
  dragon::Epics* pEpics = &epics;
  t20->SetBranchAddress(t20->GetListOfBranches()->At(0)->GetName(), &pEpics);

  AutoResetBranchAddresses Rst3_(t3);
  AutoResetBranchAddresses Rst20_(t20);

  Long64_t ncounts[NSB];
  Long64_t ncounts_full[NSB];
  for(int i=0; i< NSB; ++i) {
    ncounts[i] = 0;
    ncounts_full[i] = 0;
  }

  t3->GetEntry(0);
  Double_t tstart = pTail->header.fTimeStamp;
  Double_t low[NSB] =  { pkLow0,  pkLow1  };
  Double_t high[NSB] = { pkHigh0, pkHigh1 };

  for(int i=0; i< NSB; ++i) {
    std::stringstream cut;
    cut.precision(20);
    cut << "sb.ecal[" << i << "] > " << low[i] << " && sb.ecal[" << i << "] < " << high[i];
    ncounts_full[i] = t3->GetPlayer()->GetEntries(cut.str().c_str());

    cut << " && header.fTimeStamp - " << tstart << " < " << time;
    ncounts[i] = t3->GetPlayer()->GetEntries(cut.str().c_str());
  }

  UDouble_t live, live_full[3]; // [3]: head,tail,coinc
  {
    dragon::LiveTimeCalculator ltc;
    ltc.SetFile(datafile);
    ltc.CalculateSub(0, time);
    live = ltc.GetLivetime("tail");
    ltc.Calculate();
    live_full[0] = ltc.GetLivetime("head");
    live_full[1] = ltc.GetLivetime("tail");
    live_full[2] = ltc.GetLivetime("coinc");
  }

  t20->GetEntry(0);
  Double_t tstart20 = pTail->header.fTimeStamp;
  Int_t t1 = 0;
  std::vector<double> pressure;

  for(Long64_t entry = 0; entry != t20->GetEntries(); ++entry) {
    t20->GetEntry(entry);
    if(pEpics->ch == 0) {
      pressure.push_back(pEpics->val);
      if(pEpics->header.fTimeStamp - tstart20 < time)
        ++t1;
    }
  }

  RunData* rundata = GetOrCreateRunData(runnum);

  rundata->time = time;
  for(int i=0; i< NSB; ++i) {
    rundata->sb_counts[i] = UDouble_t(ncounts[i]);
    rundata->sb_counts_full[i] = UDouble_t(ncounts_full[i]);
  }
  //
  // Pressure over SB norm time
  {
    const Double_t pressureMean = utils::calculate_mean(pressure.begin(), pressure.begin() + t1);
    const Double_t pressureSigma = utils::calculate_stddev(pressure.begin(), pressure.begin() + t1, pressureMean);
    rundata->pressure = UDouble_t (pressureMean, pressureSigma);
  }
  //
  // Pressure over full run
  {
    const Double_t pressureMean = utils::calculate_mean(pressure.begin(), pressure.end());
    const Double_t pressureSigma = utils::calculate_stddev(pressure.begin(), pressure.end(), pressureMean);
    rundata->pressure_full = UDouble_t (pressureMean, pressureSigma);
  }

  rundata->live_time = live;
  rundata->live_time_head  = live_full[0];
  rundata->live_time_tail  = live_full[1];
  rundata->live_time_coinc = live_full[2];

  return runnum;
}

////////////////////////////////////////////////////////////////////////////////
/// Read rossum FC4 current

void dragon::BeamNorm::ReadFC4(Int_t runnum, Double_t skipBegin, Double_t skipEnd)
{
  if(!HaveRossumFile()) {
    dutils::Error("BeamNorm::ReadFC4", __FILE__, __LINE__)
      << "no rossum file loaded.";
    return;
  }

  RunData* rundata = GetOrCreateRunData(runnum);
  for(int i=0; i< 3; ++i) {
    rundata->fc4[i] = fRossum->AverageCurrent(runnum, 0, i, skipBegin, skipEnd);
  }

  rundata->fc1 = fRossum->AverageCurrent(runnum, 1, 0, skipBegin, skipEnd);
}

////////////////////////////////////////////////////////////////////////////////
/// Calculate R and total beam particles

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
        rundata->nbeam[i] = (rundata->sb_counts_full[i] / rundata->live_time_tail) * rundata->sbnorm[i] / rundata->pressure_full;
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
/// Return stored values of run data

dragon::BeamNorm::RunData* dragon::BeamNorm::GetRunData(Int_t runnum)
{
  std::map<Int_t, RunData>::iterator i = fRunData.find(runnum);
  return i == fRunData.end() ? 0 : &(i->second);
}

dragon::BeamNorm::RunData* dragon::BeamNorm::GetOrCreateRunData(Int_t runnum)
{
  RunData* rundata = GetRunData(runnum);
  if(!rundata) { // Doesn't exist yet, create
    RunData ddd;
    ddd.runnum = runnum;
    fRunData.insert(std::make_pair(runnum, ddd));
    rundata = GetRunData(runnum);

    fRunDataBranchAddr = rundata;
    if(fRunDataTree.GetListOfBranches()->GetEntries() == 0) {
      fRunDataTree.Branch("rundata", "dragon::BeamNorm::RunData", &fRunDataBranchAddr);
    }
  }
  return rundata;
}

////////////////////////////////////////////////////////////////////////////////
/// Return a vector of run numbers used in the calculation

std::vector<Int_t>& dragon::BeamNorm::GetRuns() const
{
  static std::vector<Int_t> runs;
  runs.clear();
  for(std::map<Int_t, RunData>::const_iterator it = fRunData.begin(); it != fRunData.end(); ++it) {
    runs.push_back(it->first);
  }
  return runs;
}

UInt_t dragon::BeamNorm::GetParams(const char* param, std::vector<Double_t> *runnum, std::vector<UDouble_t> *parval)
{
  TTreeFormula formula("formula", param, &fRunDataTree);
  if(formula.GetNdim() == 0) return 0;
  TLeaf* leaf = formula.GetLeaf(0);
  if(!leaf) return 0;

  TString form = param, form1, form2;
  if(!strcmp(leaf->GetTypeName(), "UDouble_t")) {
    form1 = form2 = param;
    form  += ".GetNominal()";
    form1 += ".GetErrLow()";
    form2 += ".GetErrHigh()";
  }

  std::vector<Double_t> central, low, high;

  this->Draw("runnum", "", "goff");
  for(Int_t i=0; i< fRunDataTree.GetEntries(); ++i)
    runnum->push_back(fRunDataTree.GetV1()[i]);

  this->Draw(form, "", "goff");
  for(Int_t i=0; i< fRunDataTree.GetEntries(); ++i)
    central.push_back(fRunDataTree.GetV1()[i]);

  if(form1.IsNull() == 0) {
    this->Draw(Form("%s:%s", form1.Data(), form2.Data()), "", "goff");
    for(Int_t i=0; i< fRunDataTree.GetEntries(); ++i) {
      low.push_back(fRunDataTree.GetV1()[i]);
      high.push_back(fRunDataTree.GetV2()[i]);
    }
  } else {
    for(Int_t i=0; i< fRunDataTree.GetEntries(); ++i) {
      low.push_back(0);
      high.push_back(0);
    }
  }
  for(Int_t i=0; i< fRunDataTree.GetEntries(); ++i)
    parval->push_back(UDouble_t(central[i], low[i], high[i]));

  return runnum->size();
}

////////////////////////////////////////////////////////////////////////////////
/// Plot some parameter as a function of run number
/// \param param String specifying the parameter to plot, should be a member of
///  dragon::BeamNorm::RunData
/// \param marker Marker symbol
/// \param markerColor Marker color
/// \returns TGraph pointer containing _param_ vs. run number, responsibility is on the
///  user to delete this. In case of error, returns 0.
///
///  Also draws the returned TGraph in its own window.

TGraphAsymmErrors* dragon::BeamNorm::Plot(const char* param, Marker_t marker, Color_t markerColor)
{
  TGraphAsymmErrors* gr = 0;
  std::vector<Double_t>  runnum;
  std::vector<UDouble_t> parval;
  UInt_t npar = GetParams(param, &runnum, &parval);

  if(npar) {
    gr = PlotUncertainties(runnum.size(), &runnum[0], &parval[0]);
    gr->SetMarkerStyle(marker);
    gr->SetMarkerColor(markerColor);
    gr->Draw("AP");
  }
  return gr;
}

////////////////////////////////////////////////////////////////////////////////
/// Plot some parameter as a function of run number (alt. implementation)
/// \param valstr String specifying the parameter to plot, should be a member of
///  dragon::BeamNorm::RunData
/// \param which Specifies which array index to plot, ignored if valstr is not an array
/// \param marker Marker symbol
/// \param markerColor Marker color
/// \returns TGraphErrors pointer containing _param_ vs. run number, responsibility is on the
///  user to delete this. In case of error, returns 0.
///
///  Also draws the returned TGraph in its own window.

TGraphErrors* dragon::BeamNorm::PlotVal(const TString& valstr, int which, Marker_t marker, Color_t markerColor, Option_t* option)
{
  std::vector<double> rn, val, err;
  for(size_t i=0; i< GetRuns().size(); ++i) {
    const dragon::BeamNorm::RunData* rd = GetRunData(GetRuns()[i]);
    rn.push_back(rd->runnum);

    if(0) {
    }
    else if(valstr == "sb_counts") {
      val.push_back(rd->sb_counts[which].GetNominal());
      err.push_back(rd->sb_counts[which].GetErrLow());
    }
    else if(valstr == "sb_counts_full") {
      val.push_back(rd->sb_counts_full[which].GetNominal());
      err.push_back(rd->sb_counts_full[which].GetErrLow());
    }
    else if(valstr == "live_time") {
      val.push_back(rd->live_time.GetNominal());
      err.push_back(rd->live_time.GetErrLow());
    }
    else if(valstr == "live_time_tail") {
      val.push_back(rd->live_time_tail.GetNominal());
      err.push_back(rd->live_time_tail.GetErrLow());
    }
    else if(valstr == "live_time_head") {
      val.push_back(rd->live_time_head.GetNominal());
      err.push_back(rd->live_time_head.GetErrLow());
    }
    else if(valstr == "live_time_coinc") {
      val.push_back(rd->live_time_coinc.GetNominal());
      err.push_back(rd->live_time_coinc.GetErrLow());
    }
    else if(valstr == "pressure") {
      val.push_back(rd->pressure.GetNominal());
      err.push_back(rd->pressure.GetErrLow());
    }
    else if(valstr == "pressure_full") {
      val.push_back(rd->pressure_full.GetNominal());
      err.push_back(rd->pressure_full.GetErrLow());
    }
    else if(valstr == "fc4") {
      val.push_back(rd->fc4[which].GetNominal());
      err.push_back(rd->fc4[which].GetErrLow());
    }
    else if(valstr == "fc1") {
      val.push_back(rd->fc1.GetNominal());
      err.push_back(rd->fc1.GetErrLow());
    }
    else if(valstr == "trans_corr") {
      val.push_back(rd->trans_corr.GetNominal());
      err.push_back(rd->trans_corr.GetErrLow());
    }
    else if(valstr == "sbnorm") {
      val.push_back(rd->sbnorm[which].GetNominal());
      err.push_back(rd->sbnorm[which].GetErrLow());
    }
    else if(valstr == "nbeam") {
      val.push_back(rd->nbeam[which].GetNominal());
      err.push_back(rd->nbeam[which].GetErrLow());
    }
    else if(valstr == "nrecoil") {
      val.push_back(rd->nrecoil.GetNominal());
      err.push_back(rd->nrecoil.GetErrLow());
    }
    else if(valstr == "yield") {
      val.push_back(rd->yield[which].GetNominal());
      err.push_back(rd->yield[which].GetErrLow());
    }
    else {
      std::cerr << "Invalid parameter << \"" << valstr.Data() << "\"\n";
      break;
    }
  }

  TGraphErrors* gr = 0;
  if(val.size()) {
    gr = new TGraphErrors(rn.size(), &rn[0], &val[0], 0, &err[0]);
    gr->SetMarkerStyle(marker);
    gr->SetMarkerColor(markerColor);
    gr->SetLineColor(markerColor);
    gr->Draw(option);
  }

  return gr;
}

////////////////////////////////////////////////////////////////////////////////
/// Plot number of beam particles with manual sbnorm specification

TGraphErrors* dragon::BeamNorm::PlotNbeam(double sbnorm, int which, Marker_t marker, Color_t markerColor)
{
  std::vector<double> rn, val, err;
  for(size_t i=0; i< GetRuns().size(); ++i) {
    const dragon::BeamNorm::RunData* rd = GetRunData(GetRuns()[i]);
    rn.push_back(rd->runnum);

    UDouble_t nbm = (rd->sb_counts_full[which] / rd->live_time_tail) * sbnorm / rd->pressure_full;

    val.push_back(nbm.GetNominal());
    err.push_back(nbm.GetErrLow());
  }

  TGraphErrors* gr = new TGraphErrors(rn.size(), &rn[0], &val[0], 0, &err[0]);
  gr->SetMarkerStyle(marker);
  gr->SetMarkerColor(markerColor);
  gr->SetLineColor(markerColor);
  gr->Draw("AP");

  return gr;
}

namespace {

  void null_rundata(dragon::BeamNorm::RunData* rundata)
  {
	if(!rundata) return;
	rundata->nrecoil  = UDouble_t(0,0);
	for(int i=0; i< NSB; ++i) {
      rundata->yield[i] = UDouble_t(0,0);
      rundata->nbeam[i] = UDouble_t(0,0);
    }
  }

  const UDouble_t& get_livetime(const std::string& treename, const dragon::BeamNorm::RunData* rundata) {
	if(!rundata) { }
	else if(treename == "t1") return rundata->live_time_head;
	else if(treename == "t3") return rundata->live_time_tail;
	else if(treename == "t5") return rundata->live_time_coinc;
	throw rundata;
  }
}


////////////////////////////////////////////////////////////////////////////////
/// Calculate the number of recoils per run

void dragon::BeamNorm::CalculateRecoils(TFile* datafile, const char* treename, const char* gate)
{
  if(datafile == 0 || datafile->IsZombie()) {
    std::cerr << "Invalid datafile!\n";
    return;
  }
  TTree* t = (TTree*)datafile->Get(treename);
  if(!t || t->IsA() != TTree::Class()) {
    dutils::Error("BeamNorm::CalculateRecoils", __FILE__, __LINE__)
      << "no tree named \"" << treename << "\" in the specified file!";
    return;
  }

  // Copy aliases from chain
  TChain* chain = (TChain*)gROOT->GetListOfSpecials()->FindObject(treename);
  if(chain && chain->InheritsFrom(TChain::Class())) {
    if(chain->GetListOfAliases()) {
      for(Int_t i=0; i< chain->GetListOfAliases()->GetEntries(); ++i) {
        TObject* alias = chain->GetListOfAliases()->At(i);
        if(alias) t->SetAlias(alias->GetName(), alias->GetTitle());
      }
    }
  }

  Bool_t haveRunnum = kFALSE;
  Int_t runnum = 0;
  midas::Database* db = static_cast<midas::Database*>(datafile->Get("odbstop"));
  if(db) haveRunnum = db->ReadValue("/Runinfo/Run number", runnum);
  if(!db || !haveRunnum) {
    dutils::Error("BeamNorm::CalculateRecoils", __FILE__, __LINE__)
      << "couldn't read run number from TFile at " << datafile;
    return;
  }

  UDouble_t nrecoil (t->GetPlayer()->GetEntries(gate));

  RunData* rundata = GetOrCreateRunData(runnum);
  try {
    const UDouble_t& livetime = get_livetime(treename, rundata);
    rundata->nrecoil = nrecoil;
    for(Int_t i=0; i< NSB; ++i) {
      UDouble_t eff = CalculateEfficiency(kFALSE);
      if(rundata->nbeam[i].GetNominal() != 0 && eff.GetNominal() != 0) {
        rundata->yield[i] =
          nrecoil / rundata->nbeam[i] / eff / livetime / rundata->trans_corr;
      }
    }
  } catch (RunData*) {
    null_rundata(rundata);
  }
}

////////////////////////////////////////////////////////////////////////////////
/// Draw run data information

Long64_t dragon::BeamNorm::Draw(const char* varexp, const char* selection, Option_t* option,
                                Long64_t nentries, Long64_t firstentry)
{
  //
  // Fill tree w/ latest data (circular)
  fRunDataTree.SetCircular(fRunData.size());
  for(std::map<Int_t, RunData>::iterator it = fRunData.begin();
      it != fRunData.end(); ++it) {
    fRunDataBranchAddr = &(it->second);
    fRunDataTree.Fill();
  }

  // Then draw from tree
  return fRunDataTree.Draw(varexp, selection, option, nentries, firstentry);
}

////////////////////////////////////////////////////////////////////////////////
/// Batch calculate over a chain of files
void dragon::BeamNorm::BatchCalculate(TChain* chain, Int_t chargeBeam,
                                      Double_t pkLow0, Double_t pkHigh0,
									  Double_t pkLow1, Double_t pkHigh1,
									  const char* recoilGate,
									  Double_t time, Double_t skipBegin, Double_t skipEnd)
{
  TObjArray* flist = chain->GetListOfFiles();
  for(Int_t i=0; i< flist->GetEntries(); ++i) {
    if(!i)
      std::cout << "Calculating normalization for runs ";

    std::auto_ptr<TFile> file (TFile::Open(flist->At(i)->GetTitle()));
    Int_t runnum = ReadSbCounts(file.get(), pkLow0, pkHigh0, pkLow1, pkHigh1, time);
    if(!runnum) continue;

    std::cout << runnum << "... ";
    std::flush(std::cout);

    ReadFC4(runnum, skipBegin, skipEnd);
    CalculateNorm(runnum, chargeBeam);
    if(recoilGate) {
      CalculateRecoils(file.get(), chain->GetName(), recoilGate);
    }
  }
  std::cout << "\n";
}

////////////////////////////////////////////////////////////////////////////////
/// Correct for FC4->FC1 transmission changes relative to a single "good" run
///
/// Corrects the `yield` and `nbeam` values for each run such that the transmission matches
/// the reference run.
///
/// \param reference Run number of the reference run.
/// \todo Change `nbeam` to `nbeam_fc4` and `nbeam_fc1` or something like that becuse now it's
/// confusing to change both yield and nbeam and not explicit enough what's going on.
///

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

UDouble_t dragon::BeamNorm::CalculateYield(Int_t whichSb, Int_t type, Bool_t print)
{
  if(whichSb < 0 || whichSb >= NSB) {
    std::cerr << "Invalid sb index << " << whichSb << ", valid options are 0 -> " << NSB-1 << "\n";
    return UDouble_t(0,0);
  }

  UDouble_t beam(0,0), recoil(0,0), recoilCounted(0,0), recoilTrans(0,0);
  std::vector<Double_t> recoilV, liveV;
  for(std::map<Int_t, RunData>::iterator it = fRunData.begin(); it != fRunData.end(); ++it) {
    RunData& thisData = it->second;

    UDouble_t livetime;
    switch(type) {
    case kGammaSingles: livetime = thisData.live_time_head;  break;
    case kHiSingles:    livetime = thisData.live_time_tail;  break;
    case kCoinc:        livetime = thisData.live_time_coinc; break;
    default:
      dutils::Error("CalculateYield", __FILE__, __LINE__)
        << "Invalid \"type\" specification " << type << ", must be 1 (gama singles), "
        << "3 (hi singles), or 5 (coinc)";
      return UDouble_t (0,0);
    }

    beam += thisData.nbeam[whichSb];
    recoil += (thisData.nrecoil / thisData.trans_corr / livetime);
    recoilCounted += thisData.nrecoil;
    recoilTrans   += thisData.nrecoil / thisData.trans_corr;

    // for recoil-weighted livetime
    recoilV.push_back(thisData.nrecoil.GetNominal());
    liveV.push_back(livetime.GetNominal());
  }

  UDouble_t eff = CalculateEfficiency(kFALSE);
  UDouble_t out = recoil / beam / eff;
  Double_t liveAvg = utils::calculate_weighted_mean(liveV.begin(), liveV.end(), recoilV.begin());

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
  Reset();
}

dragon::LiveTimeCalculator::LiveTimeCalculator(TFile* file, Bool_t calculate):
  fFile(file)
{
  /// \param file Pointer to TFile containing run data. Takes no ownership.
  /// \param calculate If set to true, performs a full-run live time calculation
  ///  on initialization

  Reset();
  if(calculate) Calculate();
}

void dragon::LiveTimeCalculator::Reset()
{
  std::fill_n(fRuntime,  2, 0.);
  std::fill_n(fBusytime, 2, 0.);
  std::fill_n(fLivetime, 3, 1.);
}

namespace {
  inline int get_which_indx(const char* which) {
	int indx = -1;
	TString which1 = which;
	which1.ToLower();
	if     (!which1.CompareTo("head"))  indx = 0;
	else if(!which1.CompareTo("tail"))  indx = 1;
	else if(!which1.CompareTo("coinc")) indx = 2;
	return indx;
  }

  inline Double_t get_from_array(const Double_t* arr, const char* which, const char* func) {
	int indx = get_which_indx(which);

	if(indx < 0) {
      dutils::Error(func, __FILE__, __LINE__)
        << "Invalid specification \"" << which
        << "\", please specify \"head\" or \"tail\"";
      return 0;
	}

	return *(arr+indx);
  } }

Double_t dragon::LiveTimeCalculator::GetBusytime(const char* which) const
{
  return get_from_array(fBusytime, which, "GetBusytime");
}

Double_t dragon::LiveTimeCalculator::GetRuntime(const char*  which) const
{
  return get_from_array(fRuntime, which, "GetRuntime");
}

Double_t dragon::LiveTimeCalculator::GetLivetime(const char* which) const
{
  return get_from_array(fLivetime, which, "GetLivetime");
}

Bool_t dragon::LiveTimeCalculator::CheckFile(TTree*& t1, TTree*& t3, midas::Database*& db)
{
  ///
  /// Also sets tree and DB pointers

  if(!fFile || fFile->IsZombie()) {
    dutils::Error("LiveTimeCalculator::CheckFile", __FILE__, __LINE__)
      << "Invalid or no file loaded.";
    return kFALSE;
  }
  Bool_t okay = kTRUE;
  if(okay) okay = fFile->Get("t1") && fFile->Get("t1")->IsA() == TTree::Class();
  if(okay) okay = fFile->Get("t3") && fFile->Get("t3")->IsA() == TTree::Class();
  if(!okay) {
    dutils::Error("LiveTimeCalculator::CheckFile", __FILE__, __LINE__)
      << "missing necessary trees in loaded file";
    return kFALSE;
  } else {
    t1 = static_cast<TTree*>(fFile->Get("t1"));
    t3 = static_cast<TTree*>(fFile->Get("t3"));
  }

  if(okay) okay = t1->GetLeaf("io32.busy_time");
  if(okay) okay = t3->GetLeaf("io32.busy_time");
  if(!okay) {
    dutils::Error("LiveTimeCalculator::CheckFile", __FILE__, __LINE__)
      << "missing leaf \"io32.busy_time\" in either \"t1\" or \"t3\"";
    return kFALSE;
  }

  if(okay) okay = t1->GetLeaf("io32.tsc4.trig_time");
  if(okay) okay = t3->GetLeaf("io32.tsc4.trig_time");
  if(!okay) {
    dutils::Error("LiveTimeCalculator::CheckFile", __FILE__, __LINE__)
      << "missing leaf \"io32.tsc4.trig_time\" in either \"t1\" or \"t3\"";
    return kFALSE;
  }

  okay = fFile->Get("odbstop") && fFile->Get("odbstop")->InheritsFrom(midas::Database::Class());
  if(!okay) {
    dutils::Error("LiveTimeCalculator::CheckFile", __FILE__, __LINE__)
      << "Loaded file is missing database \"odbstop\"";
    return kFALSE;
  }

  db = static_cast<midas::Database*>(fFile->Get("odbstop"));
  return kTRUE;
}

Double_t dragon::LiveTimeCalculator::CalculateRuntime(midas::Database* db, const char* which,
                                                      Double_t& start, Double_t& stop)
{
  ///
  /// \param [in] db Pointer to the "odbstop" database for the current run
  /// \param [in] which "head", "tail", or "coinc"
  /// \param [out] start Run start time (relative to TSC reset)
  /// \param [out] stop Run stop time (relative to TSC reset)
  /// \returns Total run time in seconds. Note that in the case of
  /// "coinc", the runtime is calculated as
  /// < smallest stop time > - < largest start time >, i.e. it is the
  /// total time during which _both_ acquisitions are taking triggers.
  ///

  // TSC stop is stored in the ODB with rollover, so first figure
  // out rollover time from comparison w/ the computer clock
  /// \attention In some cases this could fail to correctly calculate
  /// the rollover due to mismatches in the clocks... I think. This will
  /// be extremely rare but is something to keep in the back of your mind.
  Int_t time0, time1, tclock;
  db->ReadValue("/Runinfo/Start time binary", time0); // computer clock start
  db->ReadValue("/Runinfo/Stop time binary",  time1); // computer clock stop
  tclock = time1 - time0;

  // Read TSC start, stop times from the ODB
  double trigStart[2], trigStop[2];
  db->ReadArray("/Experiment/Run Parameters/TSC_TriggerStart", trigStart, 2);
  db->ReadArray("/Experiment/Run Parameters/TSC_TriggerStop",  trigStop,  2);

  // Append rollovers to stop time
  const Double_t rolltime = 0xffffffff / 20e6; // 32-bit clock rollover in seconds
  int nroll = tclock / rolltime;
  for(int i=0; i<2; ++i) trigStop[i] += nroll*rolltime;

  // now calculte the appropriate run time
  switch(get_which_indx(which)) {
  case 0: // "head"
    stop  = trigStop[0];
    start = trigStart[0];
    break;
  case 1: // "tail"
    stop  = trigStop[1];
    start = trigStart[1];
    break;
  case 2: // "coinc"
    stop  = *std::min_element(trigStop, trigStop + 2);
    start = *std::max_element(trigStart, trigStart + 2);
    break;
  default:
    utils::Error("CalculateRuntime", __FILE__, __LINE__)
      << "Invalid \"which\" specification: \"" << which << "\", valid options "
      << "are \"head\", \"tail\", or \"coinc\"";
    stop = 0;
    start = 0;
    break;
  }

  return stop - start;
}

namespace {
  // fix for an early frontend bug where the tsc4 36-bit rollover counter
  // started from 1 instead of 0
  inline Double_t correct_rollover_fe_bug(TBranch* trigBranch, Double_t& trig_time)
  {
	Double_t rollCorrect = 0;
	const ULong_t roll36 = (ULong64_t)1<<36;
	trigBranch->GetEntry(0);

	if(trig_time >= roll36/20.) { // the bug was present
      rollCorrect = roll36/20.; // microseconds

      // This should only be present in files analyzed on jabberwock, and part of the
      // DAQ test, so warn otherwise
      TString hostname = "$HOSTNAME1";
      gSystem->ExpandPathName(hostname);
      if(hostname.CompareTo("jabberwock.triumf.ca")) {
        TString fname = trigBranch->GetTree()->GetDirectory()->GetName();
        if(!fname.Contains("DAQ_test"))
          dutils::Warning("DoCalculate", __FILE__, __LINE__)
            << "rollCorrect != 0 and host is not jabberwock!";
      }
	}

	return rollCorrect;
  } }

void dragon::LiveTimeCalculator::DoCalculate(Double_t tbegin, Double_t tend)
{
  ///
  /// Generic function called by public functions
  /// Calculate() and CalculateSub()

  // reset to default values
  Reset();

  // make sure arguments are workable, checkif full calculation or not
  Bool_t isFull = (tbegin < 0 || tend < 0);
  if (isFull) {
    if(tbegin > 0 || tend > 0) {
      utils::Error("LiveTimeCalculator::DoCalculate", __FILE__, __LINE__)
        << "Invalid parameters to LiveTimeCalculator::DoCalculate(): "
        << "tbegin = " << tbegin <<  ", tend = " << tend;
      return;
    }
  }
  if (!isFull && tend <= tbegin) {
    utils::Error("LiveTimeCalculator::DoCalculate", __FILE__, __LINE__)
      << "Cannot have tend <= tbegin: tend, tbegin = " << tend << ", " << tbegin;
    return;
  }

  // Initialize midas database, TTrees, etc
  midas::Database* db = 0;
  TTree* trees[2] = { 0, 0 };
  if(!CheckFile(trees[0], trees[1], db)) { // CheckFile now sets trees[i], db if they're valid
    utils::Error("LiveTimeCalculator::DoCalculate", __FILE__, __LINE__)
      << "Invalid file or trees";
    return;
  }
  size_t nentries = trees[0]->GetEntries() + trees[1]->GetEntries();
  CoincBusytime coincBusy(nentries);

  // Ensure to reset branch addresses when done
  AutoResetBranchAddresses Rst_(trees, 2);

  // Get run times
  double trigStart[3], trigStop[3];
  CalculateRuntime(db, "head",  trigStart[0], trigStop[0]);
  CalculateRuntime(db, "tail",  trigStart[1], trigStop[1]);
  CalculateRuntime(db, "coinc", trigStart[2], trigStop[2]);

  // Loop over trees
  for(int i=0; i< 2; ++i) {
    // Set MakeClass to 1 (resets at end of loop or return)
    SetMakeClass_t dummy(trees[i], 1);

    // Find branches
    UInt_t busy_time;
    Double_t trig_time;
    TBranch* trigBranch = get_branch(trees[i], trig_time, "io32.tsc4.trig_time", "LiveTimeCalculator::DoCalculate");
    TBranch* busyBranch = get_branch(trees[i], busy_time, "io32.busy_time", "LiveTimeCalculator::DoCalculate");
    if(!trigBranch || !busyBranch) return;

    // Correct TSC4 rollover for early runs taken with a frontend bug
    Double_t rollCorrect = correct_rollover_fe_bug(trigBranch, trig_time);

    Long64_t busyTotal = 0;
    Long64_t nentries = trees[i]->GetEntries();
    for(Long64_t entry = 0; entry< nentries; ++entry) {

      // read entries
      trigBranch->GetEntry(entry);
      busyBranch->GetEntry(entry);
      const Double_t trigtime = trig_time - rollCorrect - trigStart[i];

      // check cut condition
      if (!isFull && !(trigtime > tbegin*1e6 && trigtime < tend*1e6))
        continue;

      // accuulate busy time
      busyTotal += busy_time;
      coincBusy.AddEvent(trigtime, busy_time/20.);
    }

    // calculate variables
    fBusytime[i] = busyTotal / 20e6;
    fRuntime[i]  = isFull ? trigStop[i] - trigStart[i] : tend - tbegin;
    fLivetime[i] = (fRuntime[i] - fBusytime[i]) / fRuntime[i];
  }

  fRuntime[2]  = isFull ? trigStop[2] - trigStart[2] : tend - tbegin;
  fBusytime[2] = coincBusy.Calculate();
  fLivetime[2] = (fRuntime[2] - fBusytime[2]) / fRuntime[2];
}

void dragon::LiveTimeCalculator::Calculate()
{
  ///
  /// Results can be viewed with GetLivetime(), GetBusytime(), etc.

  DoCalculate(-1, -1);
}


void dragon::LiveTimeCalculator::CalculateSub(Double_t tbegin, Double_t tend)
{
  ///
  /// \param tbegin Beginning of the run sub section (in seconds)
  /// \param tend End of the run sub section (seconds). If specified greater than
  ///  the end of the run, this will just calculate up to the end of the run.
  ///
  /// The parameters _tbegin_ and _tend_ are both relative to the "trigger
  /// start", which is the time at which the frontend begins to accept
  /// triggers (this is ~2 seconds later than the "run start" time, which
  /// would be when the user pushed start on the MIDAS page. The difference
  /// is due to the time it takes for the frontend programs to complete
  /// their startup routines.
  ///
  /// Results can be viewed with GetLivetime(), GetBusytime(), etc.

  DoCalculate(tbegin, tend);
}

void dragon::LiveTimeCalculator::CalculateChain(TChain* chain)
{
  ///
  /// Calculate overall times across a chain of runs.
  /// For this, the sum of running time and busy time across
  /// the chain are calculated and then used to figure out the
  /// live time fraction.

  TFile* file0 = GetFile(); if(file0) { }
  Double_t sumbusy[3] = {0,0,0}, sumrun[3] = {0,0,0};

  for (Int_t i=0; i< chain->GetListOfFiles()->GetEntries(); ++i) {
    TFile* f = TFile::Open ( chain->GetListOfFiles()->At(i)->GetTitle() );
    if(f->IsZombie()) { f->Close(); continue; }

    SetFile(f);
    Calculate();
    sumrun[0]  += GetRuntime("head");
    sumrun[1]  += GetRuntime("tail");
    sumrun[2]  += GetRuntime("coinc");
    sumbusy[0] += GetBusytime("head");
    sumbusy[1] += GetBusytime("tail");
    sumbusy[2] += GetBusytime("coinc");

    f->Close();
  }

  for(int i=0; i< 3; ++i) {
    fBusytime[i] = sumbusy[i];
    fRuntime[i]  = sumrun[i];
    fLivetime[i] = (sumrun[i] - sumbusy[i]) / sumrun[i];
  }
}


// ============ class dragon::CoincBusytime ============ //

// ====== Helper class ====== //
namespace {
  struct AccumulateBusy {
	struct Output_t {
      Double_t fEnd;
      Double_t fTotalBusy;
	};

	Output_t operator() (const Output_t& current, const dragon::CoincBusytime::Event& additional);
  };

  inline AccumulateBusy::Output_t AccumulateBusy::operator()
	(const Output_t& current, const dragon::CoincBusytime::Event& additional)
  {
	Output_t out = current;

	if( current.fEnd < additional.fTrigger ) { // no overlap
      out.fTotalBusy += additional.fBusy;
      out.fEnd = additional.End();
	}
	else if( current.fEnd < additional.End() ) { // incomplete overlap
      out.fTotalBusy += (additional.End() - current.fEnd);
      out.fEnd = additional.End();
	}
	else { // complete overlap
      ;
	}

	return out;
  } }

dragon::CoincBusytime::CoincBusytime(size_t reserve):
  fIsSorted(false)
{
  ///
  /// \param reserve If nonzero, reserve memory space for this many events
  /// (calls std::vector::reserve()).

  if(reserve) fEvents.reserve(reserve);
}


void dragon::CoincBusytime::AddEvent(Double_t trigger, Double_t busy)
{
  ///
  /// \param trigger Trigger time in microseconds
  /// \param busy Busy time in microseconds

  fIsSorted = false;
  dragon::CoincBusytime::Event evt(trigger, busy);
  try {
    fEvents.push_back(evt);
  } catch (std::bad_alloc& err) {
    utils::Error("CoincBusytime::AddEvent", __FILE__, __LINE__)
      << "Failed to allocate memory for event: trigger, busy = "
      << trigger << ", " << busy;
    throw err;
  }
}

void dragon::CoincBusytime::Sort()
{
  std::sort(fEvents.begin(), fEvents.end(), dragon::CoincBusytime::Event::TriggerCompare);
  fIsSorted = true;
}


Double_t dragon::CoincBusytime::Calculate()
{
  ///
  /// \returns Total "coincidence busy time" in seconds.
  ///
  /// Calcluation of the busy time is done as follows:
  /// First, sort the collection of events by the trigger time. Then sum up all of
  /// the busy times, but ignoring any overlap. To accomplish this, use the
  /// `std::accumulate` algorithm, with a custom "accumulator" function which
  /// ignores overlap by storing the last "end time" of an event and removing
  /// any overlap between the begin time of the present event and the previous
  /// end time. The code below shows how this is accomplished.
  /// \code
  /// struct AccumulateBusy {
  ///		struct Output_t {
  ///			Double_t fEnd;
  ///			Double_t fTotalBusy;
  ///		};
  ///
  ///		Output_t operator() (const Output_t& current, const dragon::CoincBusytime::Event& additional);
  /// };
  ///
  /// AccumulateBusy::Output_t AccumulateBusy::operator() (const Output_t& current, const dragon::CoincBusytime::Event& additional)
  /// {
  ///		Output_t out = current;
  ///
  ///		if( current.fEnd < additional.fTrigger ) { // no overlap
  ///			out.fTotalBusy += additional.fBusy;
  ///			out.fEnd = additional.End();
  ///		}
  ///		else if( current.fEnd < additional.End() ) { // incomplete overlap
  ///			out.fTotalBusy += (additional.End() - current.fEnd);
  ///			out.fEnd = additional.End();
  ///		}
  ///		else { // complete overlap
  ///			;
  ///		}
  ///
  ///		return out;
  /// }
  ///
  /// Double_t dragon::CoincBusytime::Calculate()
  /// {
  ///   if(!fIsSorted) Sort();
  ///   AccumulateBusy::Output_t accum = { 0., 0. };
  ///   accum = std::accumulate(fEvents.begin(), fEvents.end(), accum, AccumulateBusy());
  ///   return accum.fTotalBusy / 1e6;
  /// }
  /// \endcode
  ///

  if(!fIsSorted) Sort();
  AccumulateBusy::Output_t accum = { 0., 0. };
  accum = std::accumulate(fEvents.begin(), fEvents.end(), accum, AccumulateBusy());
  return accum.fTotalBusy / 1e6;
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
                                                           Double_t cmd1, Double_t cmd1Err)
{
  ///
  /// Equation used is from Dave H's NIM paper on BGO z-position (includes relativity):
  /// `E/m = cmd1 * (qB/m)^2 - [1/(2*u*c^2)] * (E/m)^2`
  ///
  /// \param md1 MD1 field in Gauss
  /// \param md1Err Absolute error on MD1 field
  /// \param q Beam charge state
  /// \param m Beam mass in AMU
  /// \param cmd1 "Magnetic constant" for MD1 in (keV/u)*G^2
  /// \param cmd1Err Absolute error on _cmd1_
  ///
  /// \returns Beam energy in keV/u

  UDouble_t ucmd1 (cmd1, cmd1Err);
  UDouble_t uAMU  (dragon::Constants::AMU(), dragon::Constants::AMUUnc());

  UDouble_t a = 1/(2*uAMU);
  Double_t  b = 1.;
  UDouble_t c = -1*ucmd1*UDouble_t::pow((q*UDouble_t(md1, md1Err) / m), 2);

  return 1.e3*(-b + UDouble_t::sqrt(b*b - 4*a*c)) / (2.*a); // relativistic beam energy in keV / u
  // return -1.e3*c // non relativistic
}

dragon::StoppingPowerCalculator::StoppingPowerCalculator(Int_t beamCharge, Double_t beamMass, Int_t nmol,
                                                         Double_t temp, Double_t targetLen,
                                                         Double_t targetLenErr, Double_t cmd1, Double_t cmd1Err):
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
    dutils::Error("StoppingPowerCalculator::GetMeasurement", __FILE__, __LINE__)
      << "invalid index " << index << ", valid options are [0, ) " << fEnergies.size();
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
    dutils::Error("StoppingPowerCalculator::GetMeasurement", __FILE__, __LINE__)
      << "invalid index " << index << ", valid options are [0, ) " << fEnergies.size();
  }
}

TGraphAsymmErrors* dragon::StoppingPowerCalculator::PlotMeasurements(XAxisType_t xaxis, YAxisType_t yaxis, Bool_t draw, Bool_t print) const
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

  TGraphAsymmErrors* out = 0;
  if(!fEnergies.empty()) {

    const UDouble_t* x = xaxis == kDENSITY ? &fDensities[0] : &fPressures[0];
    const UDouble_t* y = yaxis == kMD1     ? &fMd1[0]       : &fEnergies[0];

    out = PlotUncertainties(fEnergies.size(), x, y);
  }
  if(out) {
    out->SetTitle("");
    if(xaxis == kPRESSURE){
      out->GetXaxis()->SetTitle("Pressure [Torr]");
    } else {
      out->GetXaxis()->SetTitle("Density [10^{15} atoms / cm^{2}]");
    }
    if(yaxis == kENERGY){
      out->GetYaxis()->SetTitle("Energy [keV / u]");
    } else {
      out->GetYaxis()->SetTitle("NMR1 Field [Gauss]");
    }
    out->SetMarkerStyle(20);
    out->GetXaxis()->CenterTitle();
    out->GetYaxis()->CenterTitle();
    if(draw) out->Draw("AP");
    if(print) out->Print();
  }
  return out;
}

UDouble_t dragon::StoppingPowerCalculator::CalculateEbeam(TGraphAsymmErrors** plot)
{
  ///
  /// \param [out] plot Address of a pointer to a TGraph that will contain
  ///  a plot of energy vs. pressure upon successful return. Passing a non-NULL
  ///  plot argument also causes the plot to be drawn with option "AP". The default
  ///  argument (NULL) bypasses any plotting and simply calculates the beam energy.
  /// \returns The beam energy at zero pressure from a linear fit of E vs. P.

  UDouble_t out(0,0);
  std::auto_ptr<TGraphAsymmErrors> g(new TGraphAsymmErrors(GetNmeasurements()));

  //
  // calculate density, energy w/ random errors only
  std::vector<UDouble_t> pres(GetNmeasurements()), energy(GetNmeasurements());
  for(Int_t i=0; i< GetNmeasurements(); ++i) {
    double md1err = fMd1[i].GetErrLow() < fMd1[i].GetErrHigh() ? fMd1[i].GetErrHigh() : fMd1[i].GetErrLow();
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
    *plot = (TGraphAsymmErrors*)g->Clone();
    (*plot)->SetMarkerStyle(20);
    (*plot)->SetTitle(Form("Beam energy: %.2f^{+ %.3f}_{-%.3f} keV/u;Pressure [Torr];Energy [keV/u]", out.GetNominal(), out.GetErrHigh(), out.GetErrLow() ) );
    (*plot)->GetXaxis()->CenterTitle();
    (*plot)->GetYaxis()->CenterTitle();
    (*plot)->Draw("AP");
    if(fit.GetFunction()) {
      TF1* ffit2 = (TF1*)fit.GetFunction()->Clone();
      ffit2->SetLineColor(4); ffit2->Draw("SAME");
    }
    else std::cerr << "No function!\n";
  }

  return out;

}

UDouble_t dragon::StoppingPowerCalculator::CalculateEpsilon(TGraphAsymmErrors** plot, UDouble_t* ebeam)
{
  ///
  /// \param [out] plot Address of a pointer to a TGraph that will contain
  ///  a plot of energy vs. density upon successful return. Passing a non-NULL
  ///  plot argument also causes the plot to be drawn with option "AP". The default
  ///  argument (NULL) bypasses any plotting and simply calculates the epsilon parameter.
  ///
  /// \param [out] ebeam Pointer to UDouble_t holding beam energy from offset of linear fit.
  ///
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

  if(ebeam) {
    *ebeam = 1*fit.GetOffset(); // keV/u;
  }
  //
  // add back systematic errors
  out *= fMd1Constant / fMd1Constant.GetNominal();
  out *= fTargetLength / fTargetLength.GetNominal();
  if(ebeam) {
    // *ebeam *= fMd1Constant / fMd1Constant.GetNominal();
    // *ebeam *= fTargetLength / fTargetLength.GetNominal();
  }
  //
  // plot if requested
  if(plot) {
    *plot = (TGraphAsymmErrors*)g->Clone();
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

UDouble_t dragon::ResonanceStrengthCalculator::CalculateResonanceStrength(Int_t whichSb, Int_t type, Bool_t print)
{
  /// \param whichSb Specify the surface barrier detector to use
  ///  for normalization
  /// \returns Resonance strenght in eV.

  if(!fBeamNorm) return UDouble_t(0,0);
  UDouble_t yield = fBeamNorm->CalculateYield(whichSb, type, print);
  UDouble_t wavelength = CalculateWavelength(fResonanceEnergy, fBeamMass, fTargetMass);
  UDouble_t wg = CalculateResonanceStrength(yield, fEpsilon, wavelength, fBeamMass, fTargetMass);
  if(print) std::cout << "Resonance Strength [eV]: " << wg << "\n";
  return wg;
}

TGraph* dragon::ResonanceStrengthCalculator::PlotResonanceStrength(Int_t whichSb)
{
  if((unsigned)whichSb > 1) {
    dutils::Error("ResonanceStrengthCalculator::PlotResonanceStrength", __FILE__, __LINE__)
      << "invalid SB: " << whichSb << ", valid are 0 or 1";
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

  Double_t mu = 1e6*dragon::Constants::AMU()*mbeam*mtarget / (mbeam + mtarget); // reduced mass in eV/c^2
  eres *= 1e3; // keV -> eV
  UDouble_t pc = UDouble_t::sqrt(eres*eres + 2.*eres*mu);

  // Double_t hc = TMath::HC() * 1e9 / TMath::Qe(); // eV*nm
  Double_t hc = 2*TMath::Pi()*dragon::Constants::HbarC()*1e6; // eV*fm
  UDouble_t lambda = hc / pc; // nm
  lambda /= 1e14; // cm
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



// ============ class dragon::CrossSectionCalculator ============ //

dragon::CrossSectionCalculator::CrossSectionCalculator(dragon::BeamNorm* beamNorm, Int_t nmol, Double_t temp, Double_t targetLen):
  fBeamNorm(beamNorm),
  fNmol(nmol),
  fTemp(temp),
  fTargetLen(targetLen)
{
  ///
  /// \param beamNorm Pointer to a composed dragon::BeamNorm class which has had
  ///  BatchCalculate() run for the desired set of runs.
  /// \param nmol Number of atoms per molecule of the target (2 for hydrogen, 1 for helium).
  /// \param temp Target cell temperature in kelvin
  /// \param targtLen Effective target length in cm
  ///
  fTotalCrossSection = UDouble_t(0, 0);
}

UDouble_t dragon::CrossSectionCalculator::Calculate(Int_t whichSB, const char* prefix, Bool_t printTotal)
{
  ///
  /// \param whichSB for the normalization (0 == sb0, 1 == sb1).
  /// \param prefix Desired metrix prefix for the output unit (default = "" is barns, "m" is millibarns, etc.)
  /// \param printTotal If true, prints the total cross section to the screen.
  ///
  /// \note This function calculates the run-by-run cross sections for each run that was part of the BatchCalculate()
  ///  call to the BeamNorm pointer passed to the constructor. The total cross section is the weighted average of these
  ///  run-by-run measurements.
  ///

  if(!fBeamNorm) {
    dragon::utils::Error("CalculateCrossSection",__FILE__,__LINE__) << "fBeamNorm == 0";
    return 0;
  }
  if((UInt_t)whichSB > 1) {
    dragon::utils::Error("CalculateCrossSection",__FILE__,__LINE__)
      << "Invalid SB: " << whichSB << ", must be 0 or 1.";
    return 0;
  }

  fPrefix = prefix;
  Double_t prefixval = dragon::MetricPrefix::Get(prefix);
  if(prefixval == 0) {
    dragon::utils::Warning("CalculateCrossSection",__FILE__,__LINE__)
      << "Invalid prefix: \"" << prefix << "\", defaulting to barns for the unit.";
    prefixval = 1.;
    fPrefix = "";
  }

  std::vector<Double_t> cx, cxerror;
  std::vector<Int_t>::iterator irun = fBeamNorm->GetRuns().begin();

  while(irun !=  fBeamNorm->GetRuns().end()) {
    dragon::BeamNorm::RunData* rd = fBeamNorm->GetRunData(*irun++);
    if(rd == 0) break;

    const UDouble_t sigma = CalculateCrossSection(rd->yield[whichSB], rd->pressure, fTargetLen, fNmol, fTemp) / prefixval;
    fCrossSections.push_back( sigma );
  }
  fTotalCrossSection = MeasurementWeightedAverage(fCrossSections.begin(), fCrossSections.end());

  if(printTotal) {
    std::cout << "Total cross section: " << fTotalCrossSection << " " << fPrefix << "b.\n";
  }

  return fTotalCrossSection;
}

UDouble_t dragon::CrossSectionCalculator::CalculateCrossSection(UDouble_t yield, UDouble_t pressure, Double_t length, Int_t nmol, Double_t temp)
{
  UDouble_t rho = dragon::StoppingPowerCalculator::CalculateDensity(pressure, length, nmol, temp);
  UDouble_t sigma = yield / rho; // cm^2
  sigma /= 1e-24; // barns
  return sigma;
}

namespace {
  inline std::vector<Double_t> get_runs(dragon::BeamNorm* fBeamNorm)
  {
	std::vector<Double_t> runs;
	std::vector<Int_t>::iterator irun = fBeamNorm->GetRuns().begin();
	while(irun != fBeamNorm->GetRuns().end()) {
      runs.push_back(*irun++);
	}
	return runs;
  }
}

TGraph* dragon::CrossSectionCalculator::Plot(Marker_t marker, Color_t color)
{
  if(fBeamNorm == 0) return 0;
  Int_t npoints = fCrossSections.size();
  if(npoints == 0) return 0;

  std::vector<Double_t> runs = get_runs(fBeamNorm);
  if(runs.size() != fCrossSections.size()) {
    utils::Error("Print",__FILE__,__LINE__) << "runs.size() != fCrossSections.size()";
    return 0;
  }
  TGraph* gr = PlotUncertainties(npoints, &runs[0], &fCrossSections[0]);
  if(gr) {
    gr->SetMarkerStyle(marker);
    gr->SetMarkerColor(color);
    gr->Draw("AP");
  }
  return gr;
}


void dragon::CrossSectionCalculator::Print()
{
  std::vector<Double_t> runs = get_runs(fBeamNorm);
  if(runs.size() != fCrossSections.size()) {
    utils::Error("Print",__FILE__,__LINE__) << "runs.size() != fCrossSections.size()";
    return;
  }
  std::cout << "Run\tCross section [" << fPrefix << "b]\n";
  for (size_t i=0; i< runs.size(); ++i) {
    std::cout << runs[i] << "\t" << fCrossSections[i] << "\n";
  }
}
