/// \file SurfaceBarrier.cxx
/// \author G. Christian
/// \brief Implements SurfaceBarrier.hxx
#include <string>
#include <iostream>
#include "utils/copy_array.h"
#include "midas/Odb.hxx"
#include "midas/Xml.hxx"
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
		q[i] = vme::NONE;
	}
}

void dragon::SurfaceBarrier::read_data(const vme::caen::V785 adcs[], const vme::caen::V1190& tdc)
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
	const std::string pathModule = "Equipment/SurfaceBarrier/Variables/AnodeModule";
	const std::string pathCh     = "Equipment/SurfaceBarrier/Variables/AnodeChannel";

	if(strcmp(odb, "online")) { // Read from offline XML file
		midas::Xml mxml (odb);
		bool success = false;
		mxml.GetArray(pathModule.c_str(), MAX_CHANNELS, module, &success);
		mxml.GetArray(pathCh.c_str(), MAX_CHANNELS, ch, &success);

		if(!success) {
			std::cerr << "Error (SurfaceBarrier::Variables::set): Couldn't set one or more variable values properly.\n";
		}
	}
	else { // Read from online ODB.
#ifdef MIDASSYS
		for(int i=0; i< MAX_CHANNELS; ++i) {
			ch[i] = midas::Odb::ReadInt(pathCh.c_str(), i, 0);
			module[i] = midas::Odb::ReadInt(pathModule.c_str(), i, 0);
		}
#else
		std::cerr << "MIDASSYS not defined, can't read from online ODB, no changes made.\n";
#endif
	}
}

