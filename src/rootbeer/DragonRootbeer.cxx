///
/// \file DragonRootbeer.cxx
/// \author G. Christian
/// \brief Implementation of rb::BufferSource::New() and rb::Rint::RegisterEvents()
///
#include <cassert>

// ROOTBEER includes //
#include "Main.hxx"
#include "Rint.hxx"
#include "Buffer.hxx"

// DRAGON includes //
#include "utils/definitions.h"
#include "utils/Stringify.h"
#include "midas/Database.hxx"
#include "MidasBuffer.hxx"
#include "DragonEvents.hxx"
#include "DragonRootbeer.hxx"


namespace rootbeer {

/// Class to implement a custom main() function
class DragonMain: public rb::Main {
	public:
	/// Overrides the default rb::Main::Run()
	virtual int Run(int argc, char** argv); 
};

int rootbeer::DragonMain::Run(int argc, char** argv)
{
	assert(!"Not implemented yet!");
#if 0
 if (argc > 1 && !strcmp(argv[1], "--unpack")) { // 'rbunpack'

	 std::string fin;
	 handle_args(argc, argv, fin);
	 int argc2 = argc + 1;
	 char** argv2 = (char**)malloc(argc2*sizeof(char*));
	 for(int i=0; i< argc; ++i) {
		 argv2[i] = (char*)malloc(strlen(argv[i]+1));
		 strcpy(argv2[i], argv[i]);
	 }
	 argv2[argc] = (char*)malloc(4);
	 strcpy(argv2[argc], "-ng");

	 rb::Rint rbApp("Rbunpack", &argc2, argv2, 0, 0, true);
	 rbApp.StartSave(false);
	 rb::AttachFile(fin.c_str());
	 gSystem->Sleep(1e2);
	 while(TThread::GetThread("AttachFile"));
	 rbApp.Terminate(0);
	 for(int i=0; i< argc2; ++i)
		 free(argv2[i]);
	 free(argv2);
	 return 0;

 } else { // Standard ROOTBEER

	 std::set<std::string> args(argv, argv+argc);
	 Bool_t lite = args.count("-l");
	 rb::Rint rbApp("Rootbeer", &argc, argv, 0, 0, lite);
	 gROOT->ProcessLine("gStyle->SetOptTitle(kTRUE)");
	 gROOT->ProcessLine("gStyle->SetOptStat(kTRUE)");
	 rbApp.Run();
	 return 0;

 }
#endif
}

} // namespace rootbeer


rb::Main* rb::GetMain()
{
	/*!
	 * Required function to return an instance of rb::Main(). Here we just use the default
	 * implementation by returning a value of the base class rb::Main.
	 */
	return new rb::Main();
}

rb::BufferSource* rb::BufferSource::New()
{
	/*!
	 * From http://trshare.triumf.ca/~gchristian/rootbeer/doc/html/classrb_1_1_buffer_source.html
	 *
	 * This is used as a generic way to get a pointer to an instance of a class derived from BufferSource,
	 * when the specific derived class is unknown <i>a priori</i>.  This function should be implemented by users
	 * such that it returns a pointer to the specific class they want to use for reading and unpacking data. Example:
	 * \code
	 * BufferSource* New() {
	 *     return new MyBufferSource();
	 * }
	 * \endcode
	 * \returns A pointer to a \c new instance of a class derived from BufferSource.
	 *
	 * In this specific instance, we return a pointer to a \c new rootbeer::MidasBuffer
	 */ 
	
  return new rootbeer::MidasBuffer(1024*1024);
}

void rb::Rint::RegisterEvents()
{
	/*!
	 * From http://trshare.triumf.ca/~gchristian/rootbeer/doc/html/classrb_1_1_buffer_source.html :
	 *
	 * Registers all event processors in the program.
	 *
	 * This needs to be implemented by users to account for the different event processors
	 * they want to use in the program. See rb::Event documentation for more information.
	 *
	 * In this specific case, we registers event processors for:
	 */

	/// - Gamma singles [ rootbeer::GammaEvent ]: "GammaEvent"
	RegisterEvent<rootbeer::GammaEvent> (DRAGON_HEAD_EVENT, "GammaEvent");

	/// - Heavy-ion singles [ rootbeer::HeavyIonEvent ]: "HeavyIonEvent"
	RegisterEvent<rootbeer::HeavyIonEvent> (DRAGON_TAIL_EVENT, "HeavyIonEvent");

	/// - Coincidences [ rootbeer::CoincEvent ]: "CoincEvent"
  RegisterEvent<rootbeer::CoincEvent> (DRAGON_COINC_EVENT, "CoincEvent");

	/// - Head Scalers [ rootbeer::HeadScaler ]: "HeadScaler"
	RegisterEvent<rootbeer::HeadScaler> (DRAGON_HEAD_SCALER, "HeadScaler");

	/// - Tail Scalers [ rootbeer::TailScaler ]: "TailScaler"
	RegisterEvent<rootbeer::TailScaler> (DRAGON_TAIL_SCALER, "TailScaler");

	/// - Timestamp Diagnostics [ rootbeer::TstampDiagnostics ]: "TstampDiagnostics"
	RegisterEvent<rootbeer::TStampDiagnostics> (6, "TStampDiagnostics");

	/// - Run parameters [ rootbeer::RunParameters ]: "RunParameters"
	RegisterEvent<rootbeer::RunParameters> (7, "RunParameters");

	/// Also: expose macros in definitions.h to CINT
	std::stringstream cmd;
	cmd << "#include \"" << DRAGON_UTILS_STRINGIFY(RB_DRAGON_HOMEDIR) << "/src/utils/definitions.h\"";
	gROOT->ProcessLine(cmd.str().c_str());
}
