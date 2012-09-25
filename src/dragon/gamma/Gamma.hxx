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
#ifdef DISPLAY_MODULES
	dragon::gamma::Modules modules; //#
#else
	dragon::gamma::Modules modules; //!
#endif

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

	/// \brief Reads all variable values from an ODB (file or online)
	/// \param [in] odb_file Name of the ODB file; passing \c "online" looks at the online ODB.
	void set_variables(const char* odb_file);

	/// \brief Unpack midas event data into \e modules data structure
	/// \param [in] event Reference to a Midas event structure
	void unpack(dragon::MidasEvent& event);

	/// \brief Read data from \e modules into detector classes
	void read_data();

	/// \brief Calculate higher-level data for each detector, or across detectors
	void calculate();
};
	 

} // namespace gamma

} // namespace dragon


#endif
