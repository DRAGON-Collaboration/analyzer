//! \file DragonRootbeer.hxx
//! \author G. Christian
//! \brief Definition of DRAGON user classes for ROOTBEER.
#ifndef DRAGON_ROOTBEER_HXX
#define DRAGON_ROOTBEER_HXX

// Rootbeer includes //
#include "Buffer.hxx"
#include "Event.hxx"
#include "Data.hxx"

#ifdef MIDASSYS
#include "midas/TMidasOnline.h" // Rootbeer TMidasOnline
#endif

// Dragon includes //
#include "utils/TStamp.hxx"
#include "midas/Event.hxx"


namespace rb
{

#include <utility>
#include "dragon/Dragon.hxx"
enum {
  DRAGON_EVENT      = 1,
  DRAGON_SCALER     = 2,
	GAMMA_EVENT       = 3,
	HI_EVENT          = 4,
	COINCIDENCE_EVENT = 5
};

class CoincideceEvent;
class GammaEvent : public rb::Event
{
private:
	rb::data::Wrapper<dragon::gamma::Gamma> fGamma;
public:
	GammaEvent();
	~GammaEvent() {}
private:
	const dragon::MidasEvent* Cast(const void* addr) {return reinterpret_cast<const dragon::MidasEvent*>(addr);}
	Bool_t DoProcess(const void* event_address, Int_t nchar);
	void HandleBadEvent() {Error("GammaEvent", "Something went wrong!!");}
	friend class CoincidenceEvent;
};

class HeavyIonEvent : public rb::Event
{
private:
	rb::data::Wrapper<dragon::hion::HeavyIon> fHeavyIon;
public:
	HeavyIonEvent();
	~HeavyIonEvent() {}
private:
	const dragon::MidasEvent* Cast(const void* addr) {return reinterpret_cast<const dragon::MidasEvent*>(addr);}
	Bool_t DoProcess(const void* event_address, Int_t nchar);
	void HandleBadEvent() {Error("HeavyIonEvent", "Something went wrong!!");}
	friend class CoincidenceEvent;
};

class CoincidenceEvent : public rb::Event
{
private:
	rb::data::Wrapper<dragon::Dragon> fDragon;
public:
	CoincidenceEvent();
	~CoincidenceEvent() {}
private:
	const dragon::CoincMidasEvent* Cast(const void* addr) {return reinterpret_cast<const dragon::CoincMidasEvent*>(addr);}
	Bool_t DoProcess(const void* event_address, Int_t nchar);
	void HandleBadEvent() {Error("CoincidenceEvent", "Something went wrong!!");}
};



#else

// Throw a compile-time error.  The user should remove this one he/she has done what's required.
#error "You need to define a derived class of rb::BufferSource and implement rb::BufferSource::New()."

class /* ClassName */ : public rb::BufferSource()
{
  protected:
  // Data, etc.
  public:
  // Required functions //
	/*Name*/ (); // Constructor
	virtual ~/*Name*/ (); // Destructor
	virtual Bool_t OpenFile(const char* file_name, char** other = 0, int nother = 0);
	virtual Bool_t ConnectOnline(const char* host, const char* other_arg = "", char** other_args = 0, int n_others = 0);
	virtual Bool_t ReadBufferOffline();
	virtual Bool_t ReadBufferOnline();
	virtual Bool_t UnpackBuffer();
	virtual void CloseFile();
	virtual void DisconnectOnline();
  protected:
	// Internal functions, etc.
};

rb::BufferSource* rb::BufferSource::New() {
  // Needs to be implemented //

}

#endif 


#endif
