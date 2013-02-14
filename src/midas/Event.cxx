/// \file Event.cxx
/// \author G. Christian
/// \brief Implements Event.hxx
#include <stdint.h>
#include <cmath>
#include <limits>
#include <cassert>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include "utils/definitions.h"
#include "utils/Functions.hxx"
#include "utils/Bits.hxx"
#include "Event.hxx"


namespace {

const double COINC_WINDOW = 10.;

}


// ========= Class midas::Event ========= //

midas::Event::Event(const char* tsbank, const void* header, const void* data, int size):
	fCoincWindow(COINC_WINDOW),
	fClock (std::numeric_limits<uint32_t>::max()),
	fTriggerTime(0.)
{
	/*!
	 * \throws std::invalid_argument If \e tsbank is not found
	 * \todo Stop hard coding coincidence window
	 * \attention Passing NULL (0) for \e tsbank ignores timestamp features
	 */
	Init(tsbank, header, data, size);
}

midas::Event::Event(const char* tsbank, char* buf, int size):
	fCoincWindow(COINC_WINDOW),
 	fClock (std::numeric_limits<uint32_t>::max()),
	fTriggerTime(0.)
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
	fClock       = other.fClock;
	fCrossClock  = other.fCrossClock;
	fTriggerTime = other.fTriggerTime;
	fCoincWindow = other.fCoincWindow;
	TMidasEvent::Copy(other);
}

void midas::Event::PrintSingle(FILE* where) const
{
	std::stringstream sstr;
	sstr << "Singles event: id, ser, trig, clock: "
			 << GetEventId() << ", " <<  GetSerialNumber() << ", " <<  fTriggerTime << ", " <<  fClock;
	fprintf (where, "%s\n", sstr.str().c_str());
}

void midas::Event::PrintCoinc(const Event& other, FILE* where) const
{
	std::stringstream sstr;
	sstr << "Coincidence event: id[0], ser[0], t[0], clk[0], id[1], ser[1], t[1], clk[1] | t[0]-t[1]: "
			 << GetEventId() << ", " <<  GetSerialNumber() << ", " <<  fTriggerTime << ", " <<  fClock << ", "
			 << other.GetEventId() << ", " <<  other.GetSerialNumber() << ", " <<  other.fTriggerTime << ", "
			 <<  other.fClock << ", " << TimeDiff(other);
	fprintf (where, "%s\n", sstr.str().c_str());
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

		// Suppress compiler warning about unused values
		if (0 && version && bkts && route) { }

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
			uint32_t lower = *ptsc++, upper = *ptsc++, ch = (lower>>30) & READ2;

			switch (ch) {

			case 0: // Trigger timestamp
				fClock = std::min(fClock, lower & 0x3fffffff);
				fTriggerTime = fClock / DRAGON_TSC_FREQ;
				break;

			case 1: // Cross timestamp
				fCrossClock.push_back(lower & 0x3fffffff);
				break;

			default:
				break;
			}
		}
	} // if tsbank != 0
}

double midas::Event::TimeDiff(const Event& other) const
{
	/*!
	 * Takes into account rollover by checking that the abslute difference is < 0x1fffffff
	 * \note 'this' - 'other'
	 */
	int32_t clockDiff = utils::time_diff30(fClock, other.fClock);
	return clockDiff / DRAGON_TSC_FREQ;
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
			<< "Id1 = " << event1.GetEventId() << ", Id2 = " << event2.GetEventId()
			<< ", trig1 = " << event1.ClockTime() << ", trig2 = " << event2.ClockTime()
			<< ", time diff = " << event1.TimeDiff(event2)
			<< ". Setting fGamma and fHeavyIon to NULL...\n";
	}
	if(fGamma && fHeavyIon) {
		xtrig = fHeavyIon->TimeDiff(*fGamma);
	}
}
