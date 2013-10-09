/*!
 * \file TStamp.cxx
 * \author G. Christian
 * \brief Implements tstamp::Queue class
 */
#include <ctime>
#include <cassert>
#include <algorithm>
#include "TStamp.hxx"
#include "utils/ErrorDragon.hxx"


// ========= Class tstamp::Queue ========= //

void tstamp::Queue::HandleCoinc(const midas::Event& event1,
																const midas::Event& event2) const
{
	/*!
	 * In the base class, simply prints information about both events together.
	 * \param event1 Reference to the first coincident event.
	 * \param event2 Reference to the second coincident event.
	 */
	event1.PrintCoinc(event2);
}

void tstamp::Queue::HandleSingle(const midas::Event& e) const
{ 
	/*!
	 * In the base class, simply prints information on the event.
	 * \param event Reference to the event being handled.
	 */
	e.PrintSingle();
}

void tstamp::Queue::HandleDiagnostics(tstamp::Diagnostics* d) const
{ 
	/*!
	 * In the base class, prints diagnostic fields.
	 */
	dragon::utils::Info("tstamp::Queue", __FILE__, __LINE__)
		<< "Diagnostics event: size = " << d->size << ", n_coinc = "
		<< d->n_coinc << ", time_diff = " << d->time_diff << ", n_singles[]: ";
	for (int i=0; i< tstamp::Diagnostics::MAX_TYPES; ++i) {
		std::cout << "[" << i << "]: " << d->n_singles[i] << ", ";
	}
	std::cout << std::endl;
}

void tstamp::Queue::Push(const midas::Event& event, tstamp::Diagnostics* diagnostics)
{
	/*!
	 * First the function inserts \e event into the internal container. Then,
	 * it calls Pop() until the container size (time difference) is no longer
	 * larger than the maximum.
	 * \param [in] event Event to insert into the queue.
	 * \param [in] diagnostics Optional pointer to a Diagnostics class instance,
	 *  to be filled with information from the push
	 * \note The function does attempt to handle exceptions thrown by the
	 * std::multiset::insert() function. Most likely these would result from
	 * making the set size too large, so we empty the queue and try again
	 * (w/ error mesage). If it still fails, give up and rethrow the exception
	 * (causing program termination).  If other exceptions start to show up, then
	 * code shold be added to handle them gracefully.
	 */

	try { // insert event into the queue
		fEvents.insert(event);
	}
	catch (std::exception& e) { // try to handle exception gracefully
		dragon::utils::Error("tstamp::Queue::Push", __FILE__, __LINE__)
			<< "Caught an exception from std::multiset::insert: " << e.what()
			<< " (note: size = " << Size() << ", max size = " << fEvents.max_size()
			<< "). Clearing the Queue and trying again... WARNING: that this could cause "
			<< "coincidences to be missed!";

		Flush(-1, diagnostics); // remove everything from the queue

		try { // try again to insert

			fEvents.insert(event);
		}
		catch (std::exception& e) { // give up
			dragon::utils::Error("tstamp::Queue::Push", __FILE__, __LINE__)
				<< "Caught a second exception from std::multiset::insert: " << e.what()
				<< ". Not sure what to do: rethrowing (likely fatal!)";
			throw (e);
		}
	}

  // Erase event from the front of the queue, but first collect some
	// diagnostic info
	
	assert(fEvents.size() > 0);
	bool haveCoinc = false;
	int32_t singlesId = -1;
	double tdiff = event.TimeDiff(*fEvents.begin());
	if (IsFull()) Pop(singlesId, haveCoinc);

	/// Update diagnostic info in diagnostics != NULL
	if(diagnostics) {
		FillDiagnostics(diagnostics, tdiff, haveCoinc, singlesId, event.GetTimeStamp());
		HandleDiagnostics(diagnostics);
	}
}


void tstamp::Queue::Pop(int32_t& singles_id, bool& found_coinc)
{
	/*!
	 * Looks at the earliest event still in the queue and searches for
	 * all other events in the queue with trigger times within the trigger window.
	 * Then the function loops over each of the matches and calls HandleCoinc() on
	 * the two events. Finally, the function calls HandleSingle() on the earliest
	 * event and then deletes it from the queue.
	 * 
	 * \param [out] singles_id MIDAS event ID of the removed (handled) singles event.
	 * A return value of -1 means the queue was empty (no event handled).
	 * \param [out] found_coinc Set to true if a coincidence match was found, false
	 *  otherwise.
	 */
	singles_id = -1;
	found_coinc = false;
	if (fEvents.empty()) return;

	EqualRange_t matches = fEvents.equal_range(*fEvents.begin());
	MultiSet_t::iterator itMatch;
	for (itMatch = matches.first; itMatch != matches.second; ++itMatch) {
		if (itMatch != fEvents.begin()) {
			found_coinc = true;
			HandleCoinc(*fEvents.begin(), *itMatch);
		}
	}

	singles_id = fEvents.begin()->GetEventId();
	HandleSingle(*fEvents.begin());
	fEvents.erase(fEvents.begin());
}

void tstamp::Queue::Flush(int max_time, tstamp::Diagnostics* diagnostics)
{
	/*!
	 * \param max_time Maximum of amount of time (in seconds) to spend clearing the
	 *  queue before returning. Any unhandled events at the end of the time limit will
	 *  simply be deleted. Default value (-1) blocks indefinitely until all events are
	 *  emptied from the queue.
	 * \param [in] diagnostics Optional pointer to a Diagnostics class instance,
	 *  to be filled with information from the flushed event
	 */
	time_t t_begin = time(0);
	while (!fEvents.empty()) {
		if ( max_time < 0 || (difftime(time(0), t_begin) < max_time) ) {
			DoFlushEvent(diagnostics);
		}
		else {
			FlushTimeoutMessage(max_time);
			fEvents.clear();
		}
	}
}

size_t tstamp::Queue::FlushIterative(tstamp::Diagnostics* diagnostics)
{
	/*!
	 * "Iterative" or step-by-step flushing methos for users who perfer to flush
	 *  the queue using their own external loop.
	 * \param [in] diagnostics Optional pointer to a Diagnostics class instance,
	 *  to be filled with information from the flushed event
	 * \returns The size of the internal queue \e before performing a flush.
	 */
	size_t qsize = fEvents.size();
	if(qsize > 0) DoFlushEvent(diagnostics);
	return qsize;
}

void tstamp::Queue::DoFlushEvent(tstamp::Diagnostics* diagnostics)
{
	/// Call Pop() on the front event
	int32_t singlesId = -1;
	bool haveCoinc = false;
	uint32_t tfirst = fEvents.rbegin()->GetTimeStamp();
	Pop(singlesId, haveCoinc);

	/// Update diagnostic info if diagnostics != NULL
	if(diagnostics) {
		FillDiagnostics(diagnostics, 0., haveCoinc, singlesId, tfirst);
		HandleDiagnostics(diagnostics);
	}
}

void tstamp::Queue::FlushTimeoutMessage(int max_time) const
{
	/*!
	 * Derived classes may wish to override this method to make use of a local
	 * message handling system (e.g. cm_msg() in MIDAS).
	 *
	 * \param max_time Length of max Flush() timeout in seconds
	 */
	dragon::utils::Warning("tstamp::Queue::Flush()", __FILE__, __LINE__)
		<< "Maximum timeout of " << max_time << " seconds reached. Clearing event queue (skipping "
		<< fEvents.size() << " events...).";
}

void tstamp::Queue::FillDiagnostics(tstamp::Diagnostics* d, double tdiff, bool have_coinc,
																		int32_t singles_id, uint32_t evt_time)
{
	if(!d) return;
	d->size = Size();
	d->time_diff = tdiff;
	if(have_coinc) d->n_coinc += 1;
	if(singles_id >= 0 && singles_id < Diagnostics::MAX_TYPES) {
		d->n_singles[singles_id] += 1;
	}
	else if(singles_id > 0) { // Id >= MAX_TYPES
		dragon::utils::Warning("Queue::FillDiagnostics", __FILE__, __LINE__)
			<< "Singles id >= Diagnostics::MAX_TYPES, id = " << singles_id
			<< ", types = " << Diagnostics::MAX_TYPES;
	}

	// rates
	if(d->fTime0 == 0)	d->fTime0 = evt_time;
	uint32_t time = evt_time - d->fTime0;
	if(time > 0) {
		d->coinc_rate = d->n_coinc / (double)time;
		for(int i=0; i< Diagnostics::MAX_TYPES; ++i)
			d->singles_rate[i] = d->n_singles[i] / time;
	}
	else { 
		d->coinc_rate = 0.;
		std::fill(d->singles_rate, d->singles_rate + Diagnostics::MAX_TYPES, 0.);
	}
}


// ====== Class tstamp::Diagnostics ====== //

tstamp::Diagnostics::Diagnostics()
{
	/*! Calls reset() */
	reset();
}

tstamp::Diagnostics::~Diagnostics()
{
	/*! Empty */
	;
}

void tstamp::Diagnostics::reset()
{
	/*! All parameters -> zero. */
	fTime0 = 0;
	size = 0;
	n_coinc = 0;
	time_diff = 0.;
	coinc_rate = 0.;
	std::fill(n_singles, n_singles + MAX_TYPES, 0);
	std::fill(singles_rate, singles_rate + MAX_TYPES, 0.);
}
