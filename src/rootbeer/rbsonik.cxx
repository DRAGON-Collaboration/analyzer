///
/// \file rbsonik.cxx
/// \author G. Christian
/// \brief Implements rbsonik.hxx
///
#include <ctime>
#include <cassert>

#include <TSystem.h>
#include <TFile.h>

// ROOTBEER includes //
#include <TString.h>
#include "Rint.hxx"
#include "Attach.hxx"
#include "Rootbeer.hxx"
#include "Event.hxx"

// DRAGON includes //
#include "utils/ErrorDragon.hxx"
#include "utils/definitions.h"
#include "utils/Functions.hxx"
#include "midas/Database.hxx"
#include "midas/Event.hxx"
#include "rbdragon.hxx"
#include "rbsonik.hxx"



// namespace {

// dragon::Head  gDummyHead;
// dragon::Coinc gDummyCoinc;
// tstamp::Diagnostics gDummyDiag;

// }

rbsonik::MidasBuffer::MidasBuffer():
	rbdragon::MidasBuffer() { }
// 	rbdragon::MidasBuffer(&gDummyHead,
// 												rb::Event::Instance<SonikEvent>()->GetTail(),
// 												&gDummyCoinc,
// 												rb::Event::Instance<rbdragon::EpicsEvent>()->Get(),
// 												rb::Event::Instance<rbdragon::HeadScaler>()->Get(),
// 												rb::Event::Instance<rbdragon::TailScaler>()->Get(),
// 												rb::Event::Instance<rbdragon::AuxScaler>()->Get(),
// 												rb::Event::Instance<rbdragon::RunParameters>()->Get(),
// 												&gDummyDiag)
// {
// }

// void rbsonik::MidasBuffer::ReadVariables(midas::Database* db)
// {
// 	gDummyHead.set_variables(db);
// 	rb::Event::Instance<SonikEvent>()->GetTail()->set_variables(db);
// 	gDummyCoinc.set_variables(db);
// 	rb::Event::Instance<rbdragon::HeadScaler>()->ReadVariables(db);
// 	rb::Event::Instance<rbdragon::TailScaler>()->ReadVariables(db);
// 	rb::Event::Instance<rbdragon::AuxScaler>()->ReadVariables(db);
// }

Bool_t rbsonik::MidasBuffer::UnpackEvent(void* header, char* data)
{
	///
	/// Does the following:

	midas::Event::Header* phead = reinterpret_cast<midas::Event::Header*>(header);

	/// - If BOR event (ODB dump), set variables from there
	if (phead->fEventId == MIDAS_BOR) {
		midas::Database db(data, phead->fDataSize);

		// read variables
		this->ReadVariables(&db);
	}

	/// - For all other events, delegate to rb::Unpacker, then call the Process() function
	std::vector<Int_t> codes = fUnpacker.UnpackMidasEvent(phead, data);

	for (size_t i=0; i< codes.size(); ++i) {
		rb::Event* event = rb::Rint::gApp()->GetEvent (codes[i]);
		if(event) event->Process(0, 0);

		if(event && codes[i] == DRAGON_TAIL_EVENT) {
			rb::Event::Instance<SonikEvent>()->Process(0,0);
		}
	}

	return kTRUE;
}


// ======== Class rbsonik::SonikEvent ======== //

rbsonik::SonikEvent::SonikEvent():
	fSonik("sonik", this, true, "") { }

void rbsonik::SonikEvent::HandleBadEvent()
{
	dragon::utils::Error("SonikEvent", __FILE__, __LINE__)
		<< "Unknown error encountered during event processing";
}

void rbsonik::SonikEvent::ReadVariables(midas::Database* db)
{
	fSonik->variables.set(db);
}

Bool_t rbsonik::SonikEvent::DoProcess(const void*, Int_t)
{
	fSonik->reset();
	
	dragon::Tail* ptail = rb::Event::Instance<rbdragon::HeavyIonEvent>()->Get();
	fSonik->read_data(ptail->v785, ptail->v1190);
	fSonik->calculate();

	return kTRUE;
}
