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

// Helper function to calculate full timestamp values
inline uint64_t read_timestamp (uint32_t tscl, uint32_t tsch)
{
	// tscl: lower bits 29..0 bits, tsch: upper bits 35..28
	uint64_t tscfull = tscl & 0x1fffffff; // bits 28..0

	// check for match in overlap bit 29
	int bit29h = (tsch>>1) & 0x1, bit29l = (tscl>>29) & 0x1;
	if (bit29h != bit29l) { // adjust upper bits to account for difference
		if (bit29l < bit29h) ++tsch;
		else --tsch;
	}
	// append bits > 29
	tscfull |= ((uint64_t)(tsch>>1) << (uint64_t)29);
	return tscfull;
}

}


// ========= Class midas::Event ========= //

midas::Event::Event(const void* header, const void* data, int size, const char* tsbank, double coinc_window):
	fCoincWindow(coinc_window),
	fClock (std::numeric_limits<uint64_t>::max()),
	fTriggerTime(0.)
{
	/*!
	 * \param header Pointer to event header (midas::Event::Header struct)
	 * \param data Pointer to the data portion of an event
	 * \param size Size in bytes of the data portion of the event
	 * \param tsbank Bank name of the TSC4 data; if NULL, tsc features are ignored
	 * \param coinc_window Desired window to be considered a coincidence match w/ another event.
	 */
	Init(tsbank, header, data, size);
}

midas::Event::Event(char* buf, int size, const char* tsbank, double coinc_window):
	fCoincWindow(coinc_window),
 	fClock (std::numeric_limits<uint64_t>::max()),
	fTriggerTime(0.)
{
	/*!
	 * \param buf Buffer containing the entirity of the event data (header + actual data)
	 * \param size Size of the data portion of the event (not including the header)
	 * \param tsbank Bank name of the TSC4 data; if NULL, tsc features are ignored
	 * \param coinc_window Desired window to be considered a coincidence match w/ another event.
	 */
	Init(tsbank, buf, buf+sizeof(EventHeader_t), size);
}

void midas::Event::CopyDerived(const midas::Event& other)
{
	/*!
	 * Copies all data fields and calls TMidasEvent::Copy().
	 */
	fClock       = other.fClock;
	fTriggerTime = other.fTriggerTime;
	fCoincWindow = other.fCoincWindow;
	other.CopyFifo(fFifo);
	TMidasEvent::Copy(other);
}

void midas::Event::CopyFifo(std::vector<uint64_t>* pfifo) const
{
	/*!
	 * \attention Input pointer must point to an array w/ length >= 4.
	 */
	for(uint32_t i=0; i< MAX_FIFO; ++i) {
		(pfifo+i)->clear();
		(pfifo+i)->resize(fFifo[i].size());
		std::copy(fFifo[i].begin(), fFifo[i].end(), pfifo[i].begin());
	}
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
		uint32_t versions[] =
			{ 0x01130215, 0x01121212, 0x01120925, 0x01120809, 0x01120810, 0x01120910 };

		bool okVersion = false;
		for(uint32_t v=0; v< sizeof(versions)/sizeof(uint32_t); ++v) {
			if(version == versions[v]) { okVersion = true; break; }
		}
		if(okVersion == false) {
			utils::err::Warning("midas::Event::Init") <<
				"Unknown TSC version 0x" << std::hex << version << std::dec << " (id, serial #: " << GetEventId() <<
				", " << GetSerialNumber() << ")" << DRAGON_ERR_FILE_LINE;
		}

		// Get TSC4 info
		uint32_t ctrl = *ptsc++, roll = *ptsc++;
		uint32_t nch  = ctrl & READ14; // number of channels
		bool overflow = (ctrl>>15) & READ1; // overflow flag
		uint32_t tsch = (ctrl>>16) & READ8; // upper tsc bits 35..28

		if (overflow) {
			utils::err::Warning("midas::Event::Init") <<
				"IO32 TSC in overflow condition. Event Serial #, Id: " << GetSerialNumber() << ", " << GetEventId() << "\n";
		}

		for(uint32_t i=0; i< nch; ++i) {
			uint32_t tscl = *ptsc++, ch = (tscl>>30) & READ2;
			assert(ch< MAX_FIFO);

			uint64_t tscfull = read_timestamp(tscl, tsch | (roll<<8));
			fFifo[ch].push_back(tscfull);

			if(ch == TRIGGER_CHANNEL && fClock == std::numeric_limits<uint64_t>::max()) {
				fClock = tscfull;
				fTriggerTime = fClock / DRAGON_TSC_FREQ;
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
	return fTriggerTime - other.fTriggerTime;
}
#include "midas.h"
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
			<< "Don't know how to handle the passed events: "
			<< "Id1 = " << event1.GetEventId() << ", Id2 = " << event2.GetEventId()
			<< ", Sys time 1 = " << event1.GetTimeStamp() << ", Sys time 2 = " << event2.GetTimeStamp()
			<< ", trig1 = " << event1.ClockTime() << ", trig2 = " << event2.ClockTime()
			<< ", time diff = " << event1.TimeDiff(event2)
			<< ". Setting fGamma and fHeavyIon to NULL...\n";
	}
	if(fGamma && fHeavyIon) {
		xtrig = fHeavyIon->TimeDiff(*fGamma);
	}
}
