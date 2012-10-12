/// \file IonChamber.hxx
/// \brief Defines a class for the DRAGON Ionizatin Chamber detector
#ifndef DRAGON_ION_CHAMBER_HXX
#define DRAGON_ION_CHAMBER_HXX
#include <stdint.h>


namespace vme {
class V792;
class V1190;
typedef V792 V785;
}

namespace dragon {

/// Ionization chamber
class IonChamber {

public:
	/// Number of anodes
	static const int MAX_CHANNELS = 4; //!

public:
	/// Ion chamber variables
	struct Variables {
		/// Maps anode channel number to adc module number
		int anode_module[MAX_CHANNELS];

		/// Maps anode channel number to adc channel number
		int anode_ch[MAX_CHANNELS];

		/// Maps tof to TDC channel number
		int tof_ch;

		/// Constructor, sets data to generic values
		Variables();

		/// \brief Set variable values from an ODB file
		/// \param [in] odb_file Path of the odb file from which you are extracting variable values
		/// \todo Needs to be implemented once ODB is set up
		void set(const char* odb_file);
	};

public:
	/// Variables instance
	Variables variables; //!
	 
	/// Raw anode signals
	int16_t anode[MAX_CHANNELS]; //#

	/// Raw time signal (channels???)
	int16_t tof; //#

	/// Sum of anode signals
	double sum;

public:
	/// Constructor, initialize data
	IonChamber();

	/// Reset all data to VME::none
	void reset();

	/// \brief Read midas event data
	/// \param modules Heavy-ion module structure
	/// \param [in] v1190_trigger_ch Channel number of the v1190b trigger
	void read_data(const vme::V785 adcs[], const vme::V1190& tdc);

	/// \brief Calculate higher-level parameters
	void calculate();
};

} // namespace dragon


#endif
