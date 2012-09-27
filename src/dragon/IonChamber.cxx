/// \file IonChamber.cxx
/// \brief Implements IonChamber.hxx
#include <string>
#include <iostream>
#include "midas/Database.hxx"
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

void dragon::IonChamber::read_data(const vme::V785 adcs[], const vme::V1190& tdc)
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
	const char* const pathAnodeModule = "Equipment/IonChamber/Variables/AnodeModule";
	const char* const pathAnodeCh     = "Equipment/IonChamber/Variables/AnodeChannel";
	const char* const pathTdcCh       = "Equipment/IonChamber/Variables/TDCChannel";
	
	midas::Database database(odb);
	database.ReadArray(pathAnodeModule, anode_module, MAX_CHANNELS);
	database.ReadArray(pathAnodeCh, anode_ch, MAX_CHANNELS);
	database.ReadValue(pathTdcCh, tof_ch);
}
