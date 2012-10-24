/// \file MCP.cxx
/// \author G. Christian
/// \brief Implements MCP.hxx
#include <string>
#include <iostream>
#include <algorithm>
#include "midas/Database.hxx"
#include "vme/Functions.hxx"
#include "vme/V1190.hxx"
#include "vme/V792.hxx"
#include "Tail.hxx"
#include "MCP.hxx"


// ====== class dragon::MCP ====== //

dragon::MCP::MCP()
{
	reset();
}

void dragon::MCP::reset()
{
	reset_data(tac, x, y);
	reset_array(MAX_CHANNELS, anode);
}

void dragon::MCP::read_data(const vme::V785 adcs[], const vme::V1190& tdc)
{
	/*!
	 * Copies ADC and TDC data into anode, tcal, and tac parameters.
	 *
	 * Delegates work to vme::channel_map()
	 * \param [in] adcs Array of vme::V785 adc modules from which data can be taken
	 * \param [in] tdc vme::V1190 tdc module from which data can be read
	 */
	vme::channel_map(anode, MAX_CHANNELS, variables.adc.channel, variables.adc.module, adcs);
	vme::channel_map(tcal, NUM_DETECTORS, variables.tdc.channel, tdc);
	vme::channel_map(tac, variables.tac_adc.channel, variables.tac_adc.module, adcs);
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
	vme::transform(anode, MAX_CHANNELS, vme::PedestalSubtract(variables.adc.pedestal));
	vme::transform(anode, MAX_CHANNELS, vme::LinearCalibrate(variables.adc.offset, variables.adc.slope));

	vme::transform(tcal, NUM_DETECTORS, vme::LinearCalibrate(variables.tdc.offset, variables.tdc.slope));
	
	vme::transform(tac, vme::PedestalSubtract(variables.tac_adc.pedestal));
	vme::transform(tac, vme::LinearCalibrate(variables.tac_adc.offset, variables.tac_adc.slope));

	// Position calculation if we have all valid anode signals
	if(is_valid(anode, MAX_CHANNELS)) {
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
	std::fill_n(adc.module, MAX_CHANNELS, 0);
	vme::index_fill_n(adc.channel, MAX_CHANNELS);
	std::fill_n(adc.pedestal, MAX_CHANNELS, 0);
	std::fill_n(adc.offset, MAX_CHANNELS, 0.);
	std::fill_n(adc.slope, MAX_CHANNELS, 1.);

	tac_adc.module   = 0;
	tac_adc.channel  = MAX_CHANNELS;
	tac_adc.pedestal = 0;
	tac_adc.offset   = 0.;
	tac_adc.slope    = 1.;

	std::fill_n(tdc.module, NUM_DETECTORS, 0);
	vme::index_fill_n(tdc.channel, NUM_DETECTORS);
	std::fill_n(tdc.offset, NUM_DETECTORS, 0.);
	std::fill_n(tdc.slope, NUM_DETECTORS, 1.);
}

void dragon::MCP::Variables::set(const char* odb)
{
	/*!
	 * \param [in] odb_file Path of the odb file from which you are extracting variable values
	 * \todo Needs to be implemented once ODB is set up
	 */
	/// \todo Set actual ODB paths, TEST!!
	const char* const pathAnodeModule = "Equipment/MCP/Variables/AnodeModule";
	const char* const pathAnodeCh     = "Equipment/MCP/Variables/AnodeChannel";
	const char* const pathTacCh       = "Equipment/MCP/Variables/TACChannel";
	const char* const pathTacModule   = "Equipment/MCP/Variables/TACModule";

	// midas::Database database(odb);
	// database.ReadArray(pathAnodeModule, anode_module, MAX_CHANNELS);
	// database.ReadArray(pathAnodeCh, anode_ch, MAX_CHANNELS);
	// database.ReadValue(pathTacModule, tac_module);
	// database.ReadValue(pathTacCh, tac_ch);
}

