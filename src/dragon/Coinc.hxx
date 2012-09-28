/// \file Coinc.hxx
/// \author G. Christian
/// \brief Defines a class for the a complete DRAGON coincidence event.
#ifndef DRAGON_COINC_HXX
#define DRAGON_COINC_HXX
#include "Tail.hxx"
#include "Head.hxx"

/// Encloses all DRAGON classes
namespace dragon {

/// A complete Dragon (coincidence) event
class Coinc {

public:
	/// Head (gamma-ray) part of the event
	Head head;

	/// Tail (heavy-ion) part of the event
	Tail tail;

public:
	/// Empty
	Coinc();

	/// Construct from a head and tail event
	Coinc(const Head& head, const Tail& tail);

	/// Reset all modules (set data to defaults)
	void reset();

	/// Reads all variable values from an ODB (file or online)
	void set_variables(const char* odb_file);

	/// Copy data from head and tail coincidence events
	void read_event(const Head& head_, const Tail& tail_);
};

} // namespace dragon


#endif
