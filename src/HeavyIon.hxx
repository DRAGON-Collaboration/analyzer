/// \file HeavyIon.hxx
/// \brief Defines the DRAGON tail (heavy ion) detector classes
#ifndef DRAGON_HEAVY_ION
#define DRAGON_HEAVY_ION
#include "Modules.hxx"

namespace dragon {

namespace hion {

/// Double-Sided Silicon Strip Detector
struct DSSSD {
// ==== Const Data ==== //
	 /// Number of channels
	 static const int nch = 32; //!

// ==== Classes ==== //	 
	 /// DSSSD Variables
	 struct Variables {
			/// Maps qraw[i] to adc module number (in dragon::hion::Modules)
			int qdc_module[DSSSD::nch];

			/// Maps qraw[i] to adc channel number (in dragon::hion::Modules)
			int qdc_ch[DSSSD::nch];

			/// Maps tof to tdc channel number
			int tof_ch;

			/// Constructor, sets data to generic values (TBD)
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
	 /// Instance of DSSSD::Variables
	 DSSSD::Variables variables; //!

	 /// Raw energy signals
	 int16_t qraw[DSSSD::nch]; //#

	 /// Raw time signal (just one??)
	 int16_t tof; //#

// ==== Methods ==== //
	 /// Constructor, initialize data
	 DSSSD();

	 /// Destructor, nothing to do
	 ~DSSSD() { }

	 /// Copy constructor
	 DSSSD(const DSSSD& other);

	 ///Equivalency operator
	 DSSSD& operator= (const DSSSD& other);

	 /// Reset all data to VME::none
	 void reset();

	 /// \brief Read midas event data
	 /// \param [in] modules Heavy-ion module structure
	 /// \param [in] v1190_trigger_ch Channel number of the v1190b trigger
	 void read_data(const dragon::hion::Modules& modules, int v1190_trigger_ch);
};

/// Ionization chamber
struct IonChamber {
// ==== Const Data ==== //
	 /// Number of anodes
	 static const int nch = 4; //!

// ==== Classes ==== //
	 /// Ion chamber variables
	 struct Variables {
			/// Maps anode channel number to adc module number
			int anode_module[IonChamber::nch];

			/// Maps anode channel number to adc channel number
			int anode_ch[IonChamber::nch];

			/// Maps tof to TDC channel number
			int tof_ch;

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
	 Variables variables; //!
	 
	 /// Raw anode signals
	 int16_t anode[nch]; //#

	 /// Raw time signal (channels???)
	 int16_t tof; //#

	 /// Constructor, initialize data
	 IonChamber();

// ==== Methods ==== //
	 /// Destructor, nothing to do
	 ~IonChamber() { }

	 /// Copy constructor
	 IonChamber(const IonChamber& other);

	 /// Equivalency operator
	 IonChamber& operator= (const IonChamber& other);

	 /// Reset all data to VME::none
	 void reset();

	 /// \brief Read midas event data
	 /// \param modules Heavy-ion module structure
	 /// \param [in] v1190_trigger_ch Channel number of the v1190b trigger
	 void read_data(const dragon::hion::Modules& modules, int v1900_trigger_ch);
};

/// Micro channel plates
struct MCP {
// ==== Const Data ==== //
	 /// Number of anodes
	 static const int nch = 4; //!

// ==== Classes ==== //
	 /// MCP Variables
	 struct Variables {
			/// Maps anode channel to adc module number
			int anode_module[MCP::nch];

			/// Maps anode channel to adc channel number
			int anode_ch[MCP::nch];

			/// Maps TAC to adc module number
			int tac_module;

			/// Maps TAC to adc channel number
			int tac_ch;

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
