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
#include <cassert>
#include <algorithm>
#include <iostream>
#include <TTree.h>
#include <TFile.h>
#include <TError.h>
#include <TString.h>
#include <TSystem.h>
#include "midas/libMidasInterface/TMidasFile.h"
#include "midas/Database.hxx"
#include "utils/definitions.h"
#include "utils/Unpack.hxx"
#include "Dragon.hxx"



namespace {
bool arg_return = false;
const char* const msg_use = 
	"usage: mid2root <input file> [-o <output file>] [-v <xml odb>] [--singles] [--overwrite] [--quiet <n>] [--help]\n";
}

//
/// Encloses mid2root helper functions
namespace m2r {

//
// SKip documenting a bunch of internal stuff
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
	bool fOverwrite;
	bool fSingles;
	Options_t(): fOverwrite(false), fSingles(false) {}
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
		"\t--singles:        Unpack in singles mode. This means that every head and tail event is analyzed as a singles\n"
		"                    event only. In this mode, the buffering in a queue and timestamp matching routines are\n"
		"                    skipped completely.\n"
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
		else if (*iarg == "--singles") { // Singles mode
			options->fSingles = true;
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

} // namespace mid2root

//
/// The main function
int main(int argc, char** argv)
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
	const int nIds = 7;

	dragon::Head head;
	dragon::Tail tail;
	dragon::Coinc coinc;
	dragon::Scaler head_scaler;
	dragon::Scaler tail_scaler;
	dragon::RunParameters runpar;
	tstamp::Diagnostics tsdiag;

	const int eventIds[nIds] = {
		DRAGON_HEAD_EVENT,
		DRAGON_HEAD_SCALER,
		DRAGON_TAIL_EVENT,
		DRAGON_TAIL_SCALER,
		DRAGON_COINC_EVENT,
		DRAGON_TSTAMP_DIAGNOSTICS,
		DRAGON_RUN_PARAMETERS
	};
	const std::string eventTitles[nIds] = {
		"Head singles event.",
		"Head scaler event.",
		"Tail singles event.",
		"Tail scaler event.",
		"Coincidence event.",
		"Timestamp diagnostics.",
		"Global run parameters."
	};
	void* addr[nIds] = {
		&head,
		&head_scaler,
		&tail,
		&tail_scaler,
		&coinc,
		&tsdiag,
		&runpar
	};
	const std::string branchNames[nIds] = {
		"gamma",
		"sch",
		"hi",
		"sct",
		"coinc",
		"tsdiag",
		"runpar"
	};
	const std::string classNames[nIds] = {
		"dragon::Head",
		"dragon::Scaler",
		"dragon::Tail",
		"dragon::Scaler",
		"dragon::Coinc",
		"tstamp::Diagnostics",
		"dragon::RunParameters"
	};

	TTree* trees[nIds];
	for(int i=0; i< nIds; ++i) {
		char buf[256];
		sprintf (buf, "t%d", eventIds[i]); // n.b. TTree cleanup handled by TFile destructor
		trees[i] = new TTree(buf, eventTitles[i].c_str());
		trees[i]->Branch(branchNames[i].c_str(), classNames[i].c_str(), &(addr[i]));
	}

	dragon::utils::Unpacker
		unpack (&head, &tail, &coinc, &head_scaler, &tail_scaler, &runpar, &tsdiag, options.fSingles);
	
	//
	// Set coincidence variables
	if(!options.fSingles) {
		bool coincSuccess;
		double coincWindow, queueTime;
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
	// Loop over events in the midas file
	int nnn = 0;
	while (1) {
		//
		// Read event from MIDAS file
		TMidasEvent temp;
		bool success = fin.Read(&temp);
		if (!success) break;

		//
		// Unpack into our classes
		std::vector<Int_t> which =
			unpack.UnpackMidasEvent(temp.GetEventHeader(), temp.GetData());

		//
		// Check which classes have data, fill trees for those that do
		for (int i=0; i< nIds; ++i) {
			std::vector<Int_t>::iterator it =
				std::find(which.begin(), which.end(), eventIds[i]);
			if(it != which.end())
				trees[i]->Fill();
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
				if(it != which.end())
					trees[i]->Fill();
			}			
		} 
	}

	m2r::cout << "\nDone!\n\n";

	//
	// Write trees, variables to file, cleanup
	for (int i=0; i< nIds; ++i) {
		trees[i]->GetCurrentFile();
		trees[i]->AutoSave();
		trees[i]->ResetBranchAddresses();
	}

	midas::Database db(options.fOdb.c_str());
	db.Write("variables");

	fout.Close();
}



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
