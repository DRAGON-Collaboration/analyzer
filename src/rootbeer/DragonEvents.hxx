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
#include "TStamp.hxx"
#include "Dragon.hxx"


namespace rootbeer {

/// Global run parameters
class RunParameters: public rb::Event
{
private:
	/// Wrapper of dragon::RunParameters
	rb::data::Wrapper<dragon::RunParameters> fParameters;

public:
	/// Initializes fParameters
	RunParameters();

	/// Reset fParameters
	void Reset() { fParameters->reset(); }

private:
	/// Read parameters from the ODB (or .mid file)
	Bool_t DoProcess(const void*, Int_t);

	/// What to do in case of an error in event processing
	void HandleBadEvent();	
};

/// Timestamp diagnostics event
class TStampDiagnostics: public rb::Event
{
private:
	/// Wrapper of tstamp::Diagnostics
	rb::data::Wrapper<tstamp::Diagnostics> fDiagnostics;

public:
	/// Initializes fDiagnostics
	TStampDiagnostics();

	/// Empty
	~TStampDiagnostics() {}

	/// Returns pointer to fDiagnostics.Get()
	tstamp::Diagnostics* GetDiagnostics();

	/// Reset diagnostics
	void Reset() { fDiagnostics->reset(); }

private:
	/// Nothing to do - queue fills data automatically
	Bool_t DoProcess(const void*, Int_t) { return true; }
	
	/// What to do in case of an error in event processing
	void HandleBadEvent();
};

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

	/// Returns the TSC bank name
	const char* TscBank() const
		{ return fGamma->banks.tsc; }

	/// Read variables from ODB
	void ReadOdb();

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

	/// Returns the TSC bank name
	const char* TscBank() const
		{ return fHeavyIon->banks.tsc; }

	/// Read variables from ODB
	void ReadOdb();

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

	/// Read variables from ODB
	void ReadOdb();

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

	/// Read variables from ODB
	void ReadOdb();

	/// Reset scalers
	void Reset() { fScaler->reset(); }

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

	/// Read variables from ODB
	void ReadOdb();

	/// Reset scalers
	void Reset() { fScaler->reset(); }

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
