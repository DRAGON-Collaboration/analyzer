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

namespace midas { class Event; }

namespace rootana {

/// Application class for dragon rootana
class App: public TApplication {

	/// Tells the running mode of the application
	enum Mode_t { ONLINE, OFFLINE };

private:
	int fMode; ///< Running mode (online or offline)
	int fCutoff; ///< Event cutoff (offline only)
	int fReturn; ///< Return value
	std::string fFilename; ///< Offline file name
	std::string fHost;     ///< Online host name
	std::string fExpt;     ///< Online experiment name

public:
	/// Calls TApplication constructor
	App(const char* appClassName, Int_t* argc, char** argv, void* options = 0, Int_t numOptions = 0);

	/// Return gApplication cast to rootana::App
	static App* instance();

	/// Runs the application
	virtual void Run(Bool_t retrn = kFALSE);

	/// Gets fReturn
	int ReturnVal() { return fReturn; }

	/// Handle a midas event
	void handle_event(midas::Event& event);

	/// Process an offline MIDAS file
	int midas_file(const char* fname, int cutoff = 0);

	/// Process online MIDAS data
	int midas_online(const char* host = "", const char* experiment = "dragon");

	/// Create histograms from definitions file
	int create_histograms(const char* definition_file);

	/// Empty
	virtual ~App() { }

private:
	/// Prints 'help' message
	void help();

	ClassDef (rootana::App, 0);
};

}



#endif
