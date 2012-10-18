/*!
 * \file Application.hxx
 * \author G. Christian
 * \brief Defines TApplication class for rootana analyzer.
 */
#ifndef ROOTANA_DRAGON_HXX
#define ROOTANA_DRAGON_HXX
#include <list>
#include <memory>
#include <TApplication.h>
#include "dragon/Coinc.hxx"
#include "Directory.hxx"


class TFile;
class TDirectory;
namespace midas  { class Event; class Database; }
namespace tstamp { class Queue; }

namespace rootana {

class MidasOnline;

/// Application class for dragon rootana
class App: public TApplication {

	/// Tells the running mode of the application
	enum Mode_t { ONLINE, OFFLINE };

private:
	int fRunNumber; ///< Current run number
	int fMode;      ///< Running mode (online or offline)
	int fCutoff;    ///< Event cutoff (offline only)
	int fReturn;    ///< Return value
	int fTcp;       ///< TCP port value
	std::string fFilename;      ///< Offline file name
	std::string fHost;          ///< Online host name
	std::string fExpt;          ///< Online experiment name
	std::string fHistos;        ///< Histogram specification file (online + file)
	std::string fHistosOnline;  ///< Histogram specification file (online only)
	rootana::OfflineDirectory fOutputFile;      ///< Online/offline histograms
	rootana::OnlineDirectory fOnlineHists;      ///< Online-only histograms
	std::auto_ptr<midas::Database> fOdb;        ///< Online/offline database
	std::auto_ptr<tstamp::Queue> fQueue;        ///< Timestamping queue
	std::auto_ptr<MidasOnline> fMidasOnline;    ///< "Online midas" instance
	std::list<dragon::Head> fHeadProcessed;     ///< Head events already unpacked
	std::list<dragon::Tail> fTailProcessed;     ///< Tail events already unpacked

public:
	/// Calls TApplication constructor
	App(const char* appClassName, Int_t* argc, char** argv);

	/// Return gApplication cast to rootana::App
	static App* instance();

	/// Runs the application
	virtual void Run(Bool_t retrn = kFALSE);

	/// Terminates the application
	virtual void Terminate(Int_t status = 0);

	/// Gets fReturn
	int ReturnVal() { return fReturn; }

	/// Start-of-run actions
	void run_start(int runnum);

	/// End-of-run acitons
	void run_stop(int runnum);

	/// Handle a midas event
	void handle_event(midas::Event& event);

	/// Tells how to handle a singles event from the beginning of fQueue
	void Process(const midas::Event& event);

	/// Tells how to handle a coincidence event from the beginning of fQueue
	void Process(const midas::Event& event1, const midas::Event& event2);

	/// Process an offline MIDAS file
	int midas_file(const char* fname);

	/// Process online MIDAS data
	int midas_online(const char* host = "", const char* experiment = "dragon");

	/// Deletes fQueue if it's non-null;
	virtual ~App();

private:
	/// Prints 'help' message
	void help();

	/// Processes command line args
	void process_argv(int argc, char** argv);

	/// Handles cleanup for destructor or Terminate()
	void do_exit();

	ClassDef (rootana::App, 0);
};

}



#endif
