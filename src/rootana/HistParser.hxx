/// \file HistParser.hxx
/// \author G. Christian
/// \brief Defines a class for parsing histogram definition files.
#ifndef ROTANA_HIST_PARSER_HXX
#define ROTANA_HIST_PARSER_HXX
#include <fstream>
#include <string>


class TDirectory;

namespace rootana {

class HistBase;

/// Parses text file to create histograms at program startup.
/*!
 *  Allows histogram definitions to be changed without requiring
 *  the program to be re-compiled.
 */
class HistParser {
private:
	/// Hist definitinon filename
	const char* const fFilename;
	/// Histogram definition file
	std::ifstream fFile;
	/// Current line in the file
	std::string fLine;
	/// Current line number
	unsigned fLineNumber;
	/// Current directory argument
	std::string fDir;
	/// Most recently created histogram
	HistBase* fLastHist;
	/// Owner ('top-level') directory (or file)
	TDirectory* fOwner;

public:
	/// Sets fFile
	HistParser(const char* filename, TDirectory* owner);
	/// Empty
	~HistParser() { }
	/// Checks if fFile is 'good'
	bool is_good() { return fFile.good(); }
	/// Runs through a file and creates histograms
	void run();

private:
	/// Reads & formats a single line
	bool read_line();
	/// Handle a directory flag in the file
	void handle_dir();
	/// Handle a summary hist flag in the file
	void handle_summary();
	/// Handle a hist flag in the file
	void handle_hist(const char* type);
	/// Handle a cut flag in the file
	void handle_cut();
	/// Handles command flag in the file
	void handle_command();
	/// Adds a histogram to rootana
	void add_hist(rootana::HistBase* hst, Int_t type);
};

} //namespace rootana



#endif
