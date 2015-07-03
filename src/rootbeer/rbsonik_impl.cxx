// DRAGON includes //
#include "utils/ErrorDragon.hxx"
#include "utils/definitions.h"
#include "utils/Functions.hxx"
#include "midas/Database.hxx"
#include "midas/Event.hxx"
#include "rbdragon.hxx"
#include "rbsonik.hxx"

// ROOTBEER includes //
#include <TString.h>
#include "Rint.hxx"
#include "Attach.hxx"
#include "Rootbeer.hxx"
#include "Event.hxx"


// ============ Required Static / Free Function Implementations ============ //

rb::MidasBuffer* rb::MidasBuffer::Create()
{
	/*!
	 * \returns <tt>new rbdragon::MidasBuffer();</tt>
	 */
	return new rbsonik::MidasBuffer();
}

rb::Main* rb::GetMain()
{
	/*!
	 * Use the default main() implementation.
	 * \returns <tt>new rb::Main();</tt>
	 */
	return new rbdragon::Main();
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

	/// - Gamma singles [ rbdragon::GammaEvent ]: "GammaEvent"
	RegisterEvent<rbdragon::GammaEvent> (DRAGON_HEAD_EVENT, "GammaEvent");

	/// - Heavy-ion singles [ rbdragon::HeavyIonEvent ]: "HeavyIonEvent"
	RegisterEvent<rbdragon::HeavyIonEvent> (DRAGON_TAIL_EVENT, "HeavyIonEvent");

	/// - Coincidences [ rbdragon::CoincEvent ]: "CoincEvent"
  RegisterEvent<rbdragon::CoincEvent> (DRAGON_COINC_EVENT, "CoincEvent");

	/// - Epics [ rbdragon::EpicsEvent ]: "EpicsEvent"
  RegisterEvent<rbdragon::EpicsEvent> (DRAGON_EPICS_EVENT, "EpicsEvent");

	/// - Head Scalers [ rbdragon::HeadScaler ]: "HeadScaler"
	RegisterEvent<rbdragon::HeadScaler> (DRAGON_HEAD_SCALER, "HeadScaler");

	/// - Tail Scalers [ rbdragon::TailScaler ]: "TailScaler"
	RegisterEvent<rbdragon::TailScaler> (DRAGON_TAIL_SCALER, "TailScaler");

	/// - Aux Scalers [ rbdragon::AuxScaler ]: "AuxScaler"
	RegisterEvent<rbdragon::AuxScaler> (DRAGON_AUX_SCALER, "AuxScaler");

	/// - Timestamp Diagnostics [ rbdragon::TstampDiagnostics ]: "TstampDiagnostics"
	RegisterEvent<rbdragon::TStampDiagnostics> (6, "TStampDiagnostics");

	/// - Run parameters [ rbdragon::RunParameters ]: "RunParameters"
	RegisterEvent<rbdragon::RunParameters> (7, "RunParameters");

	/// - Sonik Event
	RegisterEvent<rbsonik::SonikEvent> (0, "SonikEvent");

	/// Also: expose macros in definitions.h to CINT
	std::stringstream cmd;
	cmd << "#include \"" << DRAGON_UTILS_STRINGIFY(RB_DRAGON_HOMEDIR) << "/src/utils/definitions.h\"";
	gROOT->ProcessLine(cmd.str().c_str());
}
