/// \file HistParser.hxx
/// \author G. Christian
/// \brief Defines a class for parsing histogram definition files.
#ifndef ROTANA_HIST_PARSER_HXX
#define ROTANA_HIST_PARSER_HXX
#include <fstream>
#include <vector>
#include <list>
#include <string>
#include "Histos.hxx"

class TCutG;

namespace rootana {

class Directory;

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
	/// Temporary TCutGs created during parsing
	std::vector<TCutG*> fLocalCuts;
	/// Any TCutGs existing before parsing
	std::vector<TCutG*> fExistingCuts;
	/// Wrapper for histogram pointer plus related info
	struct HistInfo {
		rootana::HistBase* fHist; ///< Histogram pointer
		std::string fName;        ///< Actual name of the histogram
		std::string fPath;        ///< Subdirectory path for fHist
		UShort_t fEventId;        ///< Event id of fHist
	};
	/// List of all histograms created by the parser (plus related info)
	std::list<HistInfo> fCreatedHistograms;

public:
	/// Sets fFile
	HistParser(const char* filename);
	/// Deletes local cuts and histograms (if still owning)
	~HistParser();
	/// Checks if fFile is 'good'
	bool IsGood() { return fFile.good(); }
	/// Runs through a file and creates histograms
	void Run();
	/// Transfers ownership of created histograms from \c this to a new class
	template <class T> void Transfer(T* newOwner)
		{
			/*!
			 * \param newOwner Pointer to the class instance taking ownership
			 * \tparam T Class taking ownership of the histograms; must contain the method:
			 * \code
			 * AddHist(rootana::HistBase*, const char*, uint16_t);
			 * \endcode
			 */
			std::list<HistInfo>::iterator it = fCreatedHistograms.begin();
			for(; it != fCreatedHistograms.end(); ++it) {
				it->fHist->set_name(it->fName.c_str());
				newOwner->AddHist(it->fHist, it->fPath.c_str(), it->fEventId);
				it->fHist = 0;
			}
			fCreatedHistograms.clear();
		}

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
