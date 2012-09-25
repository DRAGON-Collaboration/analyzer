#include <stdint.h>
#include <cmath>
#include <limits>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include "utils/Bits.hxx"
#include "utils/Error.hxx"
#include "MidasEvent.hxx"
#include "definitions.h"

namespace {

inline uint64_t read_timestamp (uint64_t lower, uint64_t upper)
{
	return (lower & READ30) | (upper << 30);
}

}

// ========= Class dragon::MidasEvent ========= //

dragon::MidasEvent::MidasEvent(const char* tsbank, const void* header, const void* data, int size):
	fCoincWindow(10.),
	fClock (std::numeric_limits<uint64_t>::max()),
	fTriggerTime(0.),
	fFreq(kDefaultFreq)
{
	/*!
	 * \throws std::invalid_argument If \e tsbank is not found
	 * \todo Stop hard coding coincidence window
	 */
	Init(tsbank, header, data, size);
}

dragon::MidasEvent::MidasEvent(const char* tsbank, char* buf, int size):
	fCoincWindow(10.),
 	fClock (std::numeric_limits<uint64_t>::max()),
	fTriggerTime(0.),
	fFreq(kDefaultFreq)
{
	/*!
	 * \throws std::invalid_argument If \e tsbank is not found
	 */
	Init(tsbank, buf, buf+sizeof(EventHeader_t), size);
}

void dragon::MidasEvent::CopyDerived(const dragon::MidasEvent& other)
{
	/*!
	 * Copies all data fields and calls TMidasEvent::Copy().
	 */
	fFreq        = other.fFreq;
	fClock       = other.fClock;
	fCrossClock  = other.fCrossClock;
	fTriggerTime = other.fTriggerTime;
	fCoincWindow = other.fCoincWindow;
	TMidasEvent::Copy(other);
}

void dragon::MidasEvent::PrintSingle(FILE* where) const
{
	fprintf (where, "Singles event: id, ser, trig: %i, %u, %.16f\n",
					 GetEventId(), GetSerialNumber(), fTriggerTime);
}

void dragon::MidasEvent::PrintCoinc(const MidasEvent& other, FILE* where) const
{
	fprintf (where, "Coincidence event: id[0], ser[0], t[0], id[1], ser[1], t[1] | t[0]-t[1]: "
					 "%i, %u, %.16f, %i, %u, %.16f, %.16f\n",
					 GetEventId(), GetSerialNumber(), fTriggerTime,
					 other.GetEventId(), other.GetSerialNumber(), other.fTriggerTime, TimeDiff(other));
}


void dragon::MidasEvent::Init(const char* tsbank, const void* header, const void* addr, int size)
{
	memcpy(GetEventHeader(), header, sizeof(EventHeader_t));
	memcpy(GetData(), addr, GetDataSize());
	SetBankList();

	std::vector<double> vfreq = GetBank<double> ("FREQ");
	if (vfreq.size() == 0) {
		err::Error("dragon::MidasEvent::Init") << "couldn't find the  bank \""	<<
			 "FREQ" << "\"" << ERR_FILE_LINE;
		throw(std::invalid_argument("Missing \"FREQ\" bank."));
		return;
	}

	std::vector<uint32_t> vtsc = GetBank<uint32_t> (tsbank);
	if(vtsc.size() == 0) {
		err::Error("dragon::MidasEvent::Init") << "couldn't find the  bank \""	<<
			 tsbank << "\"" << ERR_FILE_LINE;
		throw(std::invalid_argument(tsbank));
		return;
	}

	try {
		// Iterate bank vector
		std::vector<uint32_t>::iterator itsc = vtsc.begin();

		// Read: firmware revision, write timestamp, routing, sync number
		uint32_t version = *itsc++, bkts = *itsc++, route = *itsc++, syncno = *itsc++;

		// Check version
		if (version == 0x1120809 || version == 0x1120810 || version == 0x1120910);
		else {
			err::Warning("dragon::MidasEvent::Init") <<
				 "Unknown TSC version 0x" << std::hex << version << std::dec << " (id, serial #: " << GetEventId() <<
				 ", " << GetSerialNumber() << ")" << ERR_FILE_LINE;
		}

		// Get TSC4 info
		uint32_t ctrl = *itsc++, nch = ctrl & READ15;
		bool overflow = (ctrl>>15) & READ1;
		if (overflow) {
			err::Warning("dragon::MidasEvent::Init") <<
				 "IO32 TSC in overflow condition. Event Serial #, Id: " << GetSerialNumber() << ", " << GetEventId() << "\n";
		}

		for(uint32_t i=0; i< nch; ++i) {
			uint64_t lower = *itsc++, upper = *itsc++, ch = (lower>>30) & READ2;

			switch (ch) {
			case 0: // Cross timestamp
				fCrossClock.push_back( read_timestamp(lower, upper) );
				break;

			case 1: // Trigger timestamp
				if (fClock != std::numeric_limits<uint64_t>::max()) {
					err::Warning("dragon::MidasEvent::Init") <<
						 "duplicate trigger TS in fifo (okay if equivalent). Serial #: " << GetSerialNumber() <<
						 ", tsc[1][0] = " << fClock << ", tsc[1][1] = " << read_timestamp(lower, upper) << "\n";
					if (fClock != read_timestamp(lower, upper)) {
						throw (std::invalid_argument("Non-equivalent duplicate trigger ts"));
					}
				}

				fClock = read_timestamp(lower, upper);
				fFreq  = vfreq[0];
				if (fFreq > 0.) fTriggerTime = fClock / fFreq;
				else {
					err::Error("dragon::MIdasEvent::Init") << "Found a frequency <= 0: " << fFreq << ERR_FILE_LINE;
					throw (std::invalid_argument("Read invalid frequency."));
				}
				break;

			case 2:
				// Sync value - skip
				break;

			default:
				break;
			}
		}
	} catch (std::range_error err) {
		err::Error("dragon::MidasEvent::Init")
			 << "Range error in " << tsbank << "bank iterator: " << err.what() << ERR_FILE_LINE;
		throw err; // Don't recatch - treat as fatal (for now...)
	}
}


dragon::CoincMidasEvent::CoincMidasEvent(const MidasEvent& event1, const MidasEvent& event2)
{
	if (event1.GetEventId() == DRAGON_HEAD_EVENT && event2.GetEventId() == DRAGON_TAIL_EVENT) {
		fGamma    = &event1;
		fHeavyIon = &event2;
	}
	else if (event1.GetEventId() == DRAGON_TAIL_EVENT && event2.GetEventId() == DRAGON_HEAD_EVENT) {
		fGamma    = &event2;
		fHeavyIon = &event1;
	}
	else {
		err::Warning("CoincMidasEvent::CoincMidasEvent")
			<< ERR_FILE_LINE << "Don't know how to handle the passed events: "
			<< "Id1 = " << event1.GetEventId() << ", Id2 = " <<event2.GetEventId()
			<< ". Skipping...\n";
		return;
	}
}
