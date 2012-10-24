/// \file DSSSD.cxx
/// \author G. Christian
/// \brief Implements DSSSD.hxx
#include <string>
#include <cassert>
#include <iostream>
#include "midas/Database.hxx"
#include "utils/Valid.hxx"
#include "vme/Functions.hxx"
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
	reset_array(MAX_CHANNELS, ecal);
}

void dragon::DSSSD::read_data(const vme::V785 adcs[], const vme::V1190& tdc)
{
	/*!
	 * Copies adc data into \c this->ecal[] with channel and module mapping taken
	 * from variables.adc_channel and variables.adc_modules
	 *  
	 * Delegates work to vme::channel_map()
	 * \param [in] adcs Array of vme::V785 adc modules from which data can be taken
	 * \param [in] tdc vme::V1190 tdc module from which data can be read
	 */
	vme::channel_map(ecal, MAX_CHANNELS, variables.adc_channel, variables.adc_module, adcs);
	vme::channel_map(tcal, variables.tdc_channel, tdc);
}

void dragon::DSSSD::calculate()
{
	/*!
	 * Does a linear transformation on each element in \c this->ecal[] using the slopes and offsets
	 * from variables.adc_slope and variables.adc_offset, respectively.
	 *
	 * Delegates the work to vme::transform()
	 */
	vme::transform(ecal, MAX_CHANNELS, vme::LinearCalibrate(variables.adc_offset, variables.adc_slope));
	vme::transform(tcal, vme::LinearCalibrate(variables.tdc_offset, variables.tdc_slope));
}


// ====== class dragon::DSSSD::Variables ====== //

dragon::DSSSD::Variables::Variables()
{
	/*! Calls reset() */
	reset();
}

void dragon::DSSSD::Variables::reset()
{
	std::fill_n(adc_module, MAX_CHANNELS, 1);
	vme::index_fill_n(adc_channel, MAX_CHANNELS);


	std::fill_n(adc_slope,  MAX_CHANNELS, 1.);
	std::fill_n(adc_offset, MAX_CHANNELS, 0.);

	tdc_channel = 1; /// \todo Update once plugged in
	tdc_slope   = 1.;
	tdc_offset  = 0.;
}

void dragon::DSSSD::Variables::set(const char* odb)
{
	/*!
	 * \param [in] odb_file Path of the odb file from which you are extracting variable values
	 * \todo Needs testing
	 */
	const char* const pathAdcModule = "/dragon/dsssd/variables/adc/module";
	const char* const pathAdcCh     = "/dragon/dsssd/variables/adc/channel";
	const char* const pathAdcSlope  = "/dragon/dsssd/variables/adc/slope";
	const char* const pathAdcOffset = "/dragon/dsssd/variables/adc/offset";

	const char* const pathTdcCh     = "/dragon/dsssd/variables/tdc/channel";
	const char* const pathTdcSlope  = "/dragon/dsssd/variables/tdc/slope";
	const char* const pathTdcOffset = "/dragon/dsssd/variables/tdc/offset";

	midas::Database database(odb);

	database.ReadArray(pathAdcModule, adc_module,  MAX_CHANNELS);
	database.ReadArray(pathAdcCh,     adc_channel, MAX_CHANNELS);
	database.ReadArray(pathAdcSlope,  adc_slope,   MAX_CHANNELS);
	database.ReadArray(pathAdcOffset, adc_offset,  MAX_CHANNELS);

	database.ReadValue(pathTdcCh, tdc_channel);
	database.ReadValue(pathTdcSlope, tdc_slope);
	database.ReadValue(pathTdcOffset, tdc_offset);
}
