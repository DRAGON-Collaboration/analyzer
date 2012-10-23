/// \file DSSSD.cxx
/// \brief Implements DSSSD.hxx
#include <string>
#include <cassert>
#include <iostream>
#include "midas/Database.hxx"
#include "utils/Valid.hxx"
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
	reset_array(e, MAX_CHANNELS);
	reset_data(tof);
}

void dragon::DSSSD::read_data(const vme::V785 adcs[], const vme::V1190& v1190)
{
	for(int i=0; i< MAX_CHANNELS; ++i) {
		const int whichAdc = variables.qdc_module[i];
		assert (whichAdc< Tail::NUM_ADC);

		const int whichAdcChannel = variables.adc_ch[i];
		qraw[i] = adcs[whichAdc].get_data(whichAdcChannel);
	}
	tof = dragon::NO_DATA; /// \todo Calculate DSSSD tof
}

void dragon::DSSSD::calculate()
{
	/// \todo Implement DSSSD::calculate()
}


// ====== class dragon::DSSSD::Variables ====== //

dragon::DSSSD::Variables::Variables()
{
	std::fill_n(qdc_module, MAX_CHANNELS, 1);
	for(int i=0; i< MAX_CHANNELS; ++i) {
		qdc_ch[i] = i;
	}
	tof_ch = 1; /// \todo Update once plugged in

	std::fill_n(slope, MAX_CHANNELS, 1);
	std::fill_n(offset, MAX_CHANNELS, 0);
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
	database.ReadArray(pathAdcSlope, slope, MAX_CHANNELS);
	database.ReadArray(pathAdcSlope, offset, MAX_CHANNELS);
}
