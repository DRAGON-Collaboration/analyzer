/// \file Event.cxx
/// \author G. Christian
/// \brief Implements Event.hxx
#include <stdint.h>
#include <cmath>
#include <limits>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include "utils/definitions.h"
#include "utils/Bits.hxx"
#include "Event.hxx"


namespace {

const double kDefaultFreq = 20.;

inline uint64_t read_timestamp (uint64_t lower, uint64_t upper)
{
	return (lower & READ30) | (upper << 30);
} }

// ========= Class midas::Event ========= //

midas::Event::Event(const char* tsbank, const void* header, const void* data, int size):
	fCoincWindow(10.),
	fClock (std::numeric_limits<uint64_t>::max()),
	fTriggerTime(0.),
	fFreq(kDefaultFreq)
{
	/*!
	 * \throws std::invalid_argument If \e tsbank is not found
	 * \todo Stop hard coding coincidence window
	 * \attention Passing NULL (0) for \e tsbank ignores timestamp features
	 */
	Init(tsbank, header, data, size);
}

midas::Event::Event(const char* tsbank, char* buf, int size):
	fCoincWindow(10.),
 	fClock (std::numeric_limits<uint64_t>::max()),
	fTriggerTime(0.),
	fFreq(kDefaultFreq)
{
	/*!
	 * \throws std::invalid_argument If \e tsbank is not found
	 * \attention Passing NULL (0) for \e tsbank ignores timestamp features
	 */
	Init(tsbank, buf, buf+sizeof(EventHeader_t), size);
}

void midas::Event::CopyDerived(const midas::Event& other)
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

void midas::Event::PrintSingle(FILE* where) const
{
	fprintf (where, "Singles event: id, ser, trig, clock: %i, %u, %.16f, %lu\n",
					 GetEventId(), GetSerialNumber(), fTriggerTime, fClock);
}

void midas::Event::PrintCoinc(const Event& other, FILE* where) const
{
	fprintf (where, "Coincidence event: id[0], ser[0], t[0], clk[0], id[1], ser[1], t[1], clk[1] | t[0]-t[1]: "
					 "%i, %u, %.16f, %lu, %i, %u, %.16f, %lu, %.16f\n",
					 GetEventId(), GetSerialNumber(), fTriggerTime, fClock,
					 other.GetEventId(), other.GetSerialNumber(), other.fTriggerTime, other.fClock,
					 TimeDiff(other));
}


void midas::Event::Init(const char* tsbank, const void* header, const void* addr, int size)
{
	memcpy(GetEventHeader(), header, sizeof(EventHeader_t));
	memcpy(GetData(), addr, GetDataSize());
	SetBankList();

	if (tsbank != 0) {
		int tsclength;
		uint32_t* ptsc = GetBankPointer<uint32_t> (tsbank, &tsclength, true, true);
		if (!ptsc) throw(std::invalid_argument(tsbank));

		// Read: firmware revision, write timestamp, routing, sync number
		uint32_t version = *ptsc++;
		uint32_t bkts    = *ptsc++;
		uint32_t route   = *ptsc++;
		uint32_t syncno  = *ptsc++;

		// Suppress compiler warning about unused values
		if (0 && version && bkts && route && syncno) { }

		// Check version
		if (version == 0x1120809 || version == 0x1120810 || version == 0x1120910 || version == 0x01121212 || version == 0x01120925);
		else {
			utils::err::Warning("midas::Event::Init") <<
				"Unknown TSC version 0x" << std::hex << version << std::dec << " (id, serial #: " << GetEventId() <<
				", " << GetSerialNumber() << ")" << DRAGON_ERR_FILE_LINE;
		}

		// Get TSC4 info
		uint32_t ctrl = *ptsc++, nch = ctrl & READ15;
		bool overflow = (ctrl>>15) & READ1;
		if (overflow) {
			utils::err::Warning("midas::Event::Init") <<
				"IO32 TSC in overflow condition. Event Serial #, Id: " << GetSerialNumber() << ", " << GetEventId() << "\n";
		}

		for(uint32_t i=0; i< nch; ++i) {
			uint64_t lower = *ptsc++, upper = *ptsc++, ch = (lower>>30) & READ2;

			switch (ch) {
			case 1: // Cross timestamp
				fCrossClock.push_back( read_timestamp(lower, upper) );
				break;

			case 0: // Trigger timestamp
				if (fClock != std::numeric_limits<uint64_t>::max()) {
					utils::err::Warning("midas::Event::Init") <<
						"duplicate trigger TS in fifo (okay if equivalent). Serial #: " << GetSerialNumber() <<
						", tsc[1][0] = " << fClock << ", tsc[1][1] = " << read_timestamp(lower, upper) << "\n";
					if (fClock != read_timestamp(lower, upper)) {
						throw (std::invalid_argument("Non-equivalent duplicate trigger ts"));
					}
				}

				fClock = read_timestamp(lower, upper);
				if (fFreq > 0.) fTriggerTime = fClock / fFreq;
				else {
					utils::err::Error("midas::Event::Init") << "Found a frequency <= 0: " << fFreq << DRAGON_ERR_FILE_LINE;
					throw (std::invalid_argument("Read invalid frequency."));
				}
				break;

			default:
				break;
			}
		}
	} // if tsbank != 0
}

midas::CoincEvent::CoincEvent(const Event& event1, const Event& event2):
	fGamma(0), fHeavyIon(0)
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
		utils::err::Warning("CoincMidasEvent::CoincMidasEvent")
			<< DRAGON_ERR_FILE_LINE << "Don't know how to handle the passed events: "
			<< "Id1 = " << event1.GetEventId() << ", Id2 = " <<event2.GetEventId()
			<< ". Setting fGamma and fHeavyIon to NULL...\n";
	}
}
