/// \file DragonEvents.cxx
/// \author G. Christian
/// \brief Implements DragonEvents.hxx
#include "utils/Error.hxx"
#include "DragonEvents.hxx"


namespace {
template <class T, class E>
inline void handle_event(rb::data::Wrapper<T>& data, const E* buf)
{
	data->reset();
	data->unpack(*buf);
	data->calculate();
} }


// ======== Class dragon::GammaEvent ======== //

rootbeer::GammaEvent::GammaEvent():
	fGamma("gamma", this, true, "") { }

void rootbeer::GammaEvent::HandleBadEvent()
{
	utils::err::Error("GammaEvent")
		<< "Unknown error encountered during event processing";
}

Bool_t rootbeer::GammaEvent::DoProcess(const void* addr, Int_t nchar)
{
  /*!
	 * Calls unpacking routines of dragon::Head to extract the raw MIDAS
	 * data into a dragon::Head structure. Then calculates higher-level
	 * parameters.
	 */
	if(!addr) {
		utils::err::Error("rootbeer::GammaEvent::DoProcess") << "Received NULL event address";
		return false;
	}

	handle_event(fGamma, AsMidasEvent(addr));
	return true;
}


// ======== Class dragon::HeavyIonEvent ======== //

rootbeer::HeavyIonEvent::HeavyIonEvent():
	fHeavyIon("hi", this, true, "") { }

void rootbeer::HeavyIonEvent::HandleBadEvent()
{
	utils::err::Error("HeavyIonEvent")
		<< "Unknown error encountered during event processing";
}

Bool_t rootbeer::HeavyIonEvent::DoProcess(const void* addr, Int_t nchar)
{
  /*!
	 * Calls unpacking routines of dragon::Tail to extract the raw MIDAS
	 * data into a dragon::Head structure. Then calculates higher-level
	 * parameters.
	 */
	if(!addr) {
		utils::err::Error("rootbeer::HeavyIonEvent::DoProcess") << "Received NULL event address";
		return false;
	}

	handle_event(fHeavyIon, AsMidasEvent(addr));
	return true;
}


// ======== Class dragon::CoincEvent ======== //

rootbeer::CoincEvent::CoincEvent():
	fCoinc("coinc", this, false, "") { }

void rootbeer::CoincEvent::HandleBadEvent()
{
	utils::err::Error("CoincEvent")
		<< "Unknown error encountered during event processing";
}

Bool_t rootbeer::CoincEvent::DoProcess(const void* addr, Int_t nchar)
{
	/*!
	 * Unpacks data from the head and tail MIDAS events into the corresponding
	 * fields of fDragon, then calls the calculate() methods of each.
	 * \todo Figure out a way to handle this without unpacking the events; in principle,
	 * they should have already been handled as singles events, thus we add extra overhead
	 * by going through the unpacking routines twice - it should be possible to buffer and
	 * then copy the already-unpacked head and tail structures directly.
	 */
	if(!addr) {
		utils::err::Error("rootbeer::CoincEvent::DoProcess") << "Received NULL event address";
		return false;
	}

	handle_event(fCoinc, AsCoincMidasEvent(addr));
	return true;
}
