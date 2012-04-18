/// \file HeavyIon.hxx
/// \brief Defines the DRAGON tail (heavy ion) detector classes
#ifndef DRAGON_HEAVY_ION
#define DRAGON_HEAVY_ION
#include "dragon/heavy_ion/MCP.hxx"
#include "dragon/heavy_ion/DSSSD.hxx"
#include "dragon/heavy_ion/IonChamber.hxx"
#include "dragon/heavy_ion/SurfaceBarrier.hxx"

namespace dragon {

namespace hion {

struct HeavyIon {

// ==== Classes ==== //
	 /// Heavy ion variables
	 struct Variables {
			/// Channel of the V1190b TDC trigger
			int v1190_trigger_ch;
			
			/// Constructor, set data to generic values
			Variables();

			/// Destructor, nothing to do
			~Variables() { }

			/// Copy constructor
			Variables(const Variables& other);

			/// Equivalency operator
			Variables& operator= (const Variables& other);

			/// \brief Set variable values from an ODB file
			/// \param [in] odb_file Path of the odb file from which you are extracting variable values
			/// \todo Needs to be implemented once ODB is set up
			void set(const char* odb_file);
	 };

// ==== Data ==== //
	 /// Electronics modules
	 dragon::hion::Modules modules; //#

	 /// Event counter
	 int32_t evt_count; //#

// ==== Variables ==== //
	 /// HeavyIon::Variables instance
	 Variables variables; //!

// ==== Detectors ==== //
#ifndef DRAGON_OMIT_DSSSD
	 /// DSSSD detector
	 DSSSD dsssd;
#endif

#ifndef DRAGON_OMIT_IC
	 /// Ionization chamber
	 IonChamber ic;
#endif

	 /// MCPs
	 MCP mcp;

	 /// Surface barrier detectors
	 SurfaceBarrier sb;

// ==== Methods ==== //	 
	 /// Constructor, initializes data values
	 HeavyIon();

	 /// Destructor, nothing to do
	 ~HeavyIon() { }

	 /// Copy constructor
	 HeavyIon(const HeavyIon& other);
	 
	 ///Equivalency operator
	 HeavyIon& operator= (const HeavyIon& other);

   /// Sets all data values to vme::NONE
	 void reset();

	 /// \brief Unpack MIDAS event data
	 /// \param [in] event Reference to a Midas event structure
	 void unpack(TMidasEvent& event);

	 /// \brief Read data from a midas event.
	 void read_data();
};

} // namespace hion

} // namespace dragon



#endif
