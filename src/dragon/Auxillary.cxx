/// \file Auxillary.cxx
/// \author G. Christian
/// \brief Implements Auxillary.hxx
#include <string>
#include <iostream>
#include <algorithm>
#include "midas/Database.hxx"
#include "utils/Functions.hxx"
#include "vme/V1190.hxx"
#include "vme/V792.hxx"
#include "Channels.h"
#include "Auxillary.hxx"


// ====== class dragon::NaI ====== //

dragon::NaI::NaI()
{
	reset();
}

void dragon::NaI::reset()
{
	utils::reset_array(MAX_CHANNELS, ecal);
}

void dragon::NaI::read_data(const vme::V785 adcs[], const vme::V1190&)
{
	/*!
	 * Copies adc data into \c this->ecal[] with channel and module mapping taken
	 * from variables.adc.channel and variables.adc.modules
	 *
	 * Delegates work to utils::channel_map()
	 * \param [in] adcs Array of vme::V785 adc modules from which data can be taken
	 */
	utils::channel_map(ecal, MAX_CHANNELS, variables.adc.channel, variables.adc.module, adcs);
}

void dragon::NaI::calculate()
{
	utils::pedestal_subtract(ecal, MAX_CHANNELS, variables.adc);
	utils::linear_calibrate(ecal, MAX_CHANNELS, variables.adc);
}


// ====== class dragon::NaI::Variables ====== //

dragon::NaI::Variables::Variables()
{
	/*! Calls reset() */
	reset();
}

void dragon::NaI::Variables::reset()
{
	std::fill(adc.module, adc.module + MAX_CHANNELS, DEFAULT_HI_MODULE);
	utils::index_fill(adc.channel, adc.channel + MAX_CHANNELS, NAI_ADC0);
	std::fill(adc.pedestal, adc.pedestal + MAX_CHANNELS, 0);
	std::fill(adc.offset, adc.offset + MAX_CHANNELS, 0.);
	std::fill(adc.slope, adc.slope + MAX_CHANNELS, 1.);
}

void dragon::NaI::Variables::set(const char* odb)
{
	/*!
	 * \param [in] odb_file Path of the odb file from which you are extracting variable values
	 */
	midas::Database database(odb);
	if(database.IsZombie()) return;

	database.ReadArray("/dragon/nai/variables/adc/module",   adc.module,   MAX_CHANNELS);
	database.ReadArray("/dragon/nai/variables/adc/channel",  adc.channel,  MAX_CHANNELS);
	database.ReadArray("/dragon/nai/variables/adc/pedestal", adc.pedestal, MAX_CHANNELS);
	database.ReadArray("/dragon/nai/variables/adc/slope",    adc.slope,    MAX_CHANNELS);
	database.ReadArray("/dragon/nai/variables/adc/offset",   adc.offset,   MAX_CHANNELS);
}


// ====== class dragon::NaI ====== //

dragon::Ge::Ge()
{
	reset();
}

void dragon::Ge::reset()
{
	utils::reset_data(ecal);
}

void dragon::Ge::read_data(const vme::V785 adcs[], const vme::V1190&)
{
	/*!
	 * Copies adc data into \c this->ecal[] with channel and module mapping taken
	 * from variables.adc.channel and variables.adc.module
	 *
	 * Delegates work to utils::channel_map()
	 * \param [in] adcs Array of vme::V785 adc modules from which data can be taken
	 */
	utils::channel_map(ecal, variables.adc.channel, variables.adc.module, adcs);
}

void dragon::Ge::calculate()
{
	/*!
	 * Delegates work to functions in utils namespace.
	 */
	utils::pedestal_subtract(ecal, variables.adc);
	utils::linear_calibrate(ecal, variables.adc);
}


// ====== class dragon::Ge::Variables ====== //

dragon::Ge::Variables::Variables()
{
	/*! Calls reset() */
	reset();
}

void dragon::Ge::Variables::reset()
{
	adc.module = DEFAULT_HI_MODULE;
	adc.channel = GE_ADC0;
	adc.pedestal = 0;
	adc.offset = 0.;
	adc.slope = 1.;
}

void dragon::Ge::Variables::set(const char* odb)
{
	/*!
	 * \param [in] odb_file Path of the odb file from which you are extracting variable values
	 */
	midas::Database database(odb);

	database.ReadValue("/dragon/ge/variables/adc/module",   adc.module);
	database.ReadValue("/dragon/ge/variables/adc/channel",  adc.channel);
	database.ReadValue("/dragon/ge/variables/adc/pedestal", adc.pedestal);
	database.ReadValue("/dragon/ge/variables/adc/slope",    adc.slope);
	database.ReadValue("/dragon/ge/variables/adc/offset",   adc.offset);
}
