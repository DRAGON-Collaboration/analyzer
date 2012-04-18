/// \file Dragon.hxx
/// \brief Defines a class for the complete DRAGON system (head + tail)
#ifndef DRAGON_HXX
#define DRAGON_HXX
#include "Modules.hxx"
#include "heavy_ion/HeavyIon.hxx"
#include "gamma/Gamma.hxx"

/// Encloses all DRAGON classes
namespace dragon {

/// A complete Dragon (coincidence) event
class Dragon {
public:
// ===== Detector data ===== //
   /// Bgo detector
	 gamma::Gamma gamma;

	 /// Heavy ion detectors
	 hion::HeavyIon hi;


// ===== Methods ===== //
	 /// Constructor, initialize data
	 Dragon();

   /// Destructor, nothing to do
	 ~Dragon() { }

	 /// Copy constructor
	 Dragon(const Dragon& other);

	 /// Equivalency operator
	 Dragon& operator= (const Dragon& other);

	 /// Reset all modules
	 void reset();

	 /// Copy data from head and tail coincidence events
	 /// \param [in] head Head (gamma-ray) event
	 /// \param [in] tail Tail (heavy-ion) event
	 void read_event(const dragon::gamma::Gamma& head, const dragon::hion::HeavyIon& tail);
};

} // namespace dragon


#endif
