/// \file Bgo.hxx
/// \author G. Christian
/// \brief Defines a class for the DRAGON Bgo array
#ifndef DRAGON_BGO_HXX
#define DRAGON_BGO_HXX
#include <stdint.h>
#include "utils/VariableStructs.hxx"

namespace vme {
class V792;
class V1190;
}

namespace dragon {

class Head;

/// The BGO array
class Bgo {
public:
	/// Number of channels in the BGO array
	static const int MAX_CHANNELS = 30; //!

public:
	/// Bgo variables
	class Variables {
 private:
		/** @cond */
 PRIVATE:
		/** @endcond */

		/// Adc variables
		utils::AdcVariables<MAX_CHANNELS> adc;

		/// Tdc variables
		utils::TdcVariables<MAX_CHANNELS> tdc;

		/// Detector positions in space
		utils::PositionVariables<MAX_CHANNELS> pos;

 public:
		/// Sets data to defaults
		Variables();

		/// Set values to defaults
		void reset();

		/// Set variable values from an ODB file
		void set(const char* odb_file);

		/// Allow Bgo class access to internals
		friend class dragon::Bgo;
	};

	/// Instance of Bgo::Variables for mapping digitizer ch -> bgo detector
	Variables variables;          //!
	 
private:
	/** @cond */
PRIVATE:
	/** @endcond */
	/// Calibrated energies
	double ecal[MAX_CHANNELS];    //#

	/// Calibrates times
	double tcal[MAX_CHANNELS];    //#

	/// Sorted (high->low) energies
	double esort[MAX_CHANNELS];   //#

	/// Sum of all \e valid energies
	double sum; //#

	/// Which detector was the highest energy hit
	int hit0;  //#

	/// x position of the highest energy hit
	double x0; //#

	/// y position of the highest energy hit
	double y0; //#

	/// z position of the highest energy hit
	double z0; //#

	/// time of the highest energy hit
	double t0; //#

public:
	/// Constructor, initializes data values
	Bgo();

	/// Sets all data values to dragon::NO_DATA
	void reset();

	/// Read adc & tdc data
	void read_data(const vme::V792& adc, const vme::V1190& tdc);

	/// Do higher-level parameter calculations
	void calculate();

	/// Allow parent class access to internals
	friend class dragon::Head;
};

} // namespace dragon


#ifdef __MAKECINT__
#pragma link C++ class utils::AdcVariables<dragon::Bgo::MAX_CHANNELS>;
#pragma link C++ class utils::TdcVariables<dragon::Bgo::MAX_CHANNELS>;
#pragma link C++ class utils::PositionVariables<dragon::Bgo::MAX_CHANNELS>;
#endif


#endif // include guard
