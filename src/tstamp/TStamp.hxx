/*!
 * \file TStamp.hxx
 * \author G. Christian
 * \brief Defines classes for managing singles/coincidence identification based on trigger timestamps.
 */
#ifndef DRAGON_TSTAMP_HXX
#define DRAGON_TSTAMP_HXX
#include <stdint.h>
#include <set>
#include "dragon/MidasEvent.hxx"


/// Encloses timestamp matching classes.
namespace tstamp {

/// Class to manage coincidence/singles ID.
/*!
 * The basic idea is to buffer events in a queue for long enough to ensure that any possible
 * coincidence events have been received from the data source. Then, check if the early event
 * has any coincidence matches, and, if so, to process the events together. Then the earliest
 * event is processed as a singles event and removed from the queue.
 * 
 * The interface consists of Push() and Pop() functions to insert and remove events from the
 * queue, Size() to get the queue length, and Cleanup() to explicitly empty  the queue. To
 * set how coincidence and singles events should be handled, the class provides the private
 * virtual functions HandleCoincidence() and HandleSingle(). In the base class, these simply
 * print some information about the arguments to stdout.
 */
class Queue {
public:
	 /// Internal container type used to store events.
	 /*!
		* Performance testing has indicated that a std::multiset is the optimal container
		* for our needs:
		*   - Frequent insertion into the container
		*   - Frequent removal of the oldest element in the container
		*   - Quick identification of (possible multiple) coincidences by searching on the trigger time
		* Others containers tested included a sorted std::deque or std::vector; boost::container::flat_multiset
		* (essentially a sorted std::vector); and boost::unordered_multiset. The sorted std::deque performed
		* similarly to std::multiset for objects with cheap copy operations (e.g. a simple 'int' or 'double'),
		* but for objects with non-trivial copy constructors the extra copy operations required to maintain
		* sorting inhibit performance significantly (note that std::vector had bad performance in al cases
		* due to the expense of removing items at the beginning of the vector).
		*
		* The boost hash implementation showed slightly better performance for coincidence matching than
		* the std::multiset; however, the the increase was not significant, and it is not likely that it's
		* worth the extra (potentially buggy) code required to keep track of the earliest entry and the
		* the reliance on "non-standard" third-party code for a hash implementation.
		*/
	 typedef std::multiset<dragon::MidasEvent> MultiSet_t;

	 /// Pair of MultiSet_t iterators, returned from std::multiset::equal_range()
	 typedef std::pair<MultiSet_t::iterator, MultiSet_t::iterator> EqualRange_t;

private:
	 /// Maximum allowable time interval between the first and last event stored in the queue.
	 const double fMaxDelta;

	 /// Internal container of events waiting to be matched
	 MultiSet_t fEvents;

public:
	 /// Sets the maximum container size (fMaxDelta)
	 /*!
		* \param [in] deltaMax Maximum difference between timestamps before emptying
		* \note \e deltaMax should be set large enough to cover any potential timstamp overlaps,
		* but without taking up too much memory
		*/
	 Queue(double deltaMax): fMaxDelta (deltaMax) { }

	 /// Destructor, Calls Cleanup()
	 virtual ~Queue() { Cleanup(); }

	 /// Insert an element into the queue
	 void Push(const dragon::MidasEvent&);

	 /// Erase the earliest event in the queue.
	 void Pop();
 
	 /// Empty all events from the queue.
	 void Cleanup() { while (!fEvents.empty()) Pop(); }

	 /// Returns total number of entries in the queue
	 size_t Size() const { return fEvents.size(); }

protected:
	 /// Check whether the maximum size has been reached
	 bool IsFull() const { return MaxTimeDiff() > fMaxDelta; }

	 /// Get trigger time difference between earliest and latest event
	 double MaxTimeDiff() const
			{
				if (fEvents.empty()) return 0.;
				return fEvents.rbegin()->TimeDiff(*fEvents.begin());
			}

private:
	 /// What to do in case of a coincidence event
	virtual void HandleCoinc(const dragon::MidasEvent& e1, const dragon::MidasEvent& e2) const;

	 /// What to do in case of a singles event
	virtual void HandleSingle(const dragon::MidasEvent& event) const;
};

} // namespace tstamp

#endif
