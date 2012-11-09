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
}

template <class T>
inline void odb_read(rb::data::Wrapper<T>& data)
{
	data->set_variables("online");
}

} // namespace


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

void rootbeer::GammaEvent::ReadOdb()
{
	odb_read(fGamma);
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

void rootbeer::HeavyIonEvent::ReadOdb()
{
	odb_read(fHeavyIon);
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

void rootbeer::CoincEvent::ReadOdb()
{
	odb_read(fCoinc);
}


// ======== Class dragon::HeadScaler ======== //

rootbeer::HeadScaler::HeadScaler():
	fScaler("head_scaler", this, true, "")
{
	fScaler->variables.set_bank_names("SCH");
}

void rootbeer::HeadScaler::HandleBadEvent()
{
	utils::err::Error("HeadScaler")
		<< "Unknown error encountered during event processing";
}

Bool_t rootbeer::HeadScaler::DoProcess(const void* addr, Int_t nchar)
{
	/*!
	 * Unpacks into scaler event structure.
	 * \returns true is given a valid \d addr input, false otherwise
	 */
	if(!addr) {
		utils::err::Error("rootbeer::HeadScaler::DoProcess") << "Received NULL event address";
		return false;
	}

	fScaler->unpack(*AsMidasEvent(addr));
	return true;
}

void rootbeer::HeadScaler::ReadOdb()
{
	odb_read(fScaler);
}


// ======== Class dragon::TailScaler ======== //

rootbeer::TailScaler::TailScaler():
	fScaler("tail_scaler", this, true, "")
{
	fScaler->variables.set_bank_names("SCT");
}

void rootbeer::TailScaler::HandleBadEvent()
{
	utils::err::Error("TailScaler")
		<< "Unknown error encountered during event processing";
}

Bool_t rootbeer::TailScaler::DoProcess(const void* addr, Int_t nchar)
{
	/*!
	 * Unpacks into scaler event structure.
	 * \returns true is given a valid \d addr input, false otherwise
	 */
	if(!addr) {
		utils::err::Error("rootbeer::TailScaler::DoProcess") << "Received NULL event address";
		return false;
	}

	fScaler->unpack(*AsMidasEvent(addr));
	return true;
}

void rootbeer::TailScaler::ReadOdb()
{
	odb_read(fScaler);
}
