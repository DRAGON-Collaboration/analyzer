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

class Diagnostics;

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
	double fMaxDelta;
	
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
	virtual void Push(const midas::Event&, tstamp::Diagnostics* diagnostics = 0);

	/// Erase the earliest event in the queue, first searching for coincidences.
	virtual void Pop(int32_t& singles_id, bool& found_coinc);
 
	/// Flush all events from the queue.
	virtual void Flush(int max_time = -1, tstamp::Diagnostics* diagnostics = 0);

	/// Flush a single event from the queue.
	virtual size_t FlushIterative(tstamp::Diagnostics* diagnostics = 0);

	/// Returns total number of entries in the queue
	size_t Size() const { return fEvents.size(); }

	/// Set the maximum queue time to a new value
	void SetMaxDelta(double delta) { fMaxDelta = delta; }

	/// Check the maximum queue time
	double GetMaxDelta() const { return fMaxDelta; }

protected:
	/// Check whether the maximum size has been reached
	bool IsFull() const { return MaxTimeDiff() > fMaxDelta; }

	/// Get trigger time difference between earliest and latest event
	double MaxTimeDiff() const
		{
			if (fEvents.empty()) return 0.;
			return fEvents.rbegin()->TimeDiff(*fEvents.begin());
		}

	/// Fill diagnostic information after a push.
	void FillDiagnostics(tstamp::Diagnostics* d, double tdiff, bool have_coinc, int32_t singles_id, uint32_t evt_time);

private:
	/// What to do in case of a coincidence event
	virtual void HandleCoinc(const midas::Event& e1, const midas::Event& e2) const;

	/// What to do in case of a singles event
	virtual void HandleSingle(const midas::Event& event) const;

	/// What to do with a diagnostics event
	virtual void HandleDiagnostics(tstamp::Diagnostics* diagnostics) const;
	
	/// Prints a message telling that Flush() timeout has been reached
	virtual void FlushTimeoutMessage(int max_time) const;

	/// Internal helper function for flushing routines
	void DoFlushEvent(tstamp::Diagnostics*);
};


/// Queue that is a member of another class which handles popped events
/*!
 * The intended use of this class is for when the queue exists as a data member of
 * some other class (the "owner"). The owning class will perform the work of handling
 * singles or coincidence events popped from the queue (see below).
 * \tparam T The type of class owning the queue; must have the following member functions:
 * 
 * - <tt> void Process(const midas::Event&); </tt>
 * - <tt> void Process(const midas::Event&, cont midas::Event&); </tt>
 * - <tt> void Process(tstamp::Diagnostics*); </tt>
 * 
 * to handle singles and coincidence events, respectively.
 */
template <class T>
class OwnedQueue: public Queue {
private:
	/// Reference to the class "owning" the queue
	T& fOwner;
	
public:
	/// Calls base constructor with maxDelta argument; sets fOwner
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

	/// Calls <tt> fOwner.Process(tstamp::Diagnostics*); </tt>
	void HandleDiagnostics(tstamp::Diagnostics* diagnostics) const
		{ fOwner.Process(diagnostics); }
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


/// Class to store diagnostic information about coincidence matching
class Diagnostics {
/*!
 * Information in this class is updated every time an event is inserted
 * into the queue. Typically, one passes an instance of this class to a
 * queue which will handle the updating of diagnostic inforamtion
 * automatically whenever a new event is pushed into the queue.
 *
 * \note When tstamp::Queue handles the updating of this class's fields,
 * the updates happen at the \e end of a Push(). This means that anything
 * resulting from a push, be it a new coincidence match or singles event
 * processed will be reflected in the state of the Diagnostics instance.
 * Information is also updated when flushing from the queue, but here
 * tims_diff is set to zero since no new events are incoming.
 */
public:
	/// Initial event time (begin of run)
	uint32_t fTime0; //!

	/// Maximum number of event types (ids) allowable
	static const int32_t MAX_TYPES = 10;

	/// Size of the queue
	uint64_t size;

	/// Number of processed coincidence events
	uint64_t n_coinc;

	/// Rate of coincidence events
	double coinc_rate;

	/// Number of processed singles events
	/*! Array where the inde corresponds midas event id */
	uint64_t n_singles[MAX_TYPES];

	/// Rate of each singles event
	double singles_rate[MAX_TYPES];

	/// Tells the time difference between the most recently inserted
	/// event and the earliest event.
	/*! This gives an idea of how close we are getting to the maximum
	 *  time difference specified for the queue. */
	double time_diff;

public:
	/// Set all data to defaults
	Diagnostics();

	/// Nothing to do
	~Diagnostics();

	/// Reset data to default (BOR values)
	void reset();

	/// tstamp::Queue needs to fill diagnostic information
	/*! Yes the data are public so this isn't actually necessary, but
	 * I think it is good to design as if they were private */
	friend class tstamp::Queue;
};

} // namespace tstamp

#endif
