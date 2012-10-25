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
// Class global constants //
public:
	/// Number of anodes
	static const int MAX_CHANNELS = 4; //!

// Subclasses //
public:
	/// Ion chamber variables
	class Variables {
		// Data //
 private:
		/** @cond */
 PRIVATE:
		/** @endcond */

		/// Anode variables
		AdcVariables<MAX_CHANNELS> adc;

		/// Tdc variables
		TdcVariables<1> tdc;

		// Methods //
 public:
		/// Constructor, sets data to generic values
		Variables();

		/// Reset data to default values
		void reset();

		/// Set variable values from an ODB file
		void set(const char* odb_file);
		
		/// Allow IonChamber class access to internals
		friend class dragon::IonChamber;
	};

	/// Variables instance
	Variables variables; //!
	 
	// Class data //
private:
  /** @cond */
PRIVATE:
	/** @endcond */
	/// Calibrated anode signals
	double anode[MAX_CHANNELS]; //#

	/// Time signal
	double tcal; //#

	/// Sum of anode signals
	double sum;  //#

	// Methods //
public:
	/// Constructor, initialize data
	IonChamber();

	/// Reset all data to VME::none
	void reset();

	/// Read midas event data
	void read_data(const vme::V785 adcs[], const vme::V1190& tdc);

	/// Calculate higher-level parameters
	void calculate();
};

} // namespace dragon


#endif
