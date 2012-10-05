/*!
 * \file RootanaDragon.hxx
 * \author G. Christian
 * \brief Defines callbacks, helper functions, etc. for rootana
 */
#ifndef ROOTANA_DRAGON_HXX
#define ROOTANA_DRAGON_HXX

// Declare callback functions as extern "C"
#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif

/// Run start transition handler.
EXTERN_C void rootana_run_start(int transition, int run, int time);

/// Run stop transition handler
EXTERN_C void rootana_run_stop(int transition, int run, int time);

/// Run pause transition handler
EXTERN_C void rootana_run_pause(int transition, int run, int time);

/// Run resume transition handler
EXTERN_C void rootana_run_resume(int transition, int run, int time);

/// Event handler callback function
EXTERN_C void rootana_handle_event(const void* pheader, const void* pdata, int size);


#include <TApplication.h>

class TFile;
class TDirectory;
class VirtualOdb;
namespace midas { class Event; }


namespace rootana {

class TSQueue;

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
	TFile* fOutputFile;         ///< Output ROOT file
	TDirectory* fOnlineHistDir; ///< Online-only histogram directory
	VirtualOdb* fOdb;           ///< Database class. \todo Switch to midas::Database
	TSQueue* fQueue;            ///< Timestamping queue

public:
	/// Calls TApplication constructor
	App(const char* appClassName, Int_t* argc, char** argv, void* options = 0, Int_t numOptions = 0);

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

	/// Process an offline MIDAS file
	int midas_file(const char* fname);

	/// Process online MIDAS data
	int midas_online(const char* host = "", const char* experiment = "dragon");

	/// Create histograms from definitions file
	int create_histograms(const char* definition_file);

	/// Deletes fQueue if it's non-null;
	virtual ~App();

private:
	/// Prints 'help' message
	void help();

	/// Processes command line args
	void process_argv(int argc, char** argv);

	ClassDef (rootana::App, 0);
};

}



#endif
