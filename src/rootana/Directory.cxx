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
#include "HistParser.hxx"
#include "Histos.hxx"
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

void rootana::Directory::DeleteHists()
{
	for (Map_t::iterator it = fHistos.begin(); it != fHistos.end(); ++it) {
		std::for_each(it->second.begin(), it->second.end(), deleteHist);
		it->second.clear();
	}
	fHistos.clear();
}	

rootana::Directory::~Directory()
{
	DeleteHists();
}

void rootana::Directory::StartNetDirServer(Int_t tcp)
{
	if(IsOpen() && tcp) {
		StartNetDirectoryServer(tcp, fDir.get());
	}
}

void rootana::Directory::NetDirExport(const char* name)
{
	if(IsOpen()) {
		NetDirectoryExport(fDir.get(), name);
	}
}

void rootana::Directory::AddHist(rootana::HistBase* hist, const char* path, uint16_t eventId)
{
	fHistos[eventId].push_back(hist);
	TDirectory* dir = CreateSubDirectory(path);
	assert(dir);
	hist->set_directory(dir);
}

void rootana::Directory::CreateHists(const char* definitionFile)
{
	/*!
	 * Parses histogram definition file & creates histograms.
	 * \todo Maybe make error in definition file not fatal.
	 */
	try {
		rootana::HistParser parse (definitionFile, *this);
		parse.run();
	}
	catch (std::exception& e) {
		std::cerr << "\n*******\n";
		dragon::err::Error("HistParser") << e.what();
		std::cerr << "*******\n\n";
		exit(1);
	}
}

TDirectory* rootana::Directory::CreateSubDirectory(const char* path)
{
	TDirectory* current = gDirectory;
	fDir->cd();
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
	if(current) current->cd();
	else fDir->cd();
	return newDir;
}

rootana::OfflineDirectory::OfflineDirectory(const char* outPath):
	fOutputPath(outPath)
{ }

rootana::OfflineDirectory::~OfflineDirectory()
{
	if(IsOpen()) Write();
}

bool rootana::OfflineDirectory::Open(Int_t runnum, const char* defFile)
{
	Close();
	std::stringstream fullPath;
	if(!fOutputPath.empty()) fullPath << fOutputPath << "/";
	fullPath << "output" << runnum << ".root";
	Reset(new TFile(fullPath.str().c_str(), "RECREATE"));
	if (!IsOpen()) return false;
	if(!(std::string(defFile)).empty()) CreateHists(defFile);
	NetDirExport("outputFile");
  return true ;
}

void rootana::OfflineDirectory::Close()
{
	if(IsOpen()) {
		Write();
		DeleteHists();
		Reset(0); // calls TFile::Close()
	}
}

rootana::OnlineDirectory::OnlineDirectory()
{ }

rootana::OnlineDirectory::~OnlineDirectory()
{ 
	if(IsOpen()) Reset(0);
}

void rootana::OnlineDirectory::Close()
{
	if(IsOpen()) {
		Reset(0);
		DeleteHists();
	}
}

bool rootana::OnlineDirectory::Open(Int_t tcp, const char* defFile)
{
	if(!IsOpen()) {
		Close();
		gROOT->cd();
		Reset(new TDirectory("rootana", "rootana online plots"));
	}
	if(!IsOpen()) return false;
	if(!(std::string(defFile)).empty()) CreateHists(defFile);
	StartNetDirServer(tcp);
	if(!tcp) fprintf(stderr, "TCP port == 0, can't start histogram server!\n");
	return true;
}
