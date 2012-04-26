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
	 Event(uint32_t tstamp_, int type_);

	 /// \brief Check tstamp equivalency
	 /// \details Returns true if \e other
	 /// -# Is of opposite 'type' compared to \c this
	 /// -# Has a timstamp value within the coincidence window
	 bool operator== (const Event& other);
};

/// \brief Timestamp ordering functor
struct Order {
	 /// Compare two tstamp::Event structs by looking at their timestamp values
	 bool operator() (const tstamp::Event& lhs, const tstamp::Event& rhs) {
		 return lhs.tstamp < rhs.tstamp;
	 }
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
	 void Push(tstamp::Event);

	 /// \brief Erase the earliest event in the queue.
	 /// \details Writes the beginning event as a singles event and removes it from the
	 /// queue.
	 /// \note This should only be called in cases where we can assume that the earliest event
	 /// is not a coincidence - that is, either the container has reached the max size (enough time
	 /// has passed to guarantee no coincidences) or when we are done adding events.
	 void Pop();

	 /// \brief Check whether the maximum size has been reached
	 /// \details This function returns true if we have buffered enough events to effectively
	 /// guarantee that the earliest one is definitely a singles event
	 bool IsFull();

	 /// \brief Empty all events from the queue, writing each as a singles event.
	 void Cleanup();

private:
	 /// What to do in case of a coincidence event
	 void HandleCoinc(tstamp::Event, Iterator&);

	 /// What to do in case of a singles event
	 void HandleSingle(Iterator&);
};

} // namespace tstamp

#endif
