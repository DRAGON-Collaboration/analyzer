/*!
 * \file TStamp.cxx
 * \author G. Christian
 * \brief Implements tstamp::Queue class
 */
#include <ctime>
#include "TStamp.hxx"
#include "utils/ErrorDragon.hxx"


// ========= Class tstamp::Queue ========= //

void tstamp::Queue::HandleCoinc(const midas::Event& event1,
																const midas::Event& event2) const
{
	/*!
	 * In the base class, simply prints information abouut both events together.
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

void tstamp::Queue::Push(const midas::Event& event)
{
	/*!
	 * First the function inserts \e event into the internal container. Then,
	 * it calls Pop() until the container size (time difference) is no longer
	 * larger than the maximum.
	 * \param [in] event Event to insert into the queue.
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
		utils::err::Error("tstamp::Queue::Push")
			<< "Caught an exception from std::multiset::insert: " << e.what()
			<< " (note: size = " << Size() << ", max size = " << fEvents.max_size()
			<< "). Clearing the Queue and trying again... WARNING: that this could cause "
			<< "coincidences to be missed!" << DRAGON_ERR_FILE_LINE;

		Flush(); // remove everything from the queue

		try { // try again to insert

			fEvents.insert(event);
		}
		catch (std::exception& e) { // give up
			utils::err::Error("tstamp::Queue::Push")
				<< "Caught a second exception from std::multiset::insert: " << e.what()
				<< ". Not sure what to do: rethrowing (likely fatal!)" << DRAGON_ERR_FILE_LINE;
			throw (e);
		}
	}

	if (IsFull()) Pop(); // erase event from the front of the queue
}


void tstamp::Queue::Pop()
{
	/*!
	 * Looks at the earliest event still in the queue and searches for
	 * all other events in the queue with trigger times within the trigger window.
	 * Then the function loops over each of the matches and calls HandleCoinc() on
	 * the two events. Finally, the function calls HandleSingle() on the earliest
	 * event and then deletes it from the queue.
	 */

	if (fEvents.empty()) return;
	
	EqualRange_t matches = fEvents.equal_range(*fEvents.begin());
	for (MultiSet_t::iterator itMatch = matches.first; itMatch != matches.second; ++itMatch) {
		if (itMatch != fEvents.begin()) HandleCoinc(*fEvents.begin(), *itMatch);
	}

	HandleSingle(*fEvents.begin());
	fEvents.erase(fEvents.begin());
}

void tstamp::Queue::Flush(int max_time)
{
	/*!
	 * \param max_time Maximum of amount of time (in seconds) to spend clearing the
	 *  queue before returning. Any unhandled events at the end of the time limit will
	 * simply be deleted. Default value (-1) blocks indefinitely until all events are
	 * emptied from the queue.
	 */
	time_t t_begin = time(0);
	while (!fEvents.empty()) {
		if (max_time > 0 && difftime(time(0), t_begin) < max_time) {
			Pop();
		}
		else {
			FlushTimeoutMessage(max_time);
			fEvents.clear();
		}
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
	utils::err::Info("tstamp::Queue::Flush()")
		<< "Maximum timeout of " << max_time << " seconds reached. Clearing event queue (skipping "
		<< fEvents.size() << " events...).";
}
