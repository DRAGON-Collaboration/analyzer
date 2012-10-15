/*!
 * \file TStamp.hxx
 * \author G. Christian
 * \brief Defines classes for managing singles/coincidence identification based on trigger timestamps.
 */
#ifndef DRAGON_TSTAMP_HXX
#define DRAGON_TSTAMP_HXX
#include <stdint.h>
#include <set>
#include "midas/Event.hxx"


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
 * queue, Size() to get the queue length, and Flush() to explicitly empty  the queue. To
 * set how coincidence and singles events should be handled, the class provides the private
 * virtual functions HandleCoinc() and HandleSingle(). In the base class, these simply
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
	 *
	 * Others containers tested included a sorted std::deque or std::vector; boost::container::flat_multiset
	 * (essentially a sorted std::vector); and boost::unordered_multiset. The sorted std::deque performed
	 * similarly to std::multiset for objects with cheap copy operations (e.g. a simple 'int' or 'double'),
	 * but for objects with non-trivial copy constructors the extra copy operations required to maintain
	 * sorting inhibit performance significantly (note that std::vector had bad performance in all cases
	 * due to the expense of removing items at the beginning of the vector).
	 *
	 * The boost hash implementation showed slightly better performance for coincidence matching than
	 * the std::multiset; however, the the increase was not significant, and it is not likely that it's
	 * worth the extra code required to keep track of the earliest entry and the the reliance
	 * on "non-standard" third-party code for a hash implementation.
	 */
	typedef std::multiset<midas::Event> MultiSet_t;

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

	/// Destructor, empty
	/*!
	 * \warning It may be tempting to put a call to Flush() into the destructor; however, this
	 * is a bad idea since Flush() will make calls to the virtual function HandleSingle() and maybe
	 * HandleCoinc().
	 */
	virtual ~Queue() { }

	/// Insert an element into the queue
	void Push(const midas::Event&);

	/// Erase the earliest event in the queue.
	void Pop();
 
	/// Flush all events from the queue.
	void Flush(int max_time = -1);

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
	virtual void HandleCoinc(const midas::Event& e1, const midas::Event& e2) const;

	/// What to do in case of a singles event
	virtual void HandleSingle(const midas::Event& event) const;
	
	/// Prints a message telling that Flush() timeout has been reached
	virtual void FlushTimeoutMessage(int max_time) const;
};


/// Queue that is a member of another class which handles poped events
/*!
 * The intended use of this class is for when the queue exists as a data member of
 * some other class (the "owner"). The owning class will perform the work of handling
 * singles or coincidence events popped from the queue (see below).
 * \tparam T The type of class owning the queue; must have the following member functions:
 * - <tt> void Process(const midas::Event&); </tt>
 * - <tt> void Process(const midas::Event&, cont midas::Event&); </tt>
 * 
 * to handle singles and coincidence events, respectively.
 */
template <class T>
class OwnedQueue: public Queue {
private:
	/// Reference to the class "owning" the queue
	T& fOwner;
	
public:
	/// Calls base constructor with maxDelts argument; sets fOwner
	OwnedQueue(double maxDelta, T* owner):
		tstamp::Queue(maxDelta), fOwner(*owner) { }
	
	/// Empty
	~OwnedQueue() { }

private:
	/// Calls <tt> fOwner.Process(const midas::Event&); </tt>
	void HandleSingle(const midas::Event& event) const
		{ fOwner.Process(event); }
	
	/// Calls <tt> fOwner.Process(const midas::Event&, const midas::Event&); </tt>
	void HandleCoinc(const midas::Event& event1, const midas::Event& event2) const
		{ fOwner.Process(event1, event2); }
};

/// "Creation" function for OwnedQueue<T>
template <class T>
inline OwnedQueue<T>* NewOwnedQueue(double maxDelta, T* owner)
{
	/*!
	 * Allows one to create a \c new instance of OwnedQueue<T>
	 * without having to explicitly specify the template parameter.
	 */
	return new OwnedQueue<T> (maxDelta, owner);
}

} // namespace tstamp

#endif
