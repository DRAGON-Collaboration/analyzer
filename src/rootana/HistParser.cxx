/// \file HistParser.cxx
/// \author G. Christian
/// \brief Implements HistParser.hxx
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <TROOT.h>
#include <TFile.h>
#include "utils/definitions.h"
#include "utils/Error.hxx"
#include "Histos.hxx"
#include "Events.hxx"
#include "HistParser.hxx"

extern TFile* gOutputFile;


rootana::HistParser::HistParser(const char* filename):
	fFileName(filename), fFile(filename), fLine(""), fLineNumber(0), fDir("")
{
	/*!
	 *  \param filename Path to the histogram definition file
	 *
	 *  Prints error message if file name is invalid
	 */
	
	if(!is_good()) {
		std::stringstream error;
		error << "Invalid file path: " << filename;
		throw std::invalid_argument(error.str().c_str());
	}
}

namespace { struct replace_tab { void operator()(char& c) {
	if (c == '\t') c = ' ';
} }; }

bool rootana::HistParser::read_line()
{
	/*!
	 * In addition to reading the line, does the following:
	 */
	std::getline(fFile, fLine);
	fLineNumber++;

	/// - Ignore everything after the first \c #
	size_t ppos = fLine.find('#');
	if (ppos < fLine.size())
		fLine = fLine.substr(0, ppos);

	/// - Replace each tab with a space
	std::for_each(fLine.begin(), fLine.end(), replace_tab());

	/// \returns false if at eof, true otherwise
	return is_good();
}

void rootana::HistParser::handle_dir()
{
	read_line();
	if(!is_good()) {
		std::stringstream error;
		error << "Missing \"DIR:\" argument in file \"" << fFileName << "\" at line " << fLineNumber;
		throw std::invalid_argument (error.str().c_str());
	}
	fDir = fLine;
	dragon::err::Info("HistParser")	<< "New directory: " << fDir;
}

void rootana::HistParser::handle_hist()
{
}

namespace {
void throw_bad_line(const std::string& line, int linenum, const char* fname) {
	std::stringstream error;
	error << "Bad line in file \"" << fname << "\": "<< line << ", line number: " << linenum;
	throw std::invalid_argument (error.str().c_str());
}	
void throw_summary_missing(int linenum, const char* fname) {
	std::stringstream error;
	error << "Missing \"SUMMARY_HIST:\" argument in file \"" << fname << "\" at line " << linenum;
	throw std::invalid_argument (error.str().c_str());
} }

struct testCut: public rootana::Cut {
	bool operator() () const { return rootana::gHead.bgo.q[0] > 1000; }
};

void rootana::HistParser::handle_summary()
{
	unsigned lhst, lpar, lnum;
	std::string shst, spar, snum;
	{
		unsigned* lll[3] = { &lhst, &lpar, &lnum };
		std::string* lread[3] = { &shst, &spar, &snum };
		for (int i=0; i< 3; ++i) {
			read_line();
			if(!is_good()) throw_summary_missing(fLineNumber, fFileName);
			*(lll[i])   = fLineNumber;
			*(lread[i]) = fLine;
		}
	}

	std::stringstream cmdData;
	cmdData << "rootana::DataPointer::New(" << spar << ", " << snum << ")";
	rootana::DataPointer* data = (rootana::DataPointer*)gROOT->ProcessLineFast(cmdData.str().c_str());
	if (!data) throw_bad_line(spar, lpar, fFileName);

	std::stringstream cmdHist;
	cmdHist << "new rootana::SummaryHist(" << shst << ", " << cmdData.str() << ")";
	rootana::HistBase* hst = (rootana::HistBase*)gROOT->ProcessLineFast(cmdHist.str().c_str());
	if (!hst) throw_bad_line(shst, lhst, fFileName);

	bool added = add_hist(hst, spar);
	if(!added) throw_bad_line(spar, lpar, fFileName);
}

namespace { inline bool contains(const std::string& s, const char* arg) {
	return s.find(arg) < s.size();
} }

bool rootana::HistParser::add_hist(rootana::HistBase* hst, const std::string& spar) {
	UShort_t type;
	if (0) { }
	else if (contains(spar, "rootana::gHead"))  type = DRAGON_HEAD_EVENT;
	else if (contains(spar, "rootana::gTail"))  type = DRAGON_TAIL_EVENT;
	else if (contains(spar, "rootana::gCoinc")) type = DRAGON_COINC_EVENT;
	else return false;
	rootana::EventHandler::Instance()->AddHisto(hst, type, gOutputFile, fDir.c_str());
	std::cerr << "\t";
	dragon::err::Info("HistParser")
		<< "Adding histogram " << hst->name() << " to directory " << fDir;

	return true;
}

void rootana::HistParser::run()
{
	while (read_line()) {
		if (0) { }
		else if (contains(fLine, "DIR:")) handle_dir();
		else if (contains(fLine, "SUMMARY_HIST:")) handle_summary();
	}
}		

void rootana::HistParser::test()
{
	rootana::HistParser parse ("src/rootana/histos.dat");
	try { parse.run(); }
	catch (std::exception& e) {
		dragon::err::Error("HistParser::run()") << e.what();
	}
}
