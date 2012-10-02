/// \file Timestamp.cxx
/// \author G. Christian
/// \brief Implements rootana/Timestamp.hxx 
#include "utils/definitions.h"
#include "utils/Error.hxx"
#include "DragonEvents.hxx"
#include "Timestamp.hxx"


void rootana::TSQueue::HandleSingle(const midas::Event& event) const
{
	/*!
	 * Determine event ID, then send onto the appropriate event processor.
	 */
	switch (event.GetEventId()) {

	case DRAGON_HEAD_EVENT:
		// do something //
		break;

	case DRAGON_TAIL_EVENT:
		// do something //
		break;

	default:
		dragon::err::Error("rootana::TSQueue::HandleSingle")
			<< "Unknown event id: " << event.GetEventId() << ", skipping...\n";
		break;
	}
}

void rootana::TSQueue::HandleCoinc(const midas::Event& event1, const midas::Event& event2) const
{
	/*!
	 * \todo Write...
	 */
#if 0 
	midas::CoincEvent coincEvent(event1, event2);

	if (coincEvent.fHeavyIon == 0 ||	coincEvent.fGamma == 0) {
		dragon::err::Error("rootana::TSQueue::HandleCoinc")
			<< "Invalid coincidence event, skipping...\n";
		return;
	}

	rb::Event::Instance<rootana::CoincEvent>()->Process(&coincEvent, 0);
#endif
}
