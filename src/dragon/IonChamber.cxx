/// \file IonChamber.cxx
/// \brief Implements IonChamber.hxx
#include <string>
#include <iostream>
#include "utils/copy_array.h"
#include "midas/Odb.hxx"
#include "midas/Xml.hxx"
#include "vme/Constants.hxx"
#include "vme/V1190.hxx"
#include "vme/V792.hxx"
#include "vme/Vme.hxx"
#include "Tail.hxx"
#include "IonChamber.hxx"


// ====== struct dragon::IonChamber ====== //

dragon::IonChamber::IonChamber()
{
	reset();
}

void dragon::IonChamber::reset()
{
	for(int i=0; i< MAX_CHANNELS; ++i) {
		anode[i] = vme::NONE;
	}
	tof = vme::NONE;
	sum = vme::NONE;
}

void dragon::IonChamber::read_data(const vme::caen::V785 adcs[], const vme::caen::V1190& tdc)
{
	for(int i=0; i< MAX_CHANNELS; ++i) {
		const int whichAdc = variables.anode_module[i];
		assert (whichAdc< Tail::NUM_ADC); ///\todo Don't use an assert here
		const int whichAdcChannel = variables.anode_ch[i];

		anode[i] = adcs[whichAdc].get_data(whichAdcChannel);
	}
}

void dragon::IonChamber::calculate()
{
	if(!is_valid<int16_t>(anode, MAX_CHANNELS)) return;
	sum = 0;
	for(int i=0; i< MAX_CHANNELS; ++i) {
		sum += (double)anode[i];
	}
	/// \todo calculate tof
}


// ====== struct dragon::IonChamber::Variables ====== //

dragon::IonChamber::Variables::Variables()
{
	for(int i=0; i< MAX_CHANNELS; ++i) {
		anode_module[i] = 0;
		anode_ch[i] = i;
	}
	tof_ch = 1; /// \todo Update once plugged in
}

void dragon::IonChamber::Variables::set(const char* odb)
{
	/// \todo Set actual ODB paths, TEST!!
	const std::string pathAnodeModule = "Equipment/IonChamber/Variables/AnodeModule";
	const std::string pathAnodeCh     = "Equipment/IonChamber/Variables/AnodeChannel";
	const std::string pathTdcCh       = "Equipment/IonChamber/Variables/TDCChannel";

	if(strcmp(odb, "online")) { // Read from offline XML file
		midas::Xml mxml (odb);
		bool success = false;
		mxml.GetArray(pathAnodeModule.c_str(), MAX_CHANNELS, anode_ch, &success);
		mxml.GetArray(pathAnodeCh.c_str(), MAX_CHANNELS, anode_module, &success);
		mxml.GetValue(pathTdcCh.c_str(), tof_ch, &success);

		if(!success) {
			std::cerr << "Error (IonChamber::Variables::set): Couldn't set one or more variable values properly.\n";
		}
	}
	else { // Read from online ODB.
#ifdef MIDASSYS
		for(int i=0; i< MAX_CHANNELS; ++i) {
			anode_ch[i] = midas::Odb::ReadInt(pathAnodeCh.c_str(), i, 0);
			anode_module[i] = midas::Odb::ReadInt(pathAnodeModule.c_str(), i, 0);
		}
		tof_ch = midas::Odb::ReadInt(pathTdcCh.c_str(), 0, 0);
#else
		std::cerr << "MIDASSYS not defined, can't read from online ODB, no changes made.\n";
#endif
	}
}
