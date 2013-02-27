/// \file DragonRootbeer.cxx
/// \author G. Christian
/// \brief Implementation of rb::BufferSource::New() and rb::Rint::RegisterEvents()

// ROOTBEER includes //
#include "Rint.hxx"
#include "Buffer.hxx"

// DRAGON includes //
#include "utils/definitions.h"
#include "utils/Stringify.h"
#include "MidasBuffer.hxx"
#include "DragonEvents.hxx"
#include "DragonRootbeer.hxx"


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
	
  return new rootbeer::MidasBuffer(1024*100);
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
