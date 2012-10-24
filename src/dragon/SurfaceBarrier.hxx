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
class SurfaceBarrier {
	// Class global constants //
public:
	/// Number of detectors
	static const int MAX_CHANNELS = 2; //!

	// Subclases //
public:
	/// Surface Barrier Variables
	class Variables {
		// Class data //
 private:
		/** @cond */
 PRIVATE:
		/** @endcond */
		/// Maps detector to adc module number
		int module[MAX_CHANNELS];

		/// Maps detector adc channel number
		int ch[MAX_CHANNELS];

		// Methods //
 public:
		/// Constructor, sets data to generic values
		Variables();

		/// Set variable values from an ODB file
		void set(const char* odb_file);

		/// Allow SurfaceBarrier class access to internals
		friend class dragon::SurfaceBarrier;
	};

	/// Variables instance
	SurfaceBarrier::Variables variables; //!
	 
	// Class data //
private:
	/** @cond */
PRIVATE:
	/** @endcond */
	/// Charge (energy) signals
	int16_t q[MAX_CHANNELS]; //#

	// Methods //
public:
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
