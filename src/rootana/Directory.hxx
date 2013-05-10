/*!
 * \file Directory.hxx
 * \author G. Christian
 * \brief Defines directory classes to manage rootana histograms.
 */
#ifndef ROOTANA_DIRECTORY_HXX
#define ROOTANA_DIRECTORY_HXX
#include <stdint.h>
#include <map>
#include <list>
#include <string>
#include <memory>
#include <algorithm>

class TDirectory;

namespace rootana {

class HistBase;

/// Abstract rootana directory class.
/*!
 * Implementations serve the function of a standard ROOT TDirectory by wrapping
 * an instance of TDirectory* and directing it's actions. In addition, this class
 * also manages rootana histograms; in particular, it creates all desired histograms
 * from the user's definition file and provides public methods to call rootana::HistBase
 * functions for all histograms (or all histograms of a given "event" type). This class
 * also internally handles the work of "net" exporting the internal directories, allowing
 * histograms to be viewed in roody.
 */
class Directory {
public:
	/// Container sorting lists of HistBase pointers, each keyed by the event ID.
	typedef std::map<uint16_t, std::list<rootana::HistBase*> > Map_t;

private:
	/// Container for all rootana histograms.
	/*! See Map_t typedef */
	Map_t fHistos;

	/// Internal ROOT directory
	/*!
	 * \note Derived classes may set this to a specific type using the Reset() method.
	 * \warning The default ROOT behavior for histograms owned by a TDirectory is for
	 * the TDirectory to take care of their destruction, i.e. the histogram destructors
	 * are called by ~TDirectory or TDirectory::Close(), unless the histogram has already
	 * been deleted (which removes it from it's owning directory). Since the rootana::Hist
	 * destructor takes responsibility for deleting it's own internal pointer to a TH*D, we
	 * must ensure that the rootana::Hist destructor is called \e before the TDirectory
	 * destructor; otherwise, we get double deletion. Since we use std::auto_ptr to
	 * encapsulate the TDirectory pointer, the easiest way to do this is to simply ensure
	 * that fDir is always the last data member defined in rootana::Directory.
	 */
	std::auto_ptr<TDirectory> fDir;

public:
	/// Initializes fDir
	Directory(TDirectory* dir = 0);
	/// Writes directory, deletes histograms, deletes directory
	virtual ~Directory();
	/// Checks if fDir points to a "good" working directory
	bool IsOpen() const;
	/// Resets fDir to a new directory, calling the destructor on the old one.
	void Reset (TDirectory* newDir) { fDir.reset(newDir); }
	/// Adds a histogram to \c this directory
	void AddHist(rootana::HistBase* hist, const char* path, uint16_t eventId);
	/// Virtual method to open (initialize) the directory
	virtual bool Open(Int_t, const char*) = 0;
	/// Virtual method to close (cleanup) the directory
	virtual void Close() = 0;

protected:
	/// Parse definition file and create histograms
	void CreateHists(const char* definitionFile);
	/// Free memory allocated to all owned histograms
	void DeleteHists();
	/// Export fDir for viewing in roody
	void NetDirExport(const char* name);
	/// Start net directory server for roody viewing
	void StartNetDirServer(Int_t tcp);
	/// Calls fDir->Write() [const version]
	Int_t Write (const char* name = 0, Int_t i = 0, Int_t j = 0);
	/// Calls fDir->Write() [const version]
	Int_t Write (const char* name = 0, Int_t i = 0, Int_t j = 0) const;

private:
	/// Creates a sub directory with the given path name
	TDirectory* CreateSubDirectory(const char* path);
	/// Disallow copying
	Directory(const Directory& other) { }
	/// Disallow assignment
	Directory& operator= (const Directory& other) { return *this; }

public:
	/// Calls a specific member function for all histograms (or all w/ a given ID)
	template <class R>
	void CallForAll( R (HistBase::*f)(), int32_t id = -1 )
		{
			/*!
			 * \tparam R return value of the function being called
			 * \param f Pointer to the HistBase member function to call
			 * \param id Event id of the histograms for which you want to call the function \e f.
			 * Specification of an ID < 0 (the default) results in calling the function for all IDs.
			 *
			 * Example:
			 * \code
			 * directory->CallForAll (&rootana::HistBase::fill, 3); // Fills all histos w/ event ID 3
			 * directory->CallForAll (&rootana::HistBase::clear); // Clears every histogram in the directory
			 * \endcode
			 */
			Map_t::iterator itFirst = id < 0 ? fHistos.begin() : fHistos.find(static_cast<uint16_t>(id));
			Map_t::iterator itLast  = id < 0 ? fHistos.end()   : itFirst;
			if (itLast != fHistos.end()) ++itLast;

			for (Map_t::iterator it = itFirst; it != itLast; ++it) {
				std::for_each(it->second.begin(), it->second.end(), std::mem_fun(f));
			}
		}
};

/// Concrete derived class of Directory for offline data
/*!
 * Sets fDir to point to a TFile instance and handles it's opening and
 * closing appropriately for this class.
 */
class OfflineDirectory: public Directory {
private:
	/// Path specifying the file output location
	const std::string fOutputPath;
public:
	/// Sets fOutputPath
	OfflineDirectory(const char* outPath);
	/// Writes and frees histograms, closes the TFile.
	~OfflineDirectory();
	/// Opens a new TFile; creates histograms from definition file, exports directory for network viewing
	bool Open(Int_t runnum, const char* defFile);
	/// Writes and frees histograms, closes the TFile.
	void Close();
};

/// Directory class for online-only histograms
/*!
 * Histograms owned by this class are available for network viewing
 * while connected to an online data source, but they are not saved anywhere
 * to disk.
 */
class OnlineDirectory: public Directory {
public:
	/// Empty
	OnlineDirectory();
	/// Cleans up histogram memory, frees TDirectory memory
	~OnlineDirectory();
	/// Creates offline directory, histograms, exports directory for network viewing
	bool Open(Int_t tcp, const char* defFile);
	/// Cleans up histogram memory, frees TDirectory memory
	void Close();
};

} // namespace rootana



#endif
