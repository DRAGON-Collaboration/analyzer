/// \file DSSSD.hxx
/// \brief Defines the DRAGON Double Sided Silicon Strip Detector (DSSSD) class.
#ifndef DRAGON_DSSSD_HXX
#define DRAGON_DSSSD_HXX
#include <stdint.h>
#include "utils/VariableStructs.hxx"


namespace vme {
class V792;
class V1190;
typedef V792 V785;
}


namespace dragon {

class Tail;

/// Double-Sided Silicon Strip Detector
class DSSSD {
public:
	/// Number of channels
	static const int MAX_CHANNELS = 32; //!

public:
	/// DSSSD Variables Class
	class Variables {
 private:
		/** @cond */
 PRIVATE:
  	/** @endcond */

		/// Adc variables for the energy signals
		utils::AdcVariables<32> adc;

		/// Tdc variables
		utils::TdcVariables<1> tdc;

 public:
		/// Constructor, sets data to generic values
		Variables();

		/// Set variable values from an ODB file
		void set(const char* odb_file);

		/// Reset all variable values to defaults
		void reset();

		/// Allows DSSSD class access to internals
		friend class dragon::DSSSD;
	};

	/// Instance of DSSSD::Variables
	DSSSD::Variables variables; //!

private:
	/** @cond */
PRIVATE:
	/** @endcond */
	/// Calibrated energy signals
	double ecal[MAX_CHANNELS]; //#

	/// Highest energy signal in the front strips (0 - 15)
	double efront;      //#

	/// Highest energy signal in the back strips (16 - 31)
	double eback;       //#

	/// Which strip was hit in the front strips
	uint32_t hit_front; //#

	/// Which strip was hit in the back strips
	uint32_t hit_back;  //#

	/// Calibrated time signal
	double tcal;        //#

public:
	/// Constructor, initialize data
	DSSSD();

	/// Reset all data to VME::none
	void reset();

	/// Read data from vme modules
	void read_data(const vme::V785 adcs[], const vme::V1190& tdc);

	/// Performs energy and time calibrations
	void calculate();

	/// Allow ToF calculation access to internals
	friend class dragon::Tail;
};

} // namespace dragon


#ifdef __MAKECINT__
#pragma link C++ class utils::AdcVariables<dragon::DSSSD::MAX_CHANNELS>+;
#pragma link C++ class utils::TdcVariables<1>+;
#endif

#endif
