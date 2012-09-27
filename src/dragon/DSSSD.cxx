/// \file DSSSD.cxx
/// \brief Implements DSSSD.hxx
#include <string>
#include <cassert>
#include <iostream>
#include "midas/Database.hxx"
#include "vme/Vme.hxx"
#include "vme/V1190.hxx"
#include "vme/V792.hxx"
#include "Tail.hxx"
#include "DSSSD.hxx"


// ====== class dragon::DSSSD ====== //

dragon::DSSSD::DSSSD() :
	variables()
{
	reset();
}

void dragon::DSSSD::reset()
{
	for(int i=0; i< MAX_CHANNELS; ++i) {
		qraw[i] = vme::NONE;
	}
	tof = vme::NONE;
}

void dragon::DSSSD::read_data(const vme::V785 adcs[], const vme::V1190& v1190)
{
	for(int i=0; i< MAX_CHANNELS; ++i) {
		const int whichAdc = variables.qdc_module[i];
		assert (whichAdc< Tail::NUM_ADC); ///\todo Don't use an assert here
		const int whichAdcChannel = variables.qdc_ch[i];

		qraw[i] = adcs[whichAdc].get_data(whichAdcChannel);
	}
	tof = vme::NONE; /// \todo Calculate DSSSD tof
}


// ====== class dragon::DSSSD::Variables ====== //

dragon::DSSSD::Variables::Variables()
{
	for(int i=0; i< MAX_CHANNELS; ++i) {
		qdc_module[i] = 1;
		qdc_ch[i] = i;
	}
	tof_ch = 1; /// \todo Update once plugged in
}

void dragon::DSSSD::Variables::set(const char* odb)
{
	/// \todo Set actual ODB paths, TEST!!
	const char* const pathAdcModule = "Equipment/DSSSD/Variables/ADCModule";
	const char* const pathAdcCh     = "Equipment/DSSSD/Variables/ADCChannel";
	const char* const pathTdcCh     = "Equipment/DSSSD/Variables/TDCChannel";
	
	midas::Database database(odb);
	database.ReadArray(pathAdcModule, qdc_module, MAX_CHANNELS);
	database.ReadArray(pathAdcCh, qdc_ch, MAX_CHANNELS);
	database.ReadValue(pathTdcCh, tof_ch);
}
