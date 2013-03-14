/// \file rootbeer/Timestamp.cxx
/// \author G. Christian
/// \brief Implements rootbeer/Timestamp.hxx

// ROOTBEER includes //
#include "Event.hxx"

// DRAGON includes //
#include "utils/definitions.h"
#include "utils/ErrorDragon.hxx"
#include "DragonEvents.hxx"
#include "Timestamp.hxx"


void rootbeer::TSQueue::HandleSingle(const midas::Event& event) const
{
	/*!
	 * Determine event ID, then send onto the appropriate event processor.
	 */
	switch (event.GetEventId()) {

	case DRAGON_HEAD_EVENT:
		rb::Event::Instance<rootbeer::GammaEvent>()->Process(&event, 0);
		break;

	case DRAGON_TAIL_EVENT:
		rb::Event::Instance<rootbeer::HeavyIonEvent>()->Process(&event, 0);
		break;

	default:
		dragon::utils::err::Error("rootbeer::TSQueue::HandleSingle")
			<< "Unknown event id: " << event.GetEventId() << ", skipping...\n";
		break;
	}
}

void rootbeer::TSQueue::HandleCoinc(const midas::Event& event1, const midas::Event& event2) const
{
	/*!
	 * Construct a midas::CoincEvent, then sent onto the coincidence event processor
	 */ 
	midas::CoincEvent coincEvent(event1, event2);

	if (coincEvent.fHeavyIon == 0 ||	coincEvent.fGamma == 0) {
		dragon::utils::err::Error("rootbeer::TSQueue::HandleCoinc")
			<< "Invalid coincidence event, skipping...\n";
		return;
	}

	rb::Event::Instance<rootbeer::CoincEvent>()->Process(&coincEvent, 0);
}

void rootbeer::TSQueue::HandleDiagnostics(tstamp::Diagnostics*) const
{
	/*!
	 * Call Process() on the diagnostics events
	 */
	rb::Event::Instance<rootbeer::TStampDiagnostics>()->Process(0, 0);
}
