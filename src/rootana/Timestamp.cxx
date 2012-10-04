/// \file rootana/Timestamp.cxx
/// \author G. Christian
/// \brief Implements rootana/Timestamp.hxx 
#include <algorithm>
#include "utils/definitions.h"
#include "utils/Error.hxx"
#include "Events.hxx"
#include "Timestamp.hxx"


void rootana::TSQueue::HandleSingle(const midas::Event& event) const
{
	/*!
	 * Determine event ID, send on to EventHandler::Prcess(const midas::Event&)
	 */
	switch (event.GetEventId()) {
	case DRAGON_HEAD_EVENT: break;
	case DRAGON_TAIL_EVENT:	break;
	default:
		dragon::err::Error("rootana::TSQueue::HandleSingle")
			<< "Unknown event id: " << event.GetEventId() << ", skipping...\n";
		return;
	}
	rootana::EventHandler::Instance()->Process(event);
}

void rootana::TSQueue::HandleCoinc(const midas::Event& event1, const midas::Event& event2) const
{
	/*!
	 * Construct coinc event, send on to EventHandler::Prcess(const midas::CoincEvent&)
	 */
	midas::CoincEvent coincEvent(event1, event2);

	if (coincEvent.fHeavyIon == 0 ||	coincEvent.fGamma == 0) {
		dragon::err::Error("rootana::TSQueue::HandleCoinc")
			<< "Invalid coincidence event, skipping...\n";
		return;
	}

	rootana::EventHandler::Instance()->Process(coincEvent);
}
