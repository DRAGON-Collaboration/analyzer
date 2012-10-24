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

#pragma link C++ class dragon;
#pragma link C++ class vme;

#pragma link C++ class dragon::Coinc+;
#pragma link C++ class dragon::Tail+;
#pragma link C++ class dragon::MCP+;
#pragma link C++ class dragon::DSSSD+;
#pragma link C++ class dragon::IonChamber+;
#pragma link C++ class dragon::SurfaceBarrier+;
#pragma link C++ class dragon::NaI+;
#pragma link C++ class dragon::Ge+;

#pragma link C++ class dragon::Tail::Variables+;
#pragma link C++ class dragon::MCP::Variables+;
#pragma link C++ class dragon::DSSSD::Variables+;
#pragma link C++ class dragon::IonChamber::Variables+;
#pragma link C++ class dragon::SurfaceBarrier::Variables+;
#pragma link C++ class dragon::NaI::Variables+;
#pragma link C++ class dragon::Ge::Variables+;

#pragma link C++ class dragon::Head+;
#pragma link C++ class dragon::Bgo+;
#pragma link C++ class dragon::Bgo::Variables+;

#pragma link C++ class vme::V792+;
#pragma link C++ class vme::V785+;
#pragma link C++ class vme::V1190+;
#pragma link C++ class vme::IO32+;

#pragma link C++ class midas::Event::Header+;

#pragma link C++ class midas::Xml+;
#pragma link C++ class midas::Odb+;

/*
	for j in Adc Tdc
	do for i in {1..32}
	do echo "#pragma link C++ class dragon::${j}Variables<${i}>+;"
	done
	done
*/
#pragma link C++ class dragon::AdcVariables<1>+;
#pragma link C++ class dragon::AdcVariables<2>+;
#pragma link C++ class dragon::AdcVariables<3>+;
#pragma link C++ class dragon::AdcVariables<4>+;
#pragma link C++ class dragon::AdcVariables<5>+;
#pragma link C++ class dragon::AdcVariables<6>+;
#pragma link C++ class dragon::AdcVariables<7>+;
#pragma link C++ class dragon::AdcVariables<8>+;
#pragma link C++ class dragon::AdcVariables<9>+;
#pragma link C++ class dragon::AdcVariables<10>+;
#pragma link C++ class dragon::AdcVariables<11>+;
#pragma link C++ class dragon::AdcVariables<12>+;
#pragma link C++ class dragon::AdcVariables<13>+;
#pragma link C++ class dragon::AdcVariables<14>+;
#pragma link C++ class dragon::AdcVariables<15>+;
#pragma link C++ class dragon::AdcVariables<16>+;
#pragma link C++ class dragon::AdcVariables<17>+;
#pragma link C++ class dragon::AdcVariables<18>+;
#pragma link C++ class dragon::AdcVariables<19>+;
#pragma link C++ class dragon::AdcVariables<20>+;
#pragma link C++ class dragon::AdcVariables<21>+;
#pragma link C++ class dragon::AdcVariables<22>+;
#pragma link C++ class dragon::AdcVariables<23>+;
#pragma link C++ class dragon::AdcVariables<24>+;
#pragma link C++ class dragon::AdcVariables<25>+;
#pragma link C++ class dragon::AdcVariables<26>+;
#pragma link C++ class dragon::AdcVariables<27>+;
#pragma link C++ class dragon::AdcVariables<28>+;
#pragma link C++ class dragon::AdcVariables<29>+;
#pragma link C++ class dragon::AdcVariables<30>+;
#pragma link C++ class dragon::AdcVariables<31>+;
#pragma link C++ class dragon::AdcVariables<32>+;
#pragma link C++ class dragon::TdcVariables<1>+;
#pragma link C++ class dragon::TdcVariables<2>+;
#pragma link C++ class dragon::TdcVariables<3>+;
#pragma link C++ class dragon::TdcVariables<4>+;
#pragma link C++ class dragon::TdcVariables<5>+;
#pragma link C++ class dragon::TdcVariables<6>+;
#pragma link C++ class dragon::TdcVariables<7>+;
#pragma link C++ class dragon::TdcVariables<8>+;
#pragma link C++ class dragon::TdcVariables<9>+;
#pragma link C++ class dragon::TdcVariables<10>+;
#pragma link C++ class dragon::TdcVariables<11>+;
#pragma link C++ class dragon::TdcVariables<12>+;
#pragma link C++ class dragon::TdcVariables<13>+;
#pragma link C++ class dragon::TdcVariables<14>+;
#pragma link C++ class dragon::TdcVariables<15>+;
#pragma link C++ class dragon::TdcVariables<16>+;
#pragma link C++ class dragon::TdcVariables<17>+;
#pragma link C++ class dragon::TdcVariables<18>+;
#pragma link C++ class dragon::TdcVariables<19>+;
#pragma link C++ class dragon::TdcVariables<20>+;
#pragma link C++ class dragon::TdcVariables<21>+;
#pragma link C++ class dragon::TdcVariables<22>+;
#pragma link C++ class dragon::TdcVariables<23>+;
#pragma link C++ class dragon::TdcVariables<24>+;
#pragma link C++ class dragon::TdcVariables<25>+;
#pragma link C++ class dragon::TdcVariables<26>+;
#pragma link C++ class dragon::TdcVariables<27>+;
#pragma link C++ class dragon::TdcVariables<28>+;
#pragma link C++ class dragon::TdcVariables<29>+;
#pragma link C++ class dragon::TdcVariables<30>+;
#pragma link C++ class dragon::TdcVariables<31>+;
#pragma link C++ class dragon::TdcVariables<32>+;
