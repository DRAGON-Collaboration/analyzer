/// \file SurfaceBarrier.hxx
/// \author G. Christian
/// \brief Defines a class for DRAGON surface barrier (Si) detectors
#ifndef DRAGON_SURFACE_BARRIER_HXX
#define DRAGON_SURFACE_BARRIER_HXX
#include <stdint.h>

namespace vme {
class V792;
class V1190;
typedef V792 V785;
}


namespace dragon {

/// Surface barrier detectors
struct SurfaceBarrier {
// ==== Const Data ==== //
	/// Number of detectors
	static const int MAX_CHANNELS = 2; //!

// ==== Classes ==== //
	/// Surface Barrier Variables
	struct Variables {
		/// Maps detector to adc module number
		int module[MAX_CHANNELS];

		/// Maps detector adc channel number
		int ch[MAX_CHANNELS];

		/// Constructor, sets data to generic values
		Variables();

		/// \brief Set variable values from an ODB file
		/// \param [in] odb_file Path of the odb file from which you are extracting variable values
		/// \todo Needs to be implemented once ODB is set up
		void set(const char* odb_file);
	};

// ==== Data ==== //
	/// Variables instance
	SurfaceBarrier::Variables variables; //!
	 
	/// Charge (energy) signals
	int16_t q[MAX_CHANNELS]; //#

// ==== Methods ==== //
	/// Constructor, initialize data
	SurfaceBarrier();

	/// Reset all data to vme::NONE
	void reset();
	 
	/// \brief Read midas event data
	/// \param modules Heavy-ion module structure
	void read_data(const vme::V785 adcs[], const vme::V1190& tdc);

	/// Empty
	void calculate() { }
};

} // namespace dragon


#endif
