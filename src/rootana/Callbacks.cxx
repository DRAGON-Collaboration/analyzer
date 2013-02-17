/*!
 * \file Callbacks.cxx
 * \author G. Christian
 * \brief Implements callbacks.hxx
 */
#include "Callbacks.hxx"


void rootana_handle_event(const void* pheader, const void* pdata, int size)
{
	/*!
	 * Figure out the TSC bank name from event id, then pass on the work to
	 * rootana::App::handle_event().
	 */
	const midas::Event::Header* head = reinterpret_cast<const midas::Event::Header*>(pheader);
	char tscBk[5] = {'T','S','C','H', '\0'};
	char* ptsc = &tscBk[0];
	if (0) { }
	else if (head->fEventId == DRAGON_TAIL_EVENT)
		strcpy(tscBk, "TSCT");
	else if (head->fEventId == DRAGON_HEAD_EVENT)
		strcpy(tscBk, "TSCH");
	else
		ptsc = 0;
	
	midas::Event e(pheader, pdata, head->fDataSize, ptsc, rootana::App::instance()->GetCoincWindow());
	rootana::App::instance()->handle_event(e);
}

