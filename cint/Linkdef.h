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
//! (note that you need to specify the fill path to the header file).
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
#ifdef __MAKECINT__
#include <RVersion.h>

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclasses;

#pragma link C++ class dragon::utils+;
#pragma link C++ class dragon+;
#pragma link C++ class vme+;

#pragma link C++ defined_in ../src/Vme.hxx;
#pragma link C++ class vme::V1190::Channel+;
#pragma link C++ defined_in ../src/Dragon.hxx;
#pragma link C++ defined_in ../src/utils/VariableStructs.hxx;

class TTree;
#pragma link C++ function dragon::Scaler::set_aliases(TTree*, const char*);
#pragma link C++ function dragon::Epics::set_aliases(TTree*);

#pragma link C++ class midas::Event::Header+;

#pragma link C++ class midas::Xml+;
#pragma link C++ class midas::Odb+;
#pragma link C++ class midas::Database+;
#pragma link C++ class mxml_struct+;

#pragma link C++ class tstamp::Diagnostics+;
#pragma link C++ defined_in ../src/TStamp.hxx;
#pragma link C++ defined_in ../src/utils/ErrorDragon.hxx;
#pragma link C++ defined_in ../src/utils/RootAnalysis.hxx;
#pragma link C++ defined_in ../src/utils/Selectors.hxx;
#pragma link C++ defined_in ../src/utils/Calibration.hxx;
#pragma link C++ defined_in ../src/utils/Uncertainty.hxx;
#pragma link C++ defined_in ../src/utils/TAtomicMass.h;
#pragma link C++ defined_in ../src/utils/LinearFitter.hxx;



#pragma link C++ class dragon::utils::AutoPtr<midas::Xml>+;
#pragma link C++ class dragon::utils::AutoPtr<dragon::RossumData>+;
#pragma link C++ class dragon::utils::AutoPtr<std::ifstream>+;

#if ROOT_VERSION_CODE >= ROOT_VERSION(5,30,0)
#pragma link C++ class std::auto_ptr<midas::Xml>+;
#pragma link C++ class std::auto_ptr<dragon::RossumData>+;
#pragma link C++ class std::auto_ptr<std::ifstream>+;
#endif

#pragma link C++ class dragon::utils::AdcVariables<dragon::Bgo::MAX_CHANNELS>+;
#pragma link C++ class dragon::utils::TdcVariables<dragon::Bgo::MAX_CHANNELS>+;
#pragma link C++ class dragon::utils::PositionVariables<dragon::Bgo::MAX_CHANNELS>+;
#pragma link C++ class dragon::utils::AdcVariables<dragon::Dsssd::MAX_CHANNELS>+;
#pragma link C++ class dragon::utils::TdcVariables<1>+;
#pragma link C++ class dragon::utils::AdcVariables<dragon::IonChamber::MAX_CHANNELS>+;
#pragma link C++ class dragon::utils::TdcVariables<1>+;
#pragma link C++ class dragon::utils::AdcVariables<dragon::SurfaceBarrier::MAX_CHANNELS>+;
#pragma link C++ class dragon::utils::AdcVariables<dragon::NaI::MAX_CHANNELS>+;
#pragma link C++ class dragon::utils::AdcVariables<1>+;

#pragma link C++ class dragon::Constants+;

#endif
