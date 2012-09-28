/// \file DragonEvents.hxx
/// \author G. Christian
/// \brief Defines DRAGON event classes for use in ROOTBEER.
#ifndef DRAGON_RB_EVENT_HXX
#define DRAGON_RB_EVENT_HXX
#include "Event.hxx"        // rb::Event (rootbeer)
#include "Data.hxx"         // rb::Data<T> (rootbeer)
#include "midas/Event.hxx"  // midas::Event (dragon)
#include "dragon/Head.hxx"
#include "dragon/Tail.hxx"
#include "dragon/Coinc.hxx"


namespace rootbeer {

/// Singles head (gamma-ray) event
class GammaEvent: public rb::Event
{
private:
	/// Wrapper of dragon::Head class that stores the unpacked event data
	rb::data::Wrapper<dragon::Head> fGamma;

public:
	/// Initializes fGamma
	GammaEvent();

	/// Empty
	~GammaEvent() {}

private:
	/// Casts from const void* to const midas::Event*
	const midas::Event* AsMidasEvent(const void* addr)
		{ return reinterpret_cast<const midas::Event*>(addr); }

	/// Does the work of processing a received event within a loop
	Bool_t DoProcess(const void* event_address, Int_t nchar);

	/// What to do in case of an error in event processing
	void HandleBadEvent();
};


/// Singles tail (heavy-ion) event
class HeavyIonEvent : public rb::Event
{
private:
	/// Wrapper of dragon::Tail class that stores the unpacked event data
	rb::data::Wrapper<dragon::Tail> fHeavyIon;

public:
	/// Initializes fHeavyIon;
	HeavyIonEvent();

	/// Empty
	~HeavyIonEvent() {}

private:
	/// Casts from const void* to const midas::Event*
	const midas::Event* AsMidasEvent(const void* addr)
		{ return reinterpret_cast<const midas::Event*>(addr); }

	/// Does the work of processing a received event within a loop
	Bool_t DoProcess(const void* event_address, Int_t nchar);

	/// What to do in case of an error in event processing
	void HandleBadEvent();
};

class CoincEvent : public rb::Event
{
private:
	/// Wrapper of dragon::Coinc class that stores the unpacked event data
	rb::data::Wrapper<dragon::Coinc> fCoinc;

public:
	/// Initializes fCoinc
	CoincEvent();

	/// Empty
	~CoincEvent() {}

private:
	/// Casts from const void* to const midas::CoincEvent*
	const midas::CoincEvent* AsCoincMidasEvent(const void* addr)
		{ return reinterpret_cast<const midas::CoincEvent*>(addr); }

	/// Does the work of processing a received event within a loop
	Bool_t DoProcess(const void* event_address, Int_t nchar);

	/// What to do in case of an error in event processing
	void HandleBadEvent();
};

} // namespace rootbeer



#endif
