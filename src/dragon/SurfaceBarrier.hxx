/// \file SurfaceBarrier.hxx
/// \author G. Christian
/// \brief Defines a class for DRAGON surface barrier (Si) detectors
#ifndef DRAGON_SURFACE_BARRIER_HXX
#define DRAGON_SURFACE_BARRIER_HXX
#include <stdint.h>
#include "utils/VariableStructs.hxx"

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
		
		/// Adc variables
		utils::AdcVariables<MAX_CHANNELS> adc;

		// Methods //
 public:
		/// Constructor, sets data to generic values
		Variables();

		/// Sets data to defaults
		void reset();

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
	/// Energy signals
	double ecal[MAX_CHANNELS]; //#

	// Methods //
public:
	/// Constructor, initialize data
	SurfaceBarrier();

	/// Reset all data to vme::NONE
	void reset();
	 
	/// Read midas event data
	void read_data(const vme::V785 adcs[], const vme::V1190&);

	/// Final energy calculation
	void calculate();
};

} // namespace dragon


#ifdef __MAKECINT__
#pragma link C++ class utils::AdcVariables<dragon::SurfaceBarrier::MAX_CHANNELS>+;
#endif


#endif // include guard
