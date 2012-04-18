/// \file HeavyIon.hxx
/// \brief Defines the DRAGON tail (heavy ion) detector classes
#ifndef DRAGON_HEAVY_ION
#define DRAGON_HEAVY_ION
#include "heavy_ion/MCP.hxx"
#include "heavy_ion/DSSSD.hxx"
#include "heavy_ion/IonChamber.hxx"

namespace dragon {

namespace hion {

/*
/// Surface barrier detectors
struct SurfaceBarrier {
// ==== Const Data ==== //
	 /// Number of detectors
	 static const int nch = 2; //!

// ==== Classes ==== //
	 /// Surface Barrier Variables
	 struct Variables {
			/// Maps detector to adc module number
			int module[SurfaceBarrier::nch];

			/// Maps detector adc channel number
			int ch[SurfaceBarrier::nch];

			/// Constructor, sets data to generic values
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
	 /// Variables instance
	 MCP::Variables variables; //!
	 
	 /// Anode signals
	 int16_t anode[nch]; //#
	 
	 /// TAC (time) signal
	 int16_t tac; //#

	 /// x-position
	 double x; //#

	 /// y-position
	 double y; //#

	 /// Constructor, initialize data
	 MCP();

	 /// Destructor, nothing to do
	 ~MCP() { }

	 /// Copy constructor
	 MCP(const MCP& other);

	 /// Equivalency operator
	 MCP& operator= (const MCP& other);

	 /// Reset all data to VME::none
	 void reset();
	 
	 /// \brief Read midas event data
	 /// \param modules Heavy-ion module structure
	 void read_data(const dragon::hion::Modules& modules);
};
*/
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
