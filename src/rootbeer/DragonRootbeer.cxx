//! \file DragonRootbeer.cxx
//! \author G. Christian
//! \brief Implementation of DRAGON user functions for ROOTBEER.
#include "DragonRootbeer.hxx"
#include "utils/Error.hxx"
#include "dragon/definitions.h"

rb::BufferSource* rb::BufferSource::New() {
  return new rb::Midas();
}

void rb::Rint::RegisterEvents() {
  // Register events here //
  RegisterEvent<CoincidenceEvent>(COINCIDENCE_EVENT, "CoincidenceEvent");
	RegisterEvent<GammaEvent>(GAMMA_EVENT, "GammaEvent");
	RegisterEvent<HeavyIonEvent>(HI_EVENT, "HeavyIonEvent");
}


