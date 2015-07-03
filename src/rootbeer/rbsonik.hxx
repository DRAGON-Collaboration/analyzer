#ifndef RB_SONIK_INCLUDE_GUARD
#define RB_SONIK_INCLUDE_GUARD
///
/// \file rbsonik.hxx
/// \author G. Christian
/// \brief Defines classes to interface sonik with the ROOTBEER framework.
///
#include <memory>

// ROOTBEER includes
#include "Main.hxx"
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
#include "Sonik.hxx"
#include "rbsymbols.hxx"
#include "rbdragon.hxx"


namespace rbsonik {


class MidasBuffer: public rbdragon::MidasBuffer {
public:
	/// Constructor
	MidasBuffer();
	/// No actions needed
	virtual ~MidasBuffer() { }
	/// Look at the event id and handle as appropriate.
	virtual Bool_t UnpackEvent(void* header, char* data);
};


/// Sonik event
class SonikEvent: public rb::Event
{
private:
	/// Holds sonik data
	rb::data::Wrapper<Sonik> fSonik;

public:
	/// Initializes fSonik
	SonikEvent();

	/// empty
	~SonikEvent() { }

	/// Read variables from ODB
	void ReadVariables(midas::Database* db);

	/// Get raw pointer to sonik class
	Sonik* Get() { return fSonik.Get(); }

private:
	/// Casts from const void* to const midas::Event*
	const midas::Event* AsMidasEvent(const void* addr)
		{ return reinterpret_cast<const midas::Event*>(addr); }

	/// Unpack event
	Bool_t DoProcess(const void* data, Int_t nchar);

	/// What to do in case of an error in event processing
	void HandleBadEvent();
};

}





#endif
