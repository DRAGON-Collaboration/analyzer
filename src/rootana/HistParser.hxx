/// \file HistParser.hxx
/// \author G. Christian
/// \brief Defines a class for parsing histogram definition files
#ifndef ROTANA_HIST_PARSER_HXX
#define ROTANA_HIST_PARSER_HXX
#include <fstream>
#include <string>


namespace rootana {

class HistBase;

class HistParser {
private:
	/// Hist definitinon filename
	const char* const fFileName;

	/// Histogram definition file
	std::ifstream fFile;

	/// Current line in the file
	std::string fLine;

	/// Current line number
	unsigned fLineNumber;

	/// Current directory argument
	std::string fDir;
public:
	/// Sets fFile
	HistParser(const char* filename);
	
	/// Empty
	~HistParser() { }

	/// Checks if fFile is 'good'
	bool is_good() { return fFile.good(); }

	/// RUns through a file and creates histograms
	void run();

private:
	/// Reads & formats a single line
	bool read_line();
	
	/// Handle a directory flag in the file
	void handle_dir();

	/// Handle a summary hist flag in the file
	void handle_summary();

	/// Handle a hist flag in the file
	void handle_hist();

	/// Adds a histogram to rootana
	bool add_hist(rootana::HistBase* hst, const std::string& spar);

public:
	/// For testing
	static void test();
};


} //namespace rootana



#endif
