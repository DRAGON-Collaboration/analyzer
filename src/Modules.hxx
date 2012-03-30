/// \file Modules.hxx
/// \brief Defines classes that encapsulate the VME modules used to read out data
///  for each DRAGON sub-system (heavy ion and gamma).
#ifndef DRAGON_MODULES_HXX
#define DRAGON_MODULES_HXX
#include "vme/vme.hxx"

namespace dragon {

namespace gamma {

/// Encapsulates all VME modules used to read out gamma-ray data.
class Modules {
private:
	 /// CAEN v792 qdc (32 channel, integrating).
	 vme::caen::V792 v792;

   /// CAEN v1190b tdc (64 channel).
	 vme::caen::V1190b v1190b;

	 /// I032 FPGA
	 vme::IO32 io32;

public:
	 /// Initialize all modules
	 Modules();

	 /// Nothing to do
	 ~Modules() { }

   /// Unpack Midas event data into module data structures.
	 /// \param [in] event Reference to the MidasEvent object being unpacked
	 void unpack(const TMidasEvent& event);	 
};

} // namespace gamma

namespace heavy_ion {

/// Encapsulates all VME modules used to read out heavy-ion data.
class Modules {
private:
	 /// Caen v785 adcs (32 channel, peak-sensing, x2)
	 vme::caen::V785 v785[2];

	 /// CAEN v1190b TDC
	 vme::caen::V1190b v1190b;

	 /// IO32 FPGA
	 vme::IO32 io32;

public:
	 /// Initialize all modules
	 Modules();

   /// Nothing to do
	 ~Modules() { }

	 /// Unpack Midas event data into module data structures.
	 /// \param [in] event Reference to the MidasEvent object being unpacked
	 void unpack(TMidasEvent& event);	 
};

} // namespace heavy_ion

} // namespace dragon




#endif
