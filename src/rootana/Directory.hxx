#ifndef ROOTANA_DIRECTORY_HXX
#define ROOTANA_DIRECTORY_HXX
#include <stdint.h>
#include <map>
#include <list>
#include <string>
#include <memory>
#include <algorithm>
#include <TDirectory.h>

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
	Directory(TDirectory* dir = 0):	fDir(dir) { }

	/// Writes directory, deletes histograms, deletes directory
	~Directory();

	/// Checks if fDir points to a "good" working directory
	bool IsOpen() const	{ return fDir.get() && !fDir->IsZombie(); }

	/// Resets fDir to a new directory, calling the destructor on the old one.
	void Reset (TDirectory* newDir) { fDir.reset(newDir); }

	/// Returns the name of the directory if it's open, default "closed" message otherwise
	const char* GetName() const
		{
			if (IsOpen()) return fDir->GetName();
			return "< OfflineDirectory:: Unopened file >";
		}

	/// Returns the specific class name of the directory if open, "TDirectory" otherwise
	const char* ClassName() const
		{
			if (IsOpen()) return fDir->ClassName();
			return "TDirectory";
		}

	/// Adds a histogram to \c this directory
	void AddHist(rootana::HistBase* hist, const char* path, uint16_t eventId);

	/// Virtual method to open (initialize) the directory
	virtual bool Open(Int_t, const char*) = 0;

	/// Virtual method to close (cleanup) the directory
	virtual void Close() = 0;

protected:
	void CreateHists(const char* definitionFile);
	void DeleteHists();
	void NetDirExport(const char* name);
	void StartNetDirServer(Int_t tcp);
	Int_t Write (const char* name = 0, Int_t i = 0, Int_t j = 0)
		{ return fDir->Write(name, i, j); }
	Int_t Write (const char* name = 0, Int_t i = 0, Int_t j = 0) const
		{ return fDir->Write(name, i, j); }

private:
	TDirectory* CreateSubDirectory(const char* path);
	Directory(const Directory& other) { }
	Directory& operator= (const Directory& other) { return *this; }

public:
	template <class R> void CallForAll( R (HistBase::*f)(), int32_t id = -1 )
		{
			Map_t::iterator itFirst, itLast;
			if (id < 0) {
				itFirst = fHistos.begin();
				itLast  = fHistos.end();
			}
			else {
				itFirst = itLast = fHistos.find(static_cast<uint16_t>(id));
			}

			for (Map_t::iterator it = itFirst; it != itLast; ++it) {
				std::for_each(it->second.begin(), it->second.end(), std::mem_fun(f));
			}
		}
};

class OfflineDirectory: public Directory {
private:
	const std::string fOutputPath;
public:
	OfflineDirectory(const char* outPath);
	~OfflineDirectory();
	bool Open(Int_t runnum, const char* defFile);
	void Close();
};

class OnlineDirectory: public Directory {
public:
	OnlineDirectory();
	~OnlineDirectory();
	bool Open(Int_t tcp, const char* defFile);
	void Close();
};

} // namespace rootana



#endif
