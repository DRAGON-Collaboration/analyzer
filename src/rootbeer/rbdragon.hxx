#ifndef RB_DRAGON_INCLUDE_GUARD
#define RB_DRAGON_INCLUDE_GUARD
///
/// \file rbdragon.hxx
/// \author G. Christian
/// \brief Defines classes to interface dragon with the ROOTBEER framework.
///

// ROOTBEER includes
#include "Data.hxx"
#include "Event.hxx"
#include "midas/TMidasFile.h"
#include "midas/TMidasEvent.h"
#include "midas/MidasBuffer.hxx"

// DRAGON includes
#include "midas/Event.hxx"
#include "Unpack.hxx"
#include "TStamp.hxx"
#include "Dragon.hxx"


namespace midas { class Database; }


/// Encloses DRAGON-related rootbeer stuff.
namespace rbdragon {

/// Dragon-specific r::MidasBuffer class
class MidasBuffer: public rb::MidasBuffer {
public:
	/// Constructor
	MidasBuffer();
	/// No actions needed
	~MidasBuffer();
	/// Look at the event id and handle as appropriate.
	Bool_t UnpackEvent(void* header, char* data);
	/// Virtual run start transition handler
	void RunStartTransition(Int_t runnum);
	/// Virtual run stop transition handler
	void RunStopTransition(Int_t runnum);
private:
	/// Helper function to read variables from online or offline database.
	void ReadVariables(midas::Database* db);

private:
	/// Unpacker instance
	dragon::Unpacker fUnpacker;
};


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

	/// Get raw pointer
	dragon::RunParameters* Get() { return fParameters.Get(); }

private:
	/// Nothing to do - Unpacker class handles it
	Bool_t DoProcess(const void*, Int_t) { return kTRUE; }

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

	/// Get raw pointer
	tstamp::Diagnostics* Get() { return fDiagnostics.Get(); }

	/// Reset diagnostics
	void Reset() { fDiagnostics->reset(); }

private:
	/// Nothing to do - Unpacker class handles it
	Bool_t DoProcess(const void*, Int_t) { return kTRUE; }
	
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
		{ return fGamma->variables.bk_tsc; }

	/// Read variables from ODB
	void ReadVariables(midas::Database* db);

	/// Get raw pointer
	dragon::Head* Get() { return fGamma.Get(); }

private:
	/// Casts from const void* to const midas::Event*
	const midas::Event* AsMidasEvent(const void* addr)
		{ return reinterpret_cast<const midas::Event*>(addr); }

	/// Nothing to do - Unpacker class handles it
	Bool_t DoProcess(const void*, Int_t) { return kTRUE; }

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
		{ return fHeavyIon->variables.bk_tsc; }

	/// Read variables from ODB
	void ReadVariables(midas::Database* db);

	/// Get raw pointer
	dragon::Tail* Get() { return fHeavyIon.Get(); }

private:
	/// Casts from const void* to const midas::Event*
	const midas::Event* AsMidasEvent(const void* addr)
		{ return reinterpret_cast<const midas::Event*>(addr); }

	/// Nothing to do - Unpacker class handles it
	Bool_t DoProcess(const void*, Int_t) { return kTRUE; }

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
	void ReadVariables(midas::Database* db);

	/// Get raw pointer
	dragon::Coinc* Get() { return fCoinc.Get(); }

private:
	/// Casts from const void* to const midas::CoincEvent*
	const midas::CoincEvent* AsCoincMidasEvent(const void* addr)
		{ return reinterpret_cast<const midas::CoincEvent*>(addr); }

	/// Nothing to do - Unpacker class handles it
	Bool_t DoProcess(const void*, Int_t) { return kTRUE; }

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
	void ReadVariables(midas::Database* db);

	/// Reset scalers
	void Reset() { fScaler->reset(); }

	/// Get raw pointer
	dragon::Scaler* Get() { return fScaler.Get(); }

private:
	/// Casts from const void* to const midas::Event*
	const midas::Event* AsMidasEvent(const void* addr)
		{ return reinterpret_cast<const midas::Event*>(addr); }

	/// Nothing to do - Unpacker class handles it
	Bool_t DoProcess(const void*, Int_t) { return kTRUE; }

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
	void ReadVariables(midas::Database* db);

	/// Reset scalers
	void Reset() { fScaler->reset(); }

	/// Get raw pointer
	dragon::Scaler* Get() { return fScaler.Get(); }

private:
	/// Casts from const void* to const midas::Event*
	const midas::Event* AsMidasEvent(const void* addr)
		{ return reinterpret_cast<const midas::Event*>(addr); }

	/// Nothing to do - Unpacker class handles it
	Bool_t DoProcess(const void*, Int_t) { return kTRUE; }

	/// What to do in case of an error in event processing
	void HandleBadEvent();
};

} // namespace rbdragon



#ifdef FOR_DOXYGEN_ONLY
namespace rb {

//! Class that runs the interactive ROOT application.
/*!
 * From http://trshare.triumf.ca/~gchristian/rootbeer/doc/html/classrb_1_1_rint.html :
 *
 * We can essentially use the normal <tt>TRint</tt>, except
 * we need to override the Terminate() method to stop threaded processes.
 */
class Rint {
public:
  /// Required implementation of rb::Rint::RegisterEvents()
	void RegisterEvents();
};

/// Abstract class interfacing rb::BufferSource with MIDAS.
class MidasBuffer {
public:
	/// Create new instance of the rdbragon::MidasBuffer class.
	static MidasBuffer::Instance();
};

/// Required user function to return the main() implementation
rb::Main* rb::GetMain();

}
#endif

#endif
