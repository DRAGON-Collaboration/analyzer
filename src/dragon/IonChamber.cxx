/// \file IonChamber.cxx
/// \author G. Christian
/// \brief Implements IonChamber.hxx
#include <string>
#include <iostream>
#include "midas/Database.hxx"
#include "utils/Functions.hxx"
#include "vme/V1190.hxx"
#include "vme/V792.hxx"
#include "Channels.h"
#include "Tail.hxx"
#include "IonChamber.hxx"


// ====== struct dragon::IonChamber ====== //

dragon::IonChamber::IonChamber()
{
	reset();
}

void dragon::IonChamber::reset()
{
	utils::reset_array(MAX_CHANNELS, anode);
	utils::reset_data(tcal, sum);
}

void dragon::IonChamber::read_data(const vme::V785 adcs[], const vme::V1190& tdc)
{
	/*!
	 * \param modules Heavy-ion module structure
	 * \param [in] v1190_trigger_ch Channel number of the v1190b trigger
	 */
	utils::channel_map(anode, MAX_CHANNELS, variables.adc.channel, variables.adc.module, adcs);
	utils::channel_map(tcal, variables.tdc.channel, tdc);
}

void dragon::IonChamber::calculate()
{
	/*!
	 * Calibrates anode and time signals, calculates anode sum
	 */
	utils::pedestal_subtract(anode, MAX_CHANNELS, variables.adc);
	utils::linear_calibrate(anode, MAX_CHANNELS, variables.adc);

	utils::linear_calibrate(tcal, variables.tdc);

	if(utils::is_valid(anode, MAX_CHANNELS)) {
		sum = utils::calculate_sum(anode, anode + MAX_CHANNELS);
	}
}


// ====== struct dragon::IonChamber::Variables ====== //

dragon::IonChamber::Variables::Variables()
{
	/*! Calls reset() */
	reset();
}

void dragon::IonChamber::Variables::reset()
{
	std::fill(adc.module, adc.module + MAX_CHANNELS, DEFAULT_HI_MODULE);
	utils::index_fill(adc.channel, adc.channel + MAX_CHANNELS, IC_ADC0);

	std::fill(adc.pedestal, adc.pedestal + MAX_CHANNELS, 0);
	std::fill(adc.slope, adc.slope + MAX_CHANNELS, 1.);
	std::fill(adc.offset, adc.offset + MAX_CHANNELS, 0.);

	tdc.module = 0; // unused
	tdc.channel = IC_TDC0;

	tdc.offset = 0.;
	tdc.slope = 1.;
}

void dragon::IonChamber::Variables::set(const char* odb)
{
	/*!
	 * \param [in] odb_file Path of the odb file from which you are extracting variable values
	 */
	midas::Database database(odb);
	if(database.IsZombie()) return;

	database.ReadArray("/dragon/ic/variables/adc/module",   adc.module,   MAX_CHANNELS);
	database.ReadArray("/dragon/ic/variables/adc/channel",  adc.channel,  MAX_CHANNELS);
	database.ReadArray("/dragon/ic/variables/adc/pedestal", adc.pedestal, MAX_CHANNELS);
	database.ReadArray("/dragon/ic/variables/adc/slope",    adc.slope,    MAX_CHANNELS);
	database.ReadArray("/dragon/ic/variables/adc/offset",   adc.offset,   MAX_CHANNELS);

	database.ReadValue("/dragon/ic/variables/tdc/channel",  tdc.channel);
	database.ReadValue("/dragon/ic/variables/tdc/slope",    tdc.slope);
	database.ReadValue("/dragon/ic/variables/tdc/offset",   tdc.offset);
}
