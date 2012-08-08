/// \file IO32.hxx
/// \brief Defines structures and routines for IO32 FPGA data
#ifndef DRAGON_VME_IO32_HXX
#define DRAGON_VME_IO32_HXX
#include <stdint.h>
#include "midas/TMidasEvent.h"

namespace vme {

/// \brief IO32 FPGA
struct IO32 {

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

	 /// Trigger timestamp (64-bit, accounts for 32 bit rollover)
	 uint64_t tstamp64;
	 
};

/// \brief Calculate 64-bit timestamp value
/// \details, Keeps track of 32-bit rollover to calculate timestamp to 64 bits
/// \param [in] tstamp32 32-bit timestamp value
/// \param [in] event_number Event counter
/// \returns 64-bit timestamp value
uint64_t calc_ts64(uint32_t tstamp32, uint32_t event_number);

/// \brief Unpack data from an IO32
/// \details Here is the portion of the MIDAS frontent where values are written:
/// \code
/// *pdata32++ = 0xaaaa0020;           // 0 - header and version
/// *pdata32++ = trig_count-1;         // 1 - event number, counting from 0
/// *pdata32++ = trig_time;            // 2 - trigger timestamp
/// *pdata32++ = start_time;           // 3 - readout start time
/// *pdata32++ = end_time;             // 4 - readout end time
/// *pdata32++ = start_time-trig_time; // 5 - trigger latency (trigger time - start of readout time)
/// *pdata32++ = end_time-start_time;  // 6 - readout elapsed time
/// *pdata32++ = end_time-trig_time;   // 7 - busy elapsed time
/// \endcode
/// \param [in] event The midas event to unpack
/// \param [in] bank Name of the bank to unpack
/// \param [out] module The module into which you are unpacking
/// \returns True if the event was successfully unpacked, false otherwise
bool unpack_io32(const TMidasEvent& event, const char* bank, vme::IO32& module);

} // namespace vme




#endif
