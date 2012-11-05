/// \file DragonEvents.hxx
/// \author G. Christian
/// \brief Defines DRAGON event classes for use in ROOTBEER.
#ifndef DRAGON_RB_EVENT_HXX
#define DRAGON_RB_EVENT_HXX
// ROOTBEER includes //
#include "Event.hxx"
#include "Data.hxx"

// DRAGON includes //
#include "midas/Event.hxx"
#include "dragon/Head.hxx"
#include "dragon/Tail.hxx"
#include "dragon/Coinc.hxx"
#include "dragon/Scaler.hxx"


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

/// Coincidence event
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

/// Head scaler event
class HeadScaler : public rb::Event
{
private:
	/// Wrapper of dragon::Scaler class that stored the unpacked scaler data
	rb::data::Wrapper<dragon::Scaler> fScaler;

public:
	/// Initializes fGamma
	HeadScaler();

	/// Empty
	~HeadScaler() {}

private:
	/// Casts from const void* to const midas::Event*
	const midas::Event* AsMidasEvent(const void* addr)
		{ return reinterpret_cast<const midas::Event*>(addr); }

	/// Does the work of processing a received event within a loop
	Bool_t DoProcess(const void* event_address, Int_t nchar);

	/// What to do in case of an error in event processing
	void HandleBadEvent();
};

/// Tail scaler event
class TailScaler : public rb::Event
{
private:
	/// Wrapper of dragon::Scaler class that stored the unpacked scaler data
	rb::data::Wrapper<dragon::Scaler> fScaler;

public:
	/// Initializes fGamma
	TailScaler();

	/// Empty
	~TailScaler() {}

private:
	/// Casts from const void* to const midas::Event*
	const midas::Event* AsMidasEvent(const void* addr)
		{ return reinterpret_cast<const midas::Event*>(addr); }

	/// Does the work of processing a received event within a loop
	Bool_t DoProcess(const void* event_address, Int_t nchar);

	/// What to do in case of an error in event processing
	void HandleBadEvent();
};

} // namespace rootbeer



#endif
