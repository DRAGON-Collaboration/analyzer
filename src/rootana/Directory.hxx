#ifndef ROOTANA_DIRECTORY_HXX
#define ROOTANA_DIRECTORY_HXX
#include <stdint.h>
#include <map>
#include <list>
#include <string>
#include <memory>
#include "Histos.hxx"

class TFile;
class TDirectory;

namespace rootana {

class Directory {
private:
	typedef std::map<uint16_t, std::list<rootana::HistBase*> > Map_t;
	typedef std::map<rootana::HistBase*, std::string> PathMap_t;
	Map_t fHistos;
	PathMap_t fHistoPaths;

private:
	Directory(const Directory& other) { }
	Directory& operator= (const Directory& other) { return *this; }

public:
	Directory() {}
	~Directory();
	void AddHist(rootana::HistBase* hist, const char* path, uint16_t eventId);
	virtual bool Open(Int_t runnum) = 0;
	virtual void Close() = 0;

protected:
	void SetHistDirectories(TDirectory* owner);

private:
	TDirectory* CreateSubDirectory(TDirectory* owner, const std::string& path);

public:
	static void test();
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
	std::auto_ptr<TFile> fFile;
	const std::string fOutputPath;
public:
	OfflineDirectory(const char* outPath);
	~OfflineDirectory();
	bool Open(Int_t runnum);
	void Close();
};
	

} // namespace rootana



#endif
