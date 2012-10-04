/// \file rootana/Timestamp.hxx
/// \author G. Christian
/// \brief Defines a child class of tstamp::Queue for use in rootana
#ifndef DRAGON_ROOTANA_TIMESTAMP_HXX
#define DRAGON_ROOTANA_TIMESTAMP_HXX
#include <list>
#include "utils/TStamp.hxx"
#include "dragon/Coinc.hxx"

namespace rootana {

class HistBase;

/// Derived class of tstamp::Queue for use in rootana
class TSQueue : public tstamp::Queue
{
public:
	/// Just Calls tstamp::Queue::Queue()
	TSQueue(double maxDelta): tstamp::Queue(maxDelta) { }

	/// Empty
	~TSQueue() { }

private:
	/// Handles singles event for rootana
	void HandleSingle(const midas::Event& event1) const;

	/// Handles coincidence events for rootana
	void HandleCoinc(const midas::Event& event1, const midas::Event& event2) const;
};

}


#endif
