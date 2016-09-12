//! \file UserLinkdef.h
//! \brief User-defined portion of the CINT Linkdef file.
//!
//! \full This file is where you can tell CINT about the classes you've written to store and analyze
//! your data.  At the least, you will need to do this for any class that stores parameters that you
//! wish to view or variables used in calculating those parameters.
//! Usually the easiest option is link everything in the header files which define your classes. \n
//! Example:
//! \code
//! #pragma link C++ defined_in /path/to/my/header.hxx;
//! \endcode
//! (note that you need to specify the full path to the header file).
//!
//! If the above doesn't work, you might have to specify each class (or namespace) individually
//! (or figure out why not). \n
//! Example:
//! \code
//! #pragma link C++ namespace my;
//! #pragma link C++ class my::Class+;
//! // -- OR -- //
//! #pragma link C++ class MyClass+;
//! // -- ETC -- //
//! \endcode
//! (note that the '+' at the end is required except in special cases).
#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma extra_include "RVersion.h";
#include <RVersion.h>

#pragma link C++ global gROOT;
#pragma link C++ global gEnv;

class TH1;
class TTree;

#pragma link C++ class std::vector<UDouble_t>+;

#if ROOT_VERSION_CODE >= ROOT_VERSION(5,30,0)
#pragma link C++ class std::auto_ptr<midas::Xml>+;
#pragma link C++ class std::auto_ptr<dragon::RossumData>+;
#pragma link C++ class std::auto_ptr<std::ifstream>+;
#elif ROOT_VERSION_CODE < ROOTVERSION(5,30,0)
#pragma link C++ class dragon::utils::AutoPtr<midas::Xml>+;
#pragma link C++ class dragon::utils::AutoPtr<dragon::RossumData>+;
#pragma link C++ class dragon::utils::AutoPtr<std::ifstream>+;
#endif

#pragma link C++ defined_in ../src/Defaults.hxx;
#pragma link C++ defined_in ../src/Sonik.hxx;
#pragma link C++ defined_in ../src/TStamp.hxx;
#pragma link C++ defined_in ../src/Unpack.hxx;

#pragma link C++ defined_in ../src/utils/AutoPtr.hxx;
#pragma link C++ defined_in ../src/utils/Bits.hxx;
#pragma link C++ defined_in ../src/utils/Calibration.hxx;
#pragma link C++ defined_in ../src/utils/Constants.hxx;
#pragma link C++ defined_in ../src/utils/definitions.hxx;
#pragma link C++ defined_in ../src/utils/ErrorDragon.hxx;
#pragma link C++ defined_in ../src/utils/Functions.hxx;
#pragma link C++ defined_in ../src/utils/IntTypes.hxx;
#pragma link C++ defined_in ../src/utils/LinearFitter.hxx;
#pragma link C++ defined_in ../src/utils/RootAnalysis.hxx;
#pragma link C++ defined_in ../src/utils/Selectors.hxx;
#pragma link C++ defined_in ../src/utils/TAtomicMass.h;
#pragma link C++ defined_in ../src/utils/Uncertainty.hxx;
#pragma link C++ defined_in ../src/utils/Valid.hxx;
#pragma link C++ defined_in ../src/utils/VariableStructs.hxx;

#pragma link C++ defined_in ../src/midas/Database.hxx;
#pragma link C++ defined_in ../src/midas/Event.hxx;
#pragma link C++ defined_in ../src/midas/Odb.hxx;
#pragma link C++ defined_in ../src/midas/Xml.hxx;

#pragma link C++ class dragon::utils::AdcVariables<dragon::Bgo::MAX_CHANNELS>+;
#pragma link C++ class dragon::utils::TdcVariables<dragon::Bgo::MAX_CHANNELS>+;
#pragma link C++ class dragon::utils::TdcVariables<dragon::Bgo::MAX_CHANNELS>+;
#pragma link C++ class dragon::utils::PositionVariables<dragon::Bgo::MAX_CHANNELS>+;
#pragma link C++ class dragon::utils::AdcVariables<dragon::Dsssd::MAX_CHANNELS>+;
#pragma link C++ class dragon::utils::TdcVariables<1>+;
#pragma link C++ class dragon::utils::AdcVariables<dragon::Mcp::MAX_CHANNELS>+;
#pragma link C++ class dragon::utils::TdcVariables<1>+;
#pragma link C++ class dragon::utils::AdcVariables<dragon::IonChamber::MAX_CHANNELS>+;
#pragma link C++ class dragon::utils::TdcVariables<1>+;
#pragma link C++ class dragon::utils::AdcVariables<dragon::SurfaceBarrier::MAX_CHANNELS>+;
#pragma link C++ class dragon::utils::AdcVariables<dragon::NaI::MAX_CHANNELS>+;
#pragma link C++ class dragon::utils::AdcVariables<1>+;
#pragma link C++ namespace dragon::utils;

#pragma link C++ class dragon::Tail<vme::Io32>+;
#pragma link C++ class dragon::Tail<vme::V1190>+;
#pragma link C++ class dragon::Tail::<dragon::Mcp::MAX_CHANNELS>+;
#pragma link C++ class dragon::Tail<dragon::TdcChannel<1> >+;

#pragma link C++ class dragon::Head<vme::Io32>+;
#pragma link C++ class dragon::Head<vme::V1190>+;
#pragma link C++ class dragon::Head<dragon::Bgo::MAX_CHANNELS>+;
#pragma link C++ class dragon::Head<dragon::TdcChannel<1> >+;

#pragma link C++ function dragon::Scaler::plot<TTree, TH1>(TTree*, const char*, const char*, Option_t*);
#pragma link C++ function dragon::Scaler::set_aliases(TTree*, Bool_t);
#pragma link C++ function dragon::Epics::set_aliases(TTree*);

#pragma link C++ class midas::Event::Header+;

#pragma link C++ class midas::Xml+;
#pragma link C++ class midas::Odb+;
#pragma link C++ class midas::Database+;
#pragma link C++ class mxml_struct+;

#pragma link C++ class tstamp::Diagnostics+;

#pragma link C++ defined_in ../src/Vme.hxx;
#pragma link C++ namespace vme;
#pragma link C++ class vme::V1190::Channel+;
#pragma link C++ class vme::Io32::Tsc4+;
#pragma link C++ class vme::V1190::Fifo+;
#pragma link C++ class vme::Io32+;
#pragma link C++ class vme::V1190+;
#pragma link C++ class vme::V792+;

#pragma link C++ defined_in ../src/Dragon.hxx;
#pragma link C++ namespace dragon;
#pragma link C++ class TAtomicMassTable+;
#pragma link C++ class dragon::RunParameters+;
#pragma link C++ class dragon::Bgo+;
#pragma link C++ class dragon::Dsssd+;
#pragma link C++ class dragon::IonChamber+;
#pragma link C++ class dragon::Mcp+;
#pragma link C++ class dragon::SurfaceBarrier+;
#pragma link C++ class dragon::NaI+;
#pragma link C++ class dragon::Ge+;
#pragma link C++ class dragon::TdcChannel<1>+;
#pragma link C++ class dragon::TdcChannel<MAX_RF_HITS>+;
#pragma link C++ class dragon::HiTof+;
#pragma link C++ class dragon::Head+;
#pragma link C++ class dragon::Tail+;
#pragma link C++ class dragon::Coinc+;
#pragma link C++ class dragon::Epics+;
#pragma link C++ class dragon::Scaler+;
#pragma link C++ class dragon::Unpacker+;
#pragma link C++ class dragon::Constants+;
#pragma link C++ class dragon::LinearFitter+;
#pragma link C++ class dragon::BeamNorm+;
#pragma link C++ class dragon::RossumData+;

#pragma link C++ class dragon::ASelector+;
#pragma link C++ class dragon::HeadSelector+;
#pragma link C++ class dragon::TailSelector+;
#pragma link C++ class dragon::CoincSelector+;
#pragma link C++ class dragon::ScalerSelector+;
#pragma link C++ class dragon::NoData+;

#pragma link C++ namespace m2r;
#pragma link C++ namespace midas;
#pragma link C++ namespace rbdragon;
#pragma link C++ namespace timestamp;

#pragma link C++ nestedclasses;
#pragma link C++ nestedtypedef;

#endif // __CINT__
