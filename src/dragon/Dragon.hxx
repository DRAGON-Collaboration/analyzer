/// \file Dragon.hxx
/// \brief Defines a class for the complete DRAGON system (head + tail)
#ifndef DRAGON_HXX
#define DRAGON_HXX
#include "Modules.hxx"
#include "HeavyIon.hxx"
#include "Head.hxx"

/// Encloses all DRAGON classes
namespace dragon {

/// A complete Dragon (coincidence) event
class Dragon {

public:
	/// Head (gamma-ray) part of the event
	dragon::Head head;

	/// Heavy ion detectors
	hion::HeavyIon hi;


// ===== Methods ===== //
	/// Constructor, initialize data
	Dragon();

	/// Reset all modules
	void reset();

	/// \brief Reads all variable values from an ODB (file or online)
	/// \param [in] odb_file Name of the ODB file; passing \c "online" looks at the online ODB.
	void set_variables(const char* odb_file);

	/// \brief Copy data from head and tail coincidence events
	/// \param [in] head Head (gamma-ray) event
	/// \param [in] tail Tail (heavy-ion) event
	void read_event(const dragon::Head& head_, const dragon::hion::HeavyIon& tail_);
};

} // namespace dragon


#endif
