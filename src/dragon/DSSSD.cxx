/// \file DSSSD.cxx
/// \author G. Christian
/// \brief Implements DSSSD.hxx
#include <string>
#include <cassert>
#include <iostream>
#include "midas/Database.hxx"
#include "utils/Functions.hxx"
#include "vme/V1190.hxx"
#include "vme/V792.hxx"
#include "Channels.h"
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
	utils::reset_array(MAX_CHANNELS, ecal);
}

void dragon::DSSSD::read_data(const vme::V785 adcs[], const vme::V1190& tdc)
{
	/*!
	 * Copies adc data into \c this->ecal[] with channel and module mapping taken
	 * from variables.adc.channel and variables.adc.modules
	 *  
	 * Delegates work to utils::channel_map()
	 * \param [in] adcs Array of vme::V785 adc modules from which data can be taken
	 * \param [in] tdc vme::V1190 tdc module from which data can be read
	 */
	utils::channel_map(ecal, MAX_CHANNELS, variables.adc.channel, variables.adc.module, adcs);
	utils::channel_map(tcal, variables.tdc.channel, tdc);
}

void dragon::DSSSD::calculate()
{
	/*!
	 * Does a linear transformation on each element in \c this->ecal[] using the slopes and offsets
	 * from variables.adc_slope and variables.adc_offset, respectively. Also calibrates the TDC
	 * signal.
	 *
	 * Delegates the work to utils::pedestal_subtract and utils::linear_calibrate
	 */
	utils::pedestal_subtract(ecal, MAX_CHANNELS, variables.adc);
	utils::linear_calibrate(ecal, MAX_CHANNELS, variables.adc);

	utils::linear_calibrate(tcal, variables.tdc);
}


// ====== class dragon::DSSSD::Variables ====== //

dragon::DSSSD::Variables::Variables()
{
	/*! Calls reset() */
	reset();
}

void dragon::DSSSD::Variables::reset()
{
	std::fill(adc.module, adc.module + MAX_CHANNELS, DSSSD_MODULE);
	utils::index_fill(adc.channel, adc.channel + MAX_CHANNELS, DSSSD_ADC0);

	std::fill(adc.pedestal, adc.pedestal + MAX_CHANNELS, 0);
	std::fill(adc.slope,  adc.slope + MAX_CHANNELS, 1.);
	std::fill(adc.offset, adc.offset + MAX_CHANNELS, 0.);

	tdc.module  = 0; // unused
	tdc.channel = DSSSD_TDC0;
	tdc.slope   = 1.;
	tdc.offset  = 0.;
}

void dragon::DSSSD::Variables::set(const char* odb)
{
	/*!
	 * \param [in] odb_file Path of the odb file from which you are extracting variable values
	 * \todo Needs testing
	 */
	midas::Database database(odb);

	database.ReadArray("/dragon/dsssd/variables/adc/module",  adc.module,  MAX_CHANNELS);
	database.ReadArray("/dragon/dsssd/variables/adc/channel", adc.channel, MAX_CHANNELS);
	database.ReadArray("/dragon/dsssd/variables/adc/slope",   adc.slope,   MAX_CHANNELS);
	database.ReadArray("/dragon/dsssd/variables/adc/offset",  adc.offset,  MAX_CHANNELS);

	database.ReadValue("/dragon/dsssd/variables/tdc/channel", tdc.channel);
	database.ReadValue("/dragon/dsssd/variables/tdc/slope",   tdc.slope);
	database.ReadValue("/dragon/dsssd/variables/tdc/offset",  tdc.offset);
}
