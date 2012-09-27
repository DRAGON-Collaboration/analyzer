/// \file Dragon.hxx
/// \author G. Christian
/// \brief Defines a class for the complete DRAGON system (head + tail)
#ifndef DRAGON_HXX
#define DRAGON_HXX
#include "Tail.hxx"
#include "Head.hxx"
#include "Modules.hxx"

/// Encloses all DRAGON classes
namespace dragon {

/// A complete Dragon (coincidence) event
class Dragon {

public:
	/// Head (gamma-ray) part of the event
	Head head;

	/// Heavy ion detectors
	Tail tail;

public:
	/// Initialize all data to defaults
	Dragon();

	/// Reset all modules (set data to defaults)
	void reset();

	/// Reads all variable values from an ODB (file or online)
	void set_variables(const char* odb_file);

	/// Copy data from head and tail coincidence events
	void read_event(const Head& head_, const Tail& tail_);
};

} // namespace dragon


#endif
