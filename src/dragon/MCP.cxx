/// \file MCP.cxx
/// \author G. Christian
/// \brief Implements MCP.hxx
#include <string>
#include <iostream>
#include <algorithm>
#include "midas/Database.hxx"
#include "utils/Functions.hxx"
#include "vme/V1190.hxx"
#include "vme/V792.hxx"
#include "Channels.h"
#include "Tail.hxx"
#include "MCP.hxx"


// ====== class dragon::MCP ====== //

dragon::MCP::MCP()
{
	reset();
}

void dragon::MCP::reset()
{
	utils::reset_data(tac, x, y);
	utils::reset_array(MAX_CHANNELS, anode);
}

void dragon::MCP::read_data(const vme::V785 adcs[], const vme::V1190& tdc)
{
	/*!
	 * Copies ADC and TDC data into anode, tcal, and tac parameters.
	 *
	 * Delegates work to utils::channel_map()
	 * \param [in] adcs Array of vme::V785 adc modules from which data can be taken
	 * \param [in] tdc vme::V1190 tdc module from which data can be read
	 */
	utils::channel_map(anode, MAX_CHANNELS, variables.adc.channel, variables.adc.module, adcs);
	utils::channel_map(tcal, NUM_DETECTORS, variables.tdc.channel, tdc);
	utils::channel_map(tac, variables.tac_adc.channel, variables.tac_adc.module, adcs);
}

void dragon::MCP::calculate()
{
  /*!
	 * Pedestal-subtracts and calibrates anode, tcal, and tac values; calculates x- and
	 * y-positions.
	 *
	 * \note Position calculation algorithm taken from the MSc thesis of
	 * Michael Lamey, Simon Fraser University, 2001, p. 25; available online at
	 * <a href="http://dragon.triumf.ca/docs/Lamey_thesis.pdf">
	 * dragon.triumf.ca/docs/Lamey_thesis.pdf</a>
	 */
	utils::pedestal_subtract(anode, MAX_CHANNELS, variables.adc);
	utils::linear_calibrate(anode, MAX_CHANNELS, variables.adc);

	utils::linear_calibrate(tcal, NUM_DETECTORS, variables.tdc);

	utils::pedestal_subtract(tac, variables.tac_adc);
	utils::linear_calibrate(tac, variables.tac_adc);

	// Position calculation if we have all valid anode signals
	if(utils::is_valid(anode, MAX_CHANNELS)) {
		const double Lhalf = 25.;  // half the length of a single side of the MCP (50/2 [mm])
		double sum = 0;
		for(int i=0; i< MAX_CHANNELS; ++i) sum += anode[i];
		if(sum != 0) {
			x = Lhalf * ( (anode[1] + anode[2]) - (anode[0] + anode[3]) ) / sum;
			y = Lhalf * ( (anode[0] + anode[1]) - (anode[2] + anode[3]) ) / sum;
		}
	}
}


// ====== class dragon::MCP::Variables ====== //

dragon::MCP::Variables::Variables()
{
	/*! Calls reset() */
	reset();
}

void dragon::MCP::Variables::reset()
{
	std::fill(adc.module, adc.module + MAX_CHANNELS, DEFAULT_HI_MODULE);
	utils::index_fill(adc.channel, adc.channel + MAX_CHANNELS, MCP_ADC0);

	std::fill(adc.pedestal, adc.pedestal + MAX_CHANNELS, 0);
	std::fill(adc.offset, adc.offset + MAX_CHANNELS, 0.);
	std::fill(adc.slope, adc.slope + MAX_CHANNELS, 1.);

	tac_adc.module   = DEFAULT_HI_MODULE;
	tac_adc.channel  = MCP_TAC_ADC0;

	tac_adc.pedestal = 0;
	tac_adc.offset   = 0.;
	tac_adc.slope    = 1.;

	std::fill(tdc.module, tdc.module + NUM_DETECTORS, 0); // unused
	utils::index_fill(tdc.channel, tdc.channel + NUM_DETECTORS, MCP_TDC0);

	std::fill(tdc.offset, tdc.offset + NUM_DETECTORS, 0.);
	std::fill(tdc.slope, tdc.slope + NUM_DETECTORS, 1.);
}

void dragon::MCP::Variables::set(const char* odb)
{
	/*!
	 * \param [in] odb_file Path of the odb file from which you are extracting variable values
	 * \todo Test
	 */
	midas::Database database(odb);

	database.ReadArray("/dragon/mcp/variables/adc/channel",  adc.channel,  MAX_CHANNELS);
	database.ReadArray("/dragon/mcp/variables/adc/module",   adc.module,   MAX_CHANNELS);
	database.ReadArray("/dragon/mcp/variables/adc/pedestal", adc.pedestal, MAX_CHANNELS);
	database.ReadArray("/dragon/mcp/variables/adc/slope",    adc.slope,    MAX_CHANNELS);
	database.ReadArray("/dragon/mcp/variables/adc/offset",   adc.offset,   MAX_CHANNELS);

	database.ReadValue("/dragon/mcp/variables/tac_adc/channel",  tac_adc.channel);
	database.ReadValue("/dragon/mcp/variables/tac_adc/module",   tac_adc.module);
	database.ReadValue("/dragon/mcp/variables/tac_adc/pedestal", tac_adc.pedestal);
	database.ReadValue("/dragon/mcp/variables/tac_adc/slope",    tac_adc.slope);
	database.ReadValue("/dragon/mcp/variables/tac_adc/offset",   tac_adc.offset);
	
	database.ReadArray("/dragon/mcp/variables/tdc/channel", tdc.channel, NUM_DETECTORS);
	database.ReadArray("/dragon/mcp/variables/tdc/slope",   tdc.slope,   NUM_DETECTORS);
	database.ReadArray("/dragon/mcp/variables/tdc/offset",  tdc.offset,  NUM_DETECTORS);
}

