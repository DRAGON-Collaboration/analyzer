///
/// \file mid2root.cxx
/// \author G. Christian
/// \brief Implements the main() function for a simple program
///  to unpack midas files (*.mid) into ROOT trees saved in a
///  root file.
///
#ifdef USE_ROOT
#include <vector>
#include <string>
#include <memory>
#include <cassert>
#include <algorithm>
#include <iostream>
#include <TTree.h>
#include <TFile.h>
#include <TROOT.h>
#include <TError.h>
#include <TString.h>
#include <TSystem.h>
#include "midas/libMidasInterface/TMidasFile.h"
#include "midas/Database.hxx"
#include "utils/definitions.h"
#include "Unpack.hxx"
#include "Dragon.hxx"
#include "Sonik.hxx"


#ifndef DOXYGEN_SKIP
namespace {
  bool arg_return = false;
  const char* const msg_use =
	"usage: mid2root <input file> [-o <output file>] [-v <xml odb>] [-histos <*.xml> ] "
	"[--singles] [--overwrite] [--quiet <n>] [--help]\n";
}

//
// Forward declaration of rootbeer function to parse histogram file
#ifdef USE_ROOTBEER
namespace rb { void ReadHistXML(const char* filename, Option_t* option = "r"); }
#endif
#endif


//
/// Encloses mid2root helper functions
namespace m2r {

  //
  // Skip documenting a bunch of internal stuff
#ifndef DOXYGEN_SKIP

  typedef std::vector<std::string> strvector_t;

  //
  // Error handling stuff
  std::ostream cnul (0);

  class strm_ref {
  public:
	strm_ref(std::ostream& strm):
      pstrm(&strm) {}
	strm_ref():
      pstrm(&cnul) {}
	strm_ref& operator= (std::ostream& strm)
    { pstrm = &strm; return *this; }
	template <class T>
	std::ostream& operator<< (const T& t)
    { return *pstrm << t; }
	std::ostream& flush()
    { return std::flush(*pstrm); }
  private:
	std::ostream* pstrm;
  };

  inline std::ostream& flush(strm_ref& strm)
  { return strm.flush(); }

  strm_ref cout = std::cout;
  strm_ref cwar = std::cerr;
  strm_ref cerr = std::cerr;

  void SetQuietLevel(int level)
  {
	if (level < 1) {
      gErrorIgnoreLevel = 0;
      m2r::cout = std::cout;
      m2r::cwar = std::cerr;
      m2r::cerr = std::cerr;
	}
	else if (level == 1) {
      gErrorIgnoreLevel = 1001;
      m2r::cout = m2r::cnul;
      m2r::cwar = std::cerr;
      m2r::cerr = std::cerr;
	}
	else if (level == 2) {
      gErrorIgnoreLevel = 2001;
      m2r::cout = m2r::cnul;
      m2r::cwar = m2r::cnul;
      m2r::cerr = std::cerr;
	}
	else {
      gErrorIgnoreLevel = level*1000 + 1;
      m2r::cout = m2r::cnul;
      m2r::cwar = m2r::cnul;
      m2r::cerr = m2r::cnul;
	}
  }

  int GetQuietLevel()
  {
	for (int i = 3; i > 0; --i) {
      if (gErrorIgnoreLevel - i*1000 > 0)
        return i;
	}
	return 0;
  }

#endif

  /// Program options
  struct Options_t {
	std::string fIn;
	std::string fOut;
	std::string fOdb;
	std::string fHistos;
	bool fOverwrite;
	bool fSingles;
	bool fSonik;
	Options_t(): fOverwrite(false), fSingles(false), fSonik(false) {}
  };


  /// Print in-place event counter
  void static_counter(Int_t n, Int_t nupdate = 1000, bool force = false)
  {
	if(n == 0) {
      m2r::cout << "Events converted: ";
      m2r::flush(m2r::cout);
	}
	if(n % nupdate != 0 && !force) return;

	static int nOut = 0;
	std::stringstream out; out << n;
	if(nOut) {
      for(int i=0; i< nOut; ++i)
        m2r::cout << "\b";
	}
	nOut = out.str().size();
	m2r::cout << n;
	m2r::flush(m2r::cout);
  }

  /// Fill histograms
  void fill_histos(int eventCode, void*);

  /// Save histograms
  void save_histos(TDirectory*, TDirectory*);

  /// Read histograms from xml file
  void read_histos(const std::string&);

  /// Print a usage message
  int usage(const char* what = 0)
  {
	const char* msg =
      "Run 'mid2root --help' for more information.\n";
	m2r::cerr << msg_use << msg << std::endl;
	if (what) m2r::cerr << "Error: " << what << ".\n";
	arg_return = true;
	return 1;
  }

  /// Print a help message
  int help()
  {
	m2r::cerr
      << "\nmid2root: A program to convert DRAGON experiment data from MIDAS format into ROOT Trees.\n" << msg_use;

	const char* msg_args =
      "\n"
      "Program arguments:\n"
      "\n"
      "\t<input file>:     Specifies the MIDAS file to convert [required].\n"
      "\n"
      "\t-o <output file>: Specify the output file. If not set, the output file\n"
      "\t                  will have the same name as the input file, but with the extension\n"
      "\t                  converted to \'.root\'. If the environment variable $DH is set, and\n"
      "\t                  the directory $DH/rootfiles exists, the default output is written to\n"
      "\t                  that directory. Otherwise, it is written to the present working directory.\n"
      "\n"
      "\t-v <xml odb>:     Specify an XML file containing the full '/dragon' ODB tree, which defines all\n"
      "\t                  variables to be used in the program. Default is to take variable values from the\n"
      "\t                  ODB dump of the input MIDAS file (i.e. variables will reflect the state of the ODB\n"
      "\t                  when the run was taken).\n"
      "\n"
      "\t-histos <*.xml>:  Specify an XML file denoting histograms to fill and save during the unpacking.\n"
      "\t                  The XML file format should be the same as those created by ROOTBEER. If the DRAGON\n"
      "\t                  package was compiled with USE_ROOTBEER turned off, then this option is not available.\n"
      "\t                  In case it is specified but not available, the program will terminate with an error message.\n"
      "\n"
      "\t--sonik:          Unpack in \"SONIK\" mode. Treat tail data as if coming from the SONIK scattering detectors,\n"
      "\t                  rather than from the DRAGON end detectors.\n"
      "\n"
      "\t--singles:        Unpack in singles mode. This means that every head and tail event is analyzed as a singles\n"
      "\t                  event only. In this mode, the buffering in a queue and timestamp matching routines are\n"
      "\t                  skipped completely.\n"
      "\n"
      "\t--overwrite:      Overwrite any existing output files without asking the user.\n"
      "\n"
      "\t--quiet <n>:      Suppress program output messages. Followed by a numeral specifying the level of\n"
      "\t                  quietness: 1 suppresses only informational messages, 2 supresses information and\n"
      "\t                  warnings, and >=3 suppresses all output (including errors). The default setting is 0,\n"
      "\t                  which prints all messages.\n"
      "\n"
      "\t--help:           Print this help message and exit.\n"
      ;
	m2r::cerr << msg_args << std::endl;
	arg_return = true;
	return 0;
  }


  /// Parse command line arguments
  int process_args(int argc, char** argv, Options_t* options)
  {
	strvector_t args(argv+1, argv+argc);
	strvector_t::iterator iarg = args.begin();

	//
	// Look for input file
	for(; iarg != args.end(); ++iarg) {
      if(iarg->substr(0, 2) == "--")
        continue;
      if((iarg-1 >= args.begin()) && *(iarg-1) == "--quiet")
        continue;
      options->fIn = *iarg;
      break;
	}

	//
	// Check other arguments
	for(iarg = args.begin(); iarg != args.end(); ++iarg) {
      if (*iarg == options->fIn) { // Input file, already set
        continue;
      }
      else if (*iarg == "--help") { // Help message
        return help();
      }
      else if (*iarg == "-o") { // Output file
        if (++iarg == args.end()) return usage("output file not specified");
        options->fOut = *iarg;
      }
      else if (*iarg == "-v") { // Variables file
        if (++iarg == args.end()) return usage("variables file not specified");
        options->fOdb = *iarg;
      }
      else if (*iarg == "-histos") { // Histograms file
#ifndef USE_ROOTBEER
        return usage("histogram unpacking only available if compiled with USE_ROOTBEER=YES");
#endif
        if (++iarg == args.end()) return usage("histogram xml file not specified");
        options->fHistos = *iarg;
      }
      else if (*iarg == "--singles") { // Singles mode
        options->fSingles = true;
      }
      else if (*iarg == "--sonik") { // SONIK mode
        options->fSonik = true;
      }
      else if (*iarg == "--overwrite") { // Overwrite flag
        options->fOverwrite = true;
      }
      else if (*iarg == "--quiet") { // Quiet flag
        if (++iarg == args.end()) return usage("quietness level not specified");
        TString qstr = iarg->c_str();
        if (qstr.IsDigit() == false) {
          TString error ("Quietness level \'");
          error += qstr; error += "\' is not an integer";
          return usage(error.Data());
        }
        m2r::SetQuietLevel(qstr.Atoi());
      }
      else { // Unknown flag
        TString what = "unknown flag \'";
        what += *iarg; what += "\'";
        usage(what.Data());
        return 1;
      }
	}

	if (options->fIn.empty()) // Didn't find input file
      return usage("no input file specified");

	return 0;
  }

  //
  /// The main function implementation
  int main_(int argc, char** argv)
  {
	m2r::Options_t options;
	int arg_result = m2r::process_args(argc, argv, &options);
	if(arg_return) return arg_result;

	//
	// Open input file
	TMidasFile fin;
	if (fin.Open(options.fIn.c_str()) == false) {
      m2r::cerr
        << "Error: Couldn't open the file \'" << options.fIn
        << "\': \"" << fin.GetLastError() << ".\"\n\n";
      return 1;
	}

	//
	// Get output file name
	TString out = options.fOut.empty() ? options.fIn.c_str() : options.fOut.c_str();
	//
	// If no output specified, create file name from input
	if (options.fOut.empty()) {
      //
      // Strip '.mid' extension if it's there
      if(out.Contains(".mid"))
        out.Remove(out.Index(".mid"));
      //
      // Add '.root' extension
      out.Append(".root");
      //
      // Strip input directory
      out = gSystem->BaseName(out.Data());
      //
      // Check for $DH/rootfiles
      TString outdir = "$DH";
      Int_t gel = gErrorIgnoreLevel;
      gErrorIgnoreLevel = 3001; // Suppress error if $DH isn't available
      Bool_t have_DH = !(gSystem->ExpandPathName(outdir));
      gErrorIgnoreLevel = gel; // reset error level

    dhcheck:
      if (have_DH) { // Check for $DH/rootfiles
        outdir += "/rootfiles";
        void *d = gSystem->OpenDirectory(outdir.Data());
        if (d) // Directory exists, need to free it
          gSystem->FreeDirectory(d);
        else { // Doesn't exist, set path back to "."
          have_DH = false;
          goto dhcheck;
        }
      }
      else outdir = ".";

      gSystem->PrependPathName(outdir.Data(), out);
	} // if (options.fOut.empty()) {

	//
	// Handle odb variables file
	if (options.fOdb.empty()) {
      options.fOdb = options.fIn; // No file specified, use ODB dump in midas file
	}
	else { // Check if it exists
      FileStat_t dummy;
      if(gSystem->GetPathInfo(options.fOdb.c_str(), dummy) != 0) { // no file
        m2r::cerr
          << "Error: The specified variables file \'" << options.fOdb
          << "\' does not exist.\n\n";
        return 1;
      }
	}

	//
	// Open output TFile
	std::string ftitle;
	{
      midas::Database db(options.fOdb.c_str());
      bool success = db.ReadValue("/Experiment/Run Parameters/Comment", ftitle);
      if(!success) {
        m2r::cerr << "Error: Invalid database file \"" << options.fOdb << "\".\n\n";
        return 1;
      }
	}
	if (options.fOverwrite == false) {
      FileStat_t dummy;
      if(gSystem->GetPathInfo(out.Data(), dummy) == 0) { // file exists
        m2r::cerr << "\noverwrite " << out.Data() << "? (y/n [n])\n";
        std::string answer;
        answer = std::cin.get();
        if (answer.substr(0, 1) == "y" || answer.substr(0, 1) == "Y")
          ;
        else {
          m2r::cerr << "Exiting.\n";
          return 0;
        }
      }
	}

	//
	// Open histos file if specified
	bool fillHistos = false;
	if(!options.fHistos.empty()) {
      fillHistos = true;
      read_histos(options.fHistos);
	}

	m2r::cout
      << "\nConverting MIDAS file\n\t\'" << options.fIn << "\'\n"
      << "into ROOT file\n\t\'" << out.Data() << "\'\n";

	TFile fout (out.Data(), "RECREATE", ftitle.c_str());
	if (fout.IsZombie()) {
      m2r::cerr << "Error: Couldn't open the file \'" << out.Data()
                << "\' for writing.\n\n";
      return 1;
	}

	//
	// Create TTrees, set branches, etc.
	const int nIds = 9;

	dragon::Head head;
	dragon::Tail tail;
	dragon::Coinc coinc;
	dragon::Epics epics;
	dragon::Scaler head_scaler;
	dragon::Scaler tail_scaler;
	dragon::Scaler aux_scaler;
	dragon::RunParameters runpar;
	tstamp::Diagnostics tsdiag;
	Sonik sonik;

	const int eventIds[nIds] = {
                                DRAGON_HEAD_EVENT,
                                DRAGON_HEAD_SCALER,
                                DRAGON_TAIL_EVENT,
                                DRAGON_TAIL_SCALER,
                                DRAGON_COINC_EVENT,
                                DRAGON_AUX_SCALER,
                                DRAGON_EPICS_EVENT,
                                DRAGON_TSTAMP_DIAGNOSTICS,
                                DRAGON_RUN_PARAMETERS
	};
	const std::string eventTitles[nIds] = {
                                           "Head singles event.",
                                           "Head scaler event.",
                                           "Tail singles event.",
                                           "Tail scaler event.",
                                           "Coincidence event.",
                                           "Aux scaler event.",
                                           "Epics event.",
                                           "Timestamp diagnostics.",
                                           "Global run parameters."
	};
	void* addr[nIds] = {
                        &head,
                        &head_scaler,
                        &tail,
                        &tail_scaler,
                        &coinc,
                        &aux_scaler,
                        &epics,
                        &tsdiag,
                        &runpar
	};
	void *psonik = &sonik;

	const std::string branchNames[nIds] = {
                                           "head",
                                           "sch",
                                           "tail",
                                           "sct",
                                           "coinc",
                                           "scx",
                                           "epics",
                                           "tsdiag",
                                           "runpar"
	};
	const std::string classNames[nIds] = {
                                          "dragon::Head",
                                          "dragon::Scaler",
                                          "dragon::Tail",
                                          "dragon::Scaler",
                                          "dragon::Coinc",
                                          "dragon::Scaler",
                                          "dragon::Epics",
                                          "tstamp::Diagnostics",
                                          "dragon::RunParameters"
	};

	// SONIK Tree
	TTree* trees[nIds];
	TTree* t0 = 0;
	if(options.fSonik) {
      t0 = new TTree("t0", "Sonik Events");
      t0->Branch("sonik", "Sonik", &psonik);
	}

	// Normal trees
	for(int i=0; i< nIds; ++i) {
      char buf[256];
      sprintf (buf, "t%d", eventIds[i]); // n.b. TTree cleanup handled by TFile destructor
      //
      bool makeTree = true; // always make all trees
      if (makeTree) {
        trees[i] = new TTree(buf, eventTitles[i].c_str());
        trees[i]->Branch(branchNames[i].c_str(), classNames[i].c_str(), &(addr[i]));
      } else {
        trees [i] = 0;
      }
	}

	dragon::Unpacker
      unpack (&head, &tail, &coinc, &epics, &head_scaler, &tail_scaler, &aux_scaler, &runpar, &tsdiag, options.fSingles);

	//
	// Set coincidence variables
	if(!options.fSingles) {
      bool coincSuccess;
      double coincWindow = 10, queueTime = 4;
      {
        midas::Database db (options.fOdb.c_str());
        coincSuccess = db.ReadValue("/dragon/coinc/variables/window", coincWindow);
        if (coincSuccess)
          coincSuccess = db.ReadValue("/dragon/coinc/variables/buffer_time", queueTime);
      }
      if (coincSuccess) {
        unpack.SetCoincWindow(coincWindow);
        unpack.SetQueueTime(queueTime);
      }
      m2r::cout
        << "\nUnpacker parameters: coincidence window = " << unpack.GetCoincWindow() << " usec., "
        << "queue time = " << unpack.GetQueueTime() << " sec.\n\n";
	}
	else {
      m2r::cout << "\nRunning in singles mode.\n\n";
	}

	//
	// Begin-of-run initialization
	unpack.HandleBor(options.fOdb.c_str());

	//
	// ODB parameters
	std::auto_ptr<midas::Database> db0(0); // run start
	std::auto_ptr<midas::Database> db1(0); // run stop

	//
	// Loop over events in the midas file
	int nnn = 0;
	while (1) {
      //
      // Read event from MIDAS file
      TMidasEvent temp;
      bool success = fin.Read(&temp);
      if (!success) break;

      //
      // Read ODB tree if MIDAS_EOR buffer
      if (temp.GetEventId() == MIDAS_BOR) {
        db0.reset(new midas::Database(temp.GetData(), temp.GetDataSize()));
      }
      else if (temp.GetEventId() == MIDAS_EOR) {
        db1.reset(new midas::Database(temp.GetData(), temp.GetDataSize()));
      }

      //
      // Unpack into our classes
      std::vector<Int_t> which =
        unpack.UnpackMidasEvent(temp.GetEventHeader(), temp.GetData());

      //
      // Check which classes have data, fill trees for those that do
      // Also fill histograms if appropriate
      for (int i=0; i< nIds; ++i) {
        std::vector<Int_t>::iterator it =
          std::find(which.begin(), which.end(), eventIds[i]);
        if(it != which.end()) {
          if(trees[i]) {
            trees[i]->Fill();
          }
          if(fillHistos) fill_histos(*it, addr[i]);
          if(options.fSonik && eventIds[i] == DRAGON_TAIL_EVENT) {
            sonik.reset();
            sonik.read_data(tail.v785, tail.v1190);
            sonik.calculate();
            t0->Fill();
            if(fillHistos) fill_histos(0, psonik);
          }
        }
      }
      m2r::static_counter (nnn++, 1000, false);
	} // while (1) {

	m2r::static_counter (nnn, 1000, true);

	if(!options.fSingles) { // Flush the queue
      size_t qsize;
      while (1) {
        qsize = unpack.FlushQueueIterative(); // Fills classes implicitly
        if (qsize == 0) break;

        // Explicitly fill TTrees
        std::vector<Int_t> which = unpack.GetUnpackedCodes();
        for (int i=0; i< nIds; ++i) {
          std::vector<Int_t>::iterator it =
            std::find(which.begin(), which.end(), eventIds[i]);
          if(it != which.end()) {
            if(trees[i]) {
              trees[i]->Fill();
            }
            if(options.fSonik && eventIds[i] == DRAGON_TAIL_EVENT) {
              sonik.reset();
              sonik.read_data(tail.v785, tail.v1190);
              sonik.calculate();
              t0->Fill();
            }
          }
        }
      }
	}

	m2r::cout << "\nDone!\n\n";

	//
	// Write trees to file.
	if(options.fSonik && t0) {
      t0->GetCurrentFile();
      t0->AutoSave();
      t0->ResetBranchAddresses();
	}
	for (int i=0; i< nIds; ++i) {
      if(trees[i]) {
        trees[i]->GetCurrentFile();
        trees[i]->AutoSave();
        trees[i]->ResetBranchAddresses();
      }
	}
	//
	// Write histograms to file if requested
	if(fillHistos) {
      save_histos(gROOT, &fout);
      fout.cd();
	}
	//
	// Write run start ODB variables
	if(db0.get()) {
      db0->SetNameTitle("odbstart", "ODB tree at run start.");
      db0->Write("odbstart");
	}
	//
	// Write run stop ODB variables
	if(db1.get()) {
      db1->SetNameTitle("odbstop", "ODB tree at run stop.");
      db1->Write("odbstop");

      std::string ftitle;
      bool success = db1->ReadValue("/Experiment/Run Parameters/Comment", ftitle);
      if(success) fout.SetTitle(ftitle.c_str());
	}
	//
	// Write variables actually used in analysis
	midas::Database db(options.fOdb.c_str());
	db.SetNameTitle("variables", "ODB tree used in analysis.");
	db.Write("variables");
	//
	// Print delayed error messages
	dragon::utils::gDelayedMessageFactory.Flush();
	//
	// Close output file
	fout.Close();
	return 0;
  }

} // namespace m2r

#ifndef USE_ROOTBEER

//
// Dummy implementation for fill_histos() and save_histos()
void m2r::fill_histos(int, void*) { assert("Can't get here!"); }
void m2r::read_histos(const std::string&) { assert("Can't get here!"); }
void m2r::save_histos(TDirectory*, TDirectory*) { assert("Can't get here!"); }

//
// No rootbeer, just call m2r::main_()
int main(int argc, char** argv)
{
  return m2r::main_(argc, argv);
}

#else
#ifndef DOXYGEN_SKIP
//
// Rootbeer inplementation for histogram parsing stuff.
// We are just using rootbeer for parsing histogram files
// but need to "fake" some of the framework to get the histograms
// to be created and filled.

//
// Include necessary rootbeer headers
#include "Main.hxx"
#include "Rint.hxx"
#include "Event.hxx"
#include "Data.hxx"

namespace m2r {
  //
  // Rootbeer library defines it's own main(), users provide implementation
  // via a class inherited from rb::Main and by imelementing rb::GetMain()
  class Main: public rb::Main {
  public:
	int Run(int argc, char** argv)
    {
      int argc2 = 3;
      char* argv2[] = { (char*)"mid2root", (char*)"-ng", (char*)"--quiet" };
      rb::Rint rbApp("Rbunpack", &argc2, argv2, 0, 0, true);
      gSystem->ResetSignals();
      int retval = m2r::main_(argc, argv);
      rbApp.Terminate(retval);
      return retval;
    }
  }; }

//
// Return m2r::Main
rb::Main* rb::GetMain() { return new m2r::Main(); }


namespace m2r {

  //
  // Rootbeer associates histograms with a data address using
  // classes derived from rb::Event, which contain a wrapper
  // to classes holding user data. However, here we don't want
  // to use all of this, so we can fake it by just copying
  // the data from `head`, `tail`, etc that are defined in main_().
  // To make this generic we can use some template and inheritance
  // tricks. First define an abstract with a `SetData()` function
  // to copy data.
  class AEvent: public rb::Event {
  public:
	virtual void SetData(const void* addr) = 0;
  };

  //
  // Then define a template derived class that fills in all of the
  // implementations generically.
  template <class T, const char* STR, bool B>
  class EventTemplate: public AEvent {
  protected:
	rb::data::Wrapper<T> fWrapper; // data wrapper - what we copy to
  public:
	//
	// Initialize data wrapper with template arguments
	EventTemplate(): fWrapper(STR, this, B, "") { }
	//
	// Copy data at an address to the data wrapper
	void SetData(const void* addr) { *fWrapper = *reinterpret_cast<const T*>(addr); }
  private:
	//
	// Required pure virtual functions - implement with nothing
	Bool_t DoProcess(const void*, Int_t) { return true; }
	void HandleBadEvent() { }
  };

  //
  // Have to define string literals as global char arrays to use them as template arguments
  char gStrHead[] = "head", gStrTail[] = "tail", gStrCoinc[] = "coinc",
    gStrScalerH[] = "head_scaler", gStrScalerT[] = "tail_scaler", gStrScalerX[] = "aux_scaler",
    gStrEpics[] = "epics", gStrTS[] = "tstamp", gStrRP[] = "runpar", gStrSonik[] = "sonik";

  //
  // Now derive from EventTemplate<> with the appropriate template arguments
  // for each type of event
  class HeadEvent:  public EventTemplate<dragon::Head,   gStrHead,    false> { };
  class TailEvent:  public EventTemplate<dragon::Tail,   gStrTail,    false> { };
  class CoincEvent: public EventTemplate<dragon::Coinc,  gStrCoinc,   false> { };
  class EpicsEvent: public EventTemplate<dragon::Epics,  gStrEpics,   false> { };
  class HeadScaler: public EventTemplate<dragon::Scaler, gStrScalerH, false> { };
  class TailScaler: public EventTemplate<dragon::Scaler, gStrScalerT, false> { };
  class AuxScaler:  public EventTemplate<dragon::Scaler, gStrScalerX, false> { };
  class TStampDiagnostics: public EventTemplate<tstamp::Diagnostics,   gStrTS, false> { };
  class RunParameters:     public EventTemplate<dragon::RunParameters, gStrRP, false> { };
  class SonikEvent: public EventTemplate<Sonik, gStrSonik, false> { };

} // namespace m2r


//
// Implement rootbeer RegisterEvents() function with the classes
// defined above
void rb::Rint::RegisterEvents()
{
  RegisterEvent<m2r::SonikEvent> (0,                  "SonikEvent");
  RegisterEvent<m2r::HeadEvent>  (DRAGON_HEAD_EVENT,   "HeadEvent");
  RegisterEvent<m2r::TailEvent>  (DRAGON_TAIL_EVENT,   "TailEvent");
  RegisterEvent<m2r::CoincEvent> (DRAGON_COINC_EVENT, "CoincEvent");
  RegisterEvent<m2r::EpicsEvent> (DRAGON_EPICS_EVENT, "EpicsEvent");
  RegisterEvent<m2r::HeadScaler> (DRAGON_HEAD_SCALER, "HeadScaler");
  RegisterEvent<m2r::TailScaler> (DRAGON_TAIL_SCALER, "TailScaler");
  RegisterEvent<m2r::AuxScaler>  (DRAGON_AUX_SCALER,  "AuxScaler");
  RegisterEvent<m2r::TStampDiagnostics>    (6, "TStampDiagnostics");
  RegisterEvent<m2r::RunParameters>        (7,     "RunParameters");
}

//
// Now implement the functions dealing w/ histogram read write, fill
void m2r::read_histos(const std::string& fname)
{
  rb::ReadHistXML(fname.c_str(), "o");
}

void m2r::fill_histos(int eventCode, void* addr)
{
  m2r::AEvent* event = dynamic_cast<m2r::AEvent*>(rb::Rint::gApp()->GetEvent(eventCode));
  if(!event) return;

  event->SetData(addr);
  event->GetHistManager()->FillAll();
}

void m2r::save_histos(TDirectory* dir, TDirectory* newdir)
{
  for(Int_t i=0; i< dir->GetList()->GetEntries(); ++i) {
    newdir->cd();
    TObject* obj = dir->GetList()->At(i);
    if(obj->InheritsFrom(TDirectory::Class())) {
      TDirectory* newdir1 = gDirectory->mkdir(obj->GetName(), obj->GetTitle());
      save_histos(static_cast<TDirectory*>(obj), static_cast<TDirectory*>(newdir1));
    }
    else if(obj->InheritsFrom(rb::hist::Base::Class())) {
      static_cast<rb::hist::Base*>(obj)->GetHist()->Write(obj->GetName());
    }
  }
}

#endif // DOXYGEN_SKIP
#endif // USE_ROOTBEER

#else // #ifdef USE_ROOT
#include <iostream>

int main(int argc, char** argv)
{
  const char* msg =
    "\nError: ROOT is not available on ths system. Please visit\n"
    "    http://root.cern.ch/drupal/content/downloading-root\n"
    "for instructions on how to install ROOT. Once it is set up, re-\n"
    "compile the DRAGON analyzer with USE_ROOT=YES set in the Makefile."
    "\n\n";

  std::cerr << msg;
  return 0;
}

#endif
