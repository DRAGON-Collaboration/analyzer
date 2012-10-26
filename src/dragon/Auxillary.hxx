/// \file Auxillary.hxx
/// \author G. Christian
/// \brief Defines classes for Auxillary Heavy-Ion detectors.
#ifndef DRAGON_HION_AUXILLARY_HXX
#define DRAGON_HION_AUXILLARY_HXX
#include <stdint.h>
#include "VariableStructs.hxx"

namespace vme {
class V792;
class V1190;
typedef V792 V785;
}


/// Encloses all DRAGON classes
namespace dragon {

/// Sodium Iodide (NaI) detectors
class NaI {
// Class global constants //
public:
	/// Number of detectors
	static const int MAX_CHANNELS = 2; //!

public:
	/// NaI variables
	class Variables {
 private:
		/** @cond */
 PRIVATE:
		/** @endcond */

		/// Adc variables
		AdcVariables<MAX_CHANNELS> adc;

 public:
		/// Constructor, sets data to generic values
		Variables();

		/// Set variable values to defaults
		void reset();

		/// Set variable values from an ODB file
		void set(const char* odb_file);

		/// Allow NaI class access to internals
		friend class dragon::NaI;
	};

	/// Variables instance
	Variables variables; //!

private:
/** @cond */
PRIVATE:
/** @endcond */

	/// Calibrated energy signals
	double ecal[MAX_CHANNELS]; //#

public:
	/// Constructor, initialize data
	NaI();

	/// Reset all data to vme::NONE
	void reset();

	/// Read event data from vme modules
	void read_data(const vme::V785 adcs[], const vme::V1190&);

	/// Do energy calibrations
	void calculate();
};


/// Germanium (Ge) detector
class Ge {

// Subclasses //
public:
	/// Ge variables
	class Variables {

		// Class data //
 private:
		/** @cond */
 PRIVATE:

		/// ADC variables
		AdcVariables<1> adc;

 public:
		/// Constructor, sets data to generic values
		Variables();

		/// Reset variables to default values
		void reset();

		/// Set variable values from an ODB file
		void set(const char* odb_file);
		
		/// Allow Ge class access to internals
		friend class dragon::Ge;
	};

	/// Variables instance
	Variables variables; //!

private:
	/** @cond */
PRIVATE:
	/** @endcond */

	/// Calibrated energy signal
	double  ecal; //#

public:
	/// Constructor, initialize data
	Ge();

	/// Reset all data to vme::NONE
	void reset();

	/// Read event data from the ADC
	void read_data(const vme::V785 adcs[], const vme::V1190&);

	/// Do energy calibrations, pedestal subtractions
	void calculate();
};

} // namespace dragon


#ifdef __MAKECINT__
#pragma link C++ class dragon::AdcVariables<dragon::NaI::MAX_CHANNELS>+;
#pragma link C++ class dragon::AdcVariables<1>+;
#endif

#endif // include guard
