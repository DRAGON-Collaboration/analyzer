/// \file SurfaceBarrier.hxx
/// \brief Defines a class for DRAGON surface barrier (Si) detectors
#ifndef DRAGON_SURFACE_BARRIER_HXX
#define DRAGON_SURFACE_BARRIER_HXX
#include "Modules.hxx"
#include "vme/Vme.hxx"


namespace dragon {

namespace hion {

/// Surface barrier detectors
struct SurfaceBarrier {
// ==== Const Data ==== //
	 /// Number of detectors
	 static const int nch = 2; //!

// ==== Classes ==== //
	 /// Surface Barrier Variables
	 struct Variables {
			/// Maps detector to adc module number
			int module[SurfaceBarrier::nch];

			/// Maps detector adc channel number
			int ch[SurfaceBarrier::nch];

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
	 SurfaceBarrier::Variables variables; //!
	 
	 /// Charge (energy) signals
	 int16_t q[nch]; //#
	 
	 /// Constructor, initialize data
	 SurfaceBarrier();

	 /// Destructor, nothing to do
	 ~SurfaceBarrier() { }

	 /// Copy constructor
	 SurfaceBarrier(const SurfaceBarrier& other);

	 /// Equivalency operator
	 SurfaceBarrier& operator= (const SurfaceBarrier& other);

	 /// Reset all data to VME::none
	 void reset();
	 
	 /// \brief Read midas event data
	 /// \param modules Heavy-ion module structure
	 void read_data(const dragon::hion::Modules& modules);
};


} // namespace hion

} // namespace dragon


#endif
