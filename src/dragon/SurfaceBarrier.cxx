/// \file SurfaceBarrier.cxx
/// \author G. Christian
/// \brief Implements SurfaceBarrier.hxx
#include <string>
#include <iostream>
#include "midas/Database.hxx"
#include "vme/V1190.hxx"
#include "vme/V792.hxx"
#include "Tail.hxx"
#include "SurfaceBarrier.hxx"


// ====== struct dragon::SurfaceBarrier ====== //

dragon::SurfaceBarrier::SurfaceBarrier()
{
	reset();
}

void dragon::SurfaceBarrier::reset()
{
	for(int i=0; i< MAX_CHANNELS; ++i) {
		q[i] = dragon::NO_DATA;
	}
}

void dragon::SurfaceBarrier::read_data(const vme::V785 adcs[], const vme::V1190& tdc)
{
	for(int i=0; i< MAX_CHANNELS; ++i) {
		const int whichAdc = variables.module[i];
		assert (whichAdc< Tail::NUM_ADC); ///\todo Don't use an assert here
		const int whichAdcChannel = variables.ch[i];

		q[i] = adcs[whichAdc].get_data(whichAdcChannel);
	}
}

// ====== struct dragon::SurfaceBarrier::Variables ====== //

dragon::SurfaceBarrier::Variables::Variables()
{
	for(int i=0; i< MAX_CHANNELS; ++i) {
		module[i] = 1;
		ch[i] = i;
	}
}

void dragon::SurfaceBarrier::Variables::set(const char* odb)
{
	/// \todo Set actual ODB paths, TEST!!
	const char* const pathModule = "Equipment/SurfaceBarrier/Variables/AnodeModule";
	const char* const pathCh     = "Equipment/SurfaceBarrier/Variables/AnodeChannel";

	midas::Database database(odb);
	database.ReadArray(pathModule, module, MAX_CHANNELS);
	database.ReadArray(pathCh, module, MAX_CHANNELS);
}
