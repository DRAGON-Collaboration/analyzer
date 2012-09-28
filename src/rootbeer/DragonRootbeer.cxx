//! \file DragonRootbeer.cxx
//! \author G. Christian
//! \brief Implementation of DRAGON user functions for ROOTBEER.
#include "DragonRootbeer.hxx"
#include "utils/Error.hxx"
#include "dragon/definitions.h"

rb::BufferSource* rb::BufferSource::New() {
  return new rb::Midas();
}

Bool_t rb::Midas::ReadBufferOffline() {
	TMidasEvent temp;
	bool ret = fFile.Read(&temp);
	if (ret) {
		memcpy (fBuffer, temp.GetEventHeader(), sizeof(EventHeader_t));
		memcpy (fBuffer + sizeof(EventHeader_t), temp.GetData(), temp.GetDataSize());
	}
	return ret;
}

Bool_t rb::Midas::ConnectOnline(const char* host, const char* experiment, char** unused, int unused2) {
#ifdef MIDAS_ONLINE
  TMidasOnline* onlineMidas = TMidasOnline::instance();
  int err = onlineMidas->connect(host, experiment, "rootbeer");
  if (err) return kFALSE; // Message from TOnlineMidas::connect

  onlineMidas->setTransitionHandlers(RunStart, RunStop, RunPause, RunResume);
  onlineMidas->registerTransitions();
  fRequestId = onlineMidas->eventRequest("SYNC", -1, -1, (1<<1));
	return kTRUE;
#else
  return kFALSE;
#endif
}


namespace { inline bool isOnlineAttached() { return rb::Thread::IsRunning(rb::attach::ONLINE_THREAD_NAME); } }

Bool_t rb::Midas::ReadBufferOnline() { /// \todo figure out poll length
#ifdef MIDAS_ONLINE
  Bool_t ret = kFALSE;
  TMidasOnline* onlineMidas = TMidasOnline::instance();
//  char pEvent[100*1024];

	const int poll_length = 0;

  int size = 0;
  do { // loop until we get an error or event, or quit polling, or unattach
    size = onlineMidas->receiveEvent(fRequestId, fBuffer, sizeof(fBuffer), kTRUE);
  } while (size == 0 && rb::Thread::IsRunning(rb::attach::ONLINE_THREAD_NAME) && onlineMidas->poll(poll_length));

  if(size == 0) // Unattached or stopped polling
		 ret = kFALSE;
  else if(size > 0) { // Got data, copy into midas event  
    //! \todo: byte ordering??
    // memcpy(fBuffer.GetEventHeader(), pEvent, sizeof(EventHeader_t));
    // fBuffer.SetData(size, pEvent+sizeof(EventHeader_t));
    ret = kTRUE;
  }    
  else { // Error reading event
    err::Error("rb::Midas::ReadBufferOnline") << "onlineMidas->receiveEvent return val: " << size << ".";
    ret = kFALSE;
  }

  return ret;


#else
  return kFALSE;
#endif
}
namespace { 	DragonQueue gQueue(10e6);  }

void rb::Midas::FlushQueue() {
	gQueue.Flush();
}

void rb::Midas::RunStop(int transition, int run_number, int trans_time) {
	gQueue.Flush();
  Info("rb::Midas", "Stopping run number %i.", run_number);
}

Bool_t rb::Midas::UnpackBuffer() {

#ifdef MIDAS_BUFFERS
  // (DRAGON test setup)
  Short_t eventId = ((EventHeader_t*)fBuffer)->fEventId;
  switch(eventId) {
  case DRAGON_HEAD_EVENT:
		gQueue.Push(dragon::MidasEvent("TSCH", fBuffer, ((EventHeader_t*)fBuffer)->fDataSize));
		break;
	case DRAGON_TAIL_EVENT:
		gQueue.Push(dragon::MidasEvent("TSCT", fBuffer, ((EventHeader_t*)fBuffer)->fDataSize));
		break;
  case DRAGON_HEAD_SCALER: // scaler
		///\ todo Implement chris's stuff
    break;
  case DRAGON_TAIL_SCALER: // scaler
		///\ todo Implement chris's stuff
    break;
  default:
    break;
  }
  return kTRUE;
#else
  return kFALSE;
#endif
}

CoincidenceEvent::CoincidenceEvent(): fDragon("coinc", this, false, "") { }

Bool_t CoincidenceEvent::DoProcess(const void* addr, Int_t nchar) {
	const dragon::CoincMidasEvent* coinc = Cast(addr);

	dragon::gamma::Gamma gamma;
	gamma.unpack(*(coinc->fGamma));
	gamma.read_data();
	gamma.calculate();

	dragon::hion::HeavyIon heavyIon;
	heavyIon.unpack(*(coinc->fHeavyIon));
	heavyIon.read_data();
	heavyIon.calculate();

	fDragon->read_event(gamma, heavyIon);

	return true;
}

GammaEvent::GammaEvent(): fGamma("gamma", this, true, "") { }

Bool_t GammaEvent::DoProcess(const void* addr, Int_t nchar) {
  const dragon::MidasEvent* fEvent = Cast(addr);
  if(fEvent) {
		fGamma->unpack(*fEvent);
		fGamma->read_data();
		fGamma->calculate();
    return true;
  }
  else return false;
}

HeavyIonEvent::HeavyIonEvent(): fHeavyIon("hi", this, true, "") { }

Bool_t HeavyIonEvent::DoProcess(const void* addr, Int_t nchar) {
  const dragon::MidasEvent* fEvent = Cast(addr);
  if(fEvent) {
		fHeavyIon->unpack(*fEvent);
		fHeavyIon->read_data();
		fHeavyIon->calculate();
    return true;
  }
  else return false;
}

void rb::Rint::RegisterEvents() {
  // Register events here //
  RegisterEvent<CoincidenceEvent>(COINCIDENCE_EVENT, "CoincidenceEvent");
	RegisterEvent<GammaEvent>(GAMMA_EVENT, "GammaEvent");
	RegisterEvent<HeavyIonEvent>(HI_EVENT, "HeavyIonEvent");
}


