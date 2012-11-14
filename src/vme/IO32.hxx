/// \file Io32.hxx
/// \brief Defines structures and routines for IO32 FPGA data
#ifndef DRAGON_VME_IO32_HXX
#define DRAGON_VME_IO32_HXX
#include <stdint.h>


namespace midas { class Event; }

/// Encloses all VME module classes
namespace vme {

/// IO32 FPGA
class Io32 {
private:
	/** @cond */
PRIVATE:
	/** @endcond */
	/// Header and version
	uint32_t header;

	/// Event number, counting from 0
	uint32_t trig_count;

	/// Trigger timestamp
	uint32_t tstamp;

	/// Readout start time
	uint32_t start;

	/// Readout end time
	uint32_t end;

	/// Trigger latency
	uint32_t latency;

	/// Readout elapsed time
	uint32_t read_time;

	/// Busy elapsed time
	uint32_t busy_time;

	/// Dragon trigger latch code
	/*! Bitmask defining which signal generated the trigger */
	uint32_t trigger_latch;

public:
	/// Unpack all data from the io32
	bool unpack(const midas::Event& event, const char* bankName, bool reportMissing = false);

	/// Set all data fields to default values (== 0).
	void reset();

	/// Calls reset()
	Io32() { reset(); }

	/// Empty
	~Io32() { }
};

} // namespace vme




#endif
