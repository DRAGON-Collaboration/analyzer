/// \file SurfaceBarrier.cxx
/// \author G. Christian
/// \brief Implements SurfaceBarrier.hxx
#include <string>
#include <iostream>
#include "midas/Database.hxx"
#include "utils/Functions.hxx"
#include "vme/V1190.hxx"
#include "vme/V792.hxx"
#include "Channels.h"
#include "SurfaceBarrier.hxx"


// ====== struct dragon::SurfaceBarrier ====== //

dragon::SurfaceBarrier::SurfaceBarrier()
{
	reset();
}

void dragon::SurfaceBarrier::reset()
{
	utils::reset_array(MAX_CHANNELS, ecal);
}

void dragon::SurfaceBarrier::read_data(const vme::V785 adcs[], const vme::V1190&)
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

void dragon::SurfaceBarrier::calculate()
{
  /*!
	 * Performs pedestal subtraction & calibration of energies.
	 *
	 * Delegates work to utils::pedestal_aubtract() and utils::linear_calibrate()
	 */
	utils::pedestal_subtract(ecal, MAX_CHANNELS, variables.adc);
	utils::linear_calibrate(ecal, MAX_CHANNELS, variables.adc);
}


// ====== class dragon::SurfaceBarrier::Variables ====== //

dragon::SurfaceBarrier::Variables::Variables()
{
	/*! Calls reset() */
	reset();
}

void dragon::SurfaceBarrier::Variables::reset()
{
	std::fill(adc.module, adc.module + MAX_CHANNELS, DEFAULT_HI_MODULE);
	utils::index_fill(adc.channel, adc.channel + MAX_CHANNELS, SB_ADC0);
	
	std::fill(adc.pedestal, adc.pedestal + MAX_CHANNELS, 0);
	std::fill(adc.offset, adc.offset + MAX_CHANNELS, 0.);
	std::fill(adc.slope, adc.slope + MAX_CHANNELS, 1.);
}

void dragon::SurfaceBarrier::Variables::set(const char* odb)
{
	/*!
	 * \param [in] odb_file Path of the odb file from which you are extracting variable values
	 */
	midas::Database database(odb);
	if(database.IsZombie()) return;

	database.ReadArray("/dragon/sb/variables/adc/module",   adc.module,   MAX_CHANNELS);
	database.ReadArray("/dragon/sb/variables/adc/channel",  adc.channel,  MAX_CHANNELS);
	database.ReadArray("/dragon/sb/variables/adc/pedestal", adc.pedestal, MAX_CHANNELS);
	database.ReadArray("/dragon/sb/variables/adc/slope",    adc.slope,    MAX_CHANNELS);
	database.ReadArray("/dragon/sb/variables/adc/offset",   adc.offset,   MAX_CHANNELS);
}
