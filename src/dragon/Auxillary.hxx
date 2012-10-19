/// \file Auxillary.hxx
/// \author G. Christian
/// \brief Defines classes for Auxillary Heavy-Ion detectors.
#ifndef DRAGON_HION_AUXILLARY_HXX
#define DRAGON_HION_AUXILLARY_HXX
#include <stdint.h>

namespace vme {
class V792;
class V1190;
typedef V792 V785;
}


namespace dragon {

/// Sodium Iodide (NaI) detectors
class NaI {

public:
	/// Number of detectors
	static const int MAX_CHANNELS = 2; //!

public:
	/// NaI variables
	class Variables {
 PRIVATE:
		/// Maps detector to adc module number
		int module[MAX_CHANNELS];
			
		/// Maps detector to adc channel number
		int ch[MAX_CHANNELS];

		/// Calibration slope
		double slope[MAX_CHANNELS];

		/// Calibration offset
		double offset[MAX_CHANNELS];

 public:
		/// Constructor, sets data to generic values
		Variables();

		/// \brief Set variable values from an ODB file
		/// \param [in] odb_file Path of the odb file from which you are extracting variable values
		/// \todo Needs to be implemented once ODB is set up
		void set(const char* odb_file);

		/// Allow NaI class access to internals
		friend class dragon::NaI;
	};

	/// Variables instance
	Variables variables; //!

PRIVATE:
	/// Raw energy signals
	int16_t qraw[MAX_CHANNELS]; //#

	/// Calibrated energy signals
	double  qcal[MAX_CHANNELS]; //#

public:
	/// Constructor, initialize data
	NaI();

	/// Reset all data to vme::NONE
	void reset();

	/// \brief Read event data from \e modules structure
	/// \param [in] modules Heavy-ion modules structure
	void read_data(const vme::V785 adcs[], const vme::V1190& tdc);

	/// \brief Do energy calibrations
	void calculate();
};

/// Germanium (Ge) detector
class Ge {

public:
	/// Number of detectors
	static const int MAX_CHANNELS = 1; //!

public:
	/// Ge variables
	class Variables {
 PRIVATE:
		/// Maps detector to adc module number
		int module[MAX_CHANNELS];
			
		/// Maps detector to adc channel number
		int ch[MAX_CHANNELS];

		/// Calibration slope
		double slope[MAX_CHANNELS];

		/// Calibration offset
		double offset[MAX_CHANNELS];

 public:
		/// Constructor, sets data to generic values
		Variables();

		/// \brief Set variable values from an ODB file
		/// \param [in] odb_file Path of the odb file from which you are extracting variable values
		/// \todo Needs to be implemented once ODB is set up
		void set(const char* odb_file);
		
		/// Allow Ge class access to internals
		friend class dragon::Ge;
	};

	/// Variables instance
	Variables variables; //!

PRIVATE:
	/// Raw energy signals
	int16_t qraw[MAX_CHANNELS]; //#

	/// Calibrated energy signals
	double  qcal[MAX_CHANNELS]; //#

public:
	/// Constructor, initialize data
	Ge();

	/// Reset all data to vme::NONE
	void reset();

	/// \brief Read event data from \e modules structure
	/// \param [in] modules Heavy-ion modules structure
	void read_data(const vme::V785 adcs[], const vme::V1190& tdc);

	/// \brief Do energy calibrations
	void calculate();
};

} // namespace dragon
	 
#endif
