#include <stdio.h>
#include <sstream>
#include <algorithm>
#include <TROOT.h>
#include <TFile.h>
#include <TString.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TDirectory.h>
#include "Directory.hxx"
#include "DataPointer.hxx"
#include "Globals.h"

#ifdef MIDASSYS
#include "libNetDirectory/netDirectoryServer.h"
#else
namespace {
inline void NetDirectoryExport(TFile*, const char*) { }
inline void StartNetDirectoryServer(int, TDirectory*) { }
}
#endif


namespace {
inline void deleteHist(rootana::HistBase* h) { delete h; }
}

rootana::Directory::~Directory()
{
	for (Map_t::iterator it = fHistos.begin(); it != fHistos.end(); ++it) {
		std::for_each(it->second.begin(), it->second.end(), deleteHist);
	}
}

void rootana::Directory::AddHist(rootana::HistBase* hist, const char* path, uint16_t eventId)
{
	fHistos[eventId].push_back(hist);
	fHistoPaths[hist] = path;
}

TDirectory* rootana::Directory::CreateSubDirectory(TDirectory* owner, const std::string& path)
{
	owner->cd();
	TString sdp(path);
	std::auto_ptr<TObjArray> dirs (sdp.Tokenize("/"));
	for (int i=0; i< dirs->GetEntries(); ++i) {
		TString dirname = static_cast<TObjString*>(dirs->At(i))->GetString();
		TDirectory* dir = dynamic_cast<TDirectory*>(gDirectory->FindObject(dirname.Data()));
		if(dir) dir->cd();
		else {
			TDirectory* d = gDirectory->mkdir(dirname.Data());
			d->cd();
		}
	}
	TDirectory* newDir = gDirectory;
	owner->cd();
	return newDir;
}

void rootana::Directory::SetHistDirectories(TDirectory* owner)
{
	for(PathMap_t::iterator it = fHistoPaths.begin(); it!= fHistoPaths.end(); ++it) {
		TDirectory* dir = CreateSubDirectory(owner, it->second);
		it->first->set_directory(dir);
	}
}


void rootana::Directory::test()
{

}

rootana::OfflineDirectory::OfflineDirectory(const char* outPath):
	fFile(0), fOutputPath(outPath)
{ }

rootana::OfflineDirectory::~OfflineDirectory()
{
	if(fFile.get()) {
		fFile->Write();
	}
}

bool rootana::OfflineDirectory::Open(Int_t runnum)
{
	Close();
	std::stringstream fullPath;
	fullPath << fOutputPath << "/output" << runnum << ".root";
	fFile.reset(new TFile(fullPath.str().c_str(), "RECREATE"));
	if (fFile->IsZombie()) return false;
	SetHistDirectories(fFile.get());
  NetDirectoryExport(fFile.get(), "outputFile");
  return true ;
}

void rootana::OfflineDirectory::Close()
{
	if(fFile.get()) {
		fFile->Write();
		fFile.reset(0); // calls fFile->Close()
	}
}

// void rootana::Directory::Open(int tcp)
// {
// 	Close();
// 	gROOT->cd();
// 	fDirectory.reset(new TDirectory("rootana", "rootana online plots"));
// 	if (tcp) StartNetDirectoryServer(tcp, fDirectory.get());
// 	else fprintf(stderr, "TCP port == 0, can't start histogram server!\n");
// }

