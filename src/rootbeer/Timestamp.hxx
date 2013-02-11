/// \file rootbeer/Timestamp.hxx
/// \author G. Christian
/// \brief Defines a child class of tstamp::Queue for use in ROOTBEER
#ifndef DRAGON_RB_TIMESTAMP_HXX
#define DRAGON_RB_TIMESTAMP_HXX
#include "utils/TStamp.hxx"


namespace rootbeer {

/// Derived class of tstamp::Queue for use in ROOTBEER
class TSQueue : public tstamp::Queue
{
public:
	/// Just Calls tstamp::Queue::Queue()
	TSQueue(double maxDelta): tstamp::Queue(maxDelta) { }

	/// Empty
	~TSQueue() { }

private:
	/// Handles singles event for ROOTBEER
	virtual void HandleSingle(const midas::Event& event1) const;

	/// Handles coincidence events for ROOTBEER
	virtual void HandleCoinc(const midas::Event& event1, const midas::Event& event2) const;

	/// Handles diagnostic events for ROOTBEER
	virtual void HandleDiagnostics(tstamp::Diagnostics* d) const;
};

}


#endif
