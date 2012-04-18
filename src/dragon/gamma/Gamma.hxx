/// \file Gamma.hxx
/// \brief Defines classes for DRAGON head (gamma) detectors.
#ifndef DRAGON_GAMMA_HXX
#define DRAGON_GAMMA_HXX
#include "dragon/gamma/Bgo.hxx"

namespace dragon {

namespace gamma {

/// All DRAGON gamma ray detectors
/*! Currently this is just the BGO array, but in the future we may add others
 *  (germanium, BaF, etc.) */
class Gamma {
public:
// ==== Data ==== //
	 /// Gamma electronics modules
	 dragon::gamma::Modules modules; //#

   /// Event counter
	 int32_t evt_count; //#

// ==== Detectors ==== //
	 /// Bgo array
	 Bgo bgo;

// ==== Methods ==== //	 
	 /// Constructor, initializes data values
	 Gamma();

	 /// Destructor, nothing to do
	 ~Gamma() { }

	 /// Copy constructor
	 Gamma(const Gamma& other);

	 /// Equalivalency operator
	 Gamma& operator= (const Gamma& other);

   /// Sets all data values to vme::NONE
	 void reset();

	 /// Unpack midas event data
	 /// \param [in] event Reference to a Midas event structure
	 void unpack(TMidasEvent& event);

	 /// \brief Read data from modules into detector classes
	 void read_data();
};
	 

} // namespace gamma

} // namespace dragon


#endif
