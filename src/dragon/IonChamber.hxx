/// \file IonChamber.hxx
/// \brief Defines a class for the DRAGON Ionizatin Chamber detector
#ifndef DRAGON_ION_CHAMBER_HXX
#define DRAGON_ION_CHAMBER_HXX
#include "dragon/Modules.hxx"
#include "vme/Vme.hxx"


namespace dragon {

namespace hion {

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

	 /// Sum of anode signals
	 double sum;

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

	 /// \brief Calculate higher-level parameters
	 void calculate();
};

} // namespace hion

} // namespace dragon


#endif
