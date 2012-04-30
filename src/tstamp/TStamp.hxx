/// \file TStamp.hxx
/// \brief Defines classes for managing singles/coincidence identification based on timestamps
#ifndef DRAGON_TSTAMP_HXX
#define DRAGON_TSTAMP_HXX
#include <stdint.h>
#include <algorithm>
#include <set>
#include <TRandom3.h>
#include <TFile.h>
#include <TTree.h>
#include "dragon/Dragon.hxx"


// ========= Classes ========= //

/// Encloses timstamp matching classes
namespace tstamp {

/// \brief 'Event' class, consists of a MIDAS event and timestamp value
struct Event {
	 /// Type codes
	 enum { GAMMA, HION };

	 /// Timestamp value
	 uint64_t tstamp;

	 /// 'Type' code
	 int type;

	 /// Midas event
	 TMidasEvent fMidasEvent;

	 /// Set timestamp and type
	 Event(uint32_t tstamp_, int type_, TMidasEvent event_);

	 /// Default constructor
	 Event() {}

	 /// Copy constructor
	 Event(const Event& other) { Copy(other); }

	 /// Equivalency
	 Event& operator= (const Event& other) { Copy(other); return *this; }

	 /// For Copying
	 void Copy(const Event& other);

	 /// Destructor
	 ~Event();
};

/// \brief Timestamp ordering functor
struct Order {
	 /// \brief Order two tstamp::Event classes based on their \e tstamp value
	 bool operator() (const tstamp::Event& lhs, const tstamp::Event& rhs) {
		 return lhs.tstamp < rhs.tstamp;
	 }
};

/// \brief Timestamp comparison functor
struct Compare {
	 /// \brief Compare two tstamp::Events for 'equivalency'
	 /// \details This is to be used when searching for matches.
	 /// Two Events are considered a match if:
	 /// -# They have different \e type
	 /// -# The difference between their \e tstamp values is within a coincidence window
	 /// \note STL search algorithms define 'equivalency' as !comp(a,b) && !comp(b,a)
	 bool operator() (const tstamp::Event& lhs, const tstamp::Event& rhs);
};

/// \brief Class to manage coincidence/singles ID.
/// \details This class uses a basic queue structure to check timestamp matches
/// Upon insertion of a new element
class Queue {
public:
	 /// Container typedef
	 typedef std::set<tstamp::Event, tstamp::Order> Container;
	 /// Container iterator typedef
	 typedef Container::iterator Iterator;
private:
	 /// \brief Maximum size of the container
	 const int fMaxSize;
	 /// \brief Internal container for storing unmatched timestamps
	 Container fContainer;
public:
	 /// \brief Sets the maximum container size
	 /// \param [in] size maximum container size
	 /// \note \e size should be set large enough to cover any potential timstamp overlaps
	 Queue(int size);

	 /// \brief Calls Cleanup()
	 ~Queue() { Cleanup(); }

	 /// \brief Insert an element into the queue
	 /// \details First checks existing elements for a match. If one is found,
	 /// it writes the present event and its match as a coincidence. Otherwise, the
	 /// present event is inserted into the buffer so that it can be checked against
	 /// future events.
	 void Push(tstamp::Event&);

	 /// \brief Erase the earliest event in the queue.
	 /// \details Writes the beginning event as a singles event and removes it from the
	 /// queue.
	 /// \note This should only be called in cases where we can assume that the earliest event
	 /// is not a coincidence - that is, either the container has reached the max size (enough time
	 /// has passed to guarantee no coincidences) or when we are done adding events.
	 void Pop();

	 /// \brief Return an iterator to the beginning of the Queue
	 Iterator First() { return fContainer.begin(); }

	 /// \brief Return an iterator to the end of the Queue
	 Iterator Last() { return fContainer.end(); }

	 /// \brief Check for a timestamp match
	 /// \details Uses the tstamp::Compare functor to see if there's a timstamp match to \e event
	 /// in the range between \e first and <i>last<\i>.
	 /// \returns Iterator to match position if there is a match, \e last otherwise
	 Iterator CheckMatch(const tstamp::Event& event, Iterator first, Iterator last);

	 /// \brief Check for a timestamp match
	 /// \details Same as the other CheckMatch() function, except the range is the full range
	 /// of fContainer
	 Iterator CheckMatch(const tstamp::Event& event) { return CheckMatch(event, First(), Last()); }

	 /// \brief Check whether the maximum size has been reached
	 /// \details This function returns true if we have buffered enough events to effectively
	 /// guarantee that the earliest one is definitely a singles event
	 bool IsFull();

	 /// \brief Empty all events from the queue, writing each as a singles event.
	 void Cleanup();

private:
	 /// What to do in case of a coincidence event
	 void HandleCoinc(tstamp::Event&, Iterator&);

	 /// What to do in case of a singles event
	 void HandleSingle(Iterator&);
};

} // namespace tstamp

#endif
