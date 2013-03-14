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
#include <iostream>
#include <TTree.h>
#include <TFile.h>
#include <TError.h>
#include <TString.h>
#include <TSystem.h>
#include "midas/libMidasInterface/TMidasFile.h"
#include "Dragon.hxx"



namespace {
bool arg_return = false;
const char* const msg_use = 
	"usage: mid2root <input file> [-o <output file>] [-v <xml odb>] [--overwrite] [--quiet <n>] [--help]\n";
}

//
/// Encloses mid2root helper functions
namespace m2r {

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
private:
	std::ostream* pstrm;
};

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

struct Options_t {
	std::string fIn;
	std::string fOut;
	std::string fOdb;
	bool fOverwrite;
	Options_t(): fOverwrite(false) {}
};

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
	} // if (options.fOut.empty())

	//
	// Open output TFile
	if (options.fOverwrite == false) {
		FileStat_t dummy;
		if(gSystem->GetPathInfo(out.Data(), dummy) == 0) { // file exists
			m2r::cerr << "\nThe file " << out.Data() << " already exists. Overwrite (y/[n])?\n";
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

	TFile fout (out.Data(), "RECREATE");
	if (fout.IsZombie()) {
		m2r::cerr << "Error: Couldn't open the file \'" << out.Data()
							<< "\' for writing.\n";
		return 1;
	}

	//
	// Create TTrees
	
//	TTree* tree = new TTree 
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
