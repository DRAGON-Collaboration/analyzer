/*!
 * \file Directory.cxx
 * \author G. Christian
 * \brief Implements Directory.hxx
 */
#include <stdio.h>
#include <sstream>
#include <algorithm>
#include <TROOT.h>
#include <TFile.h>
#include <TString.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TDirectory.h>
#include "utils/ErrorDragon.hxx"
#include "HistParser.hxx"
#include "Histos.hxx"
#include "Directory.hxx"

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

bool rootana::Directory::IsOpen() const
{
	return fDir.get() && !fDir->IsZombie();
}

Int_t rootana::Directory::Write(const char* name, Int_t i, Int_t j)
{
	return fDir->Write(name, i, j);
}

Int_t rootana::Directory::Write(const char* name, Int_t i, Int_t j) const
{
	return fDir->Write(name, i, j);
}

void rootana::Directory::AddHist(rootana::HistBase* hist, const char* path, uint16_t eventId)
{
	/*!
	 * \param hist Pointer to the rootana::HistBase object to add; the directory takes over ownership
	 * (cleanup responsibility) for the object once it's added.
	 * \param path String specifying the directory path where the histogram should reside, \e within
	 * \c this directory
	 * \param eventId MIDAS event id with which the histogram should be associated.
	 *
	 * Example:
	 * \code
	 * 
	 * \endcode
	 */
	fHistos[eventId].push_back(hist);
	TDirectory* dir = CreateSubDirectory(path);
	assert(dir);
	hist->set_directory(dir);
}

void rootana::Directory::CreateHists(const char* definitionFile)
{
	/*!
	 * Parses histogram definition file & creates histograms.
	 */
	assert(IsOpen());
	utils::Info("rootana::Directory")
		<< "Creating histograms for " << fDir->ClassName() << ": " << fDir->GetName();

	rootana::HistParser parse (definitionFile);
	parse.Run();
	parse.Transfer(this);
}

TDirectory* rootana::Directory::CreateSubDirectory(const char* path)
{
	/*!
	 * Parses \e path and creates the entire directory structure specified by it, within fDir.
	 * For example, a path of \c "/top/sub/bottom" first creates the directory \c "top" then
	 * \c "sub" as a subdirectory of \c "top" then \c "bottom" as a subdirectory of \c "sub".
	 * If any of the portions of the directory are already existing, they will not be re-created,
	 * but rather descended into.
	 * \returns A pointer to the bottom level of the specified directory path.
	 */
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
{
	/*!
	 * \param outPath String specifying the location where the output
	 * TFile will reside. Note that this is only the path of the TFile's
	 * \e directory, not the full path (e.g. "/path/to/somewhere", not
	 * "/path/to/somewhere/and_some_file.root").
	 */
}

rootana::OfflineDirectory::~OfflineDirectory()
{
	if(IsOpen()) {
		Write();
		DeleteHists();
		Reset(0); // calls TFile::Close()
	}
}

bool rootana::OfflineDirectory::Open(Int_t runnum, const char* defFile)
{
	/*!
	 * \param runnum Integer specifying the current run. The output file name is
	 * created from this: "output12345.root" for run number 12345.
	 * \param defFile Full path of the histogram definition file
	 * \attention No checks are made to see if you are overwriting an existing file.
	 */
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
/*!
 * \attention DeleteHists() must be called before Reset(0); see warning in fDir for why.
 */
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
/*!
 * \note DeleteHists() must be called before Reset(0); see warning in fDir for why.
 */
if(IsOpen()) {
		DeleteHists();
		Reset(0);
	}
}

void rootana::OnlineDirectory::Close()
{
	if(IsOpen()) {
		DeleteHists();
		Reset(0);
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
