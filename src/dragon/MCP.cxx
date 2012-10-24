/// \file MCP.cxx
/// \author G. Christian
/// \brief Implements MCP.hxx
#include <string>
#include <iostream>
#include "midas/Database.hxx"
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
	tac = dragon::NO_DATA;
	x = dragon::NO_DATA;
	y = dragon::NO_DATA;
	std::fill_n(anode, MAX_CHANNELS, dragon::NO_DATA);
}

void dragon::MCP::read_data(const vme::V785 adcs[], const vme::V1190& tdc)
{
	/*!
	 * \param modules Heavy-ion module structure
	 */
	for(int i=0; i< MAX_CHANNELS; ++i) {
		const int whichAdc = variables.anode_module[i];
		assert (whichAdc< Tail::NUM_ADC); ///\todo Don't use an assert here
		const int whichAdcChannel = variables.anode_ch[i];

		anode[i] = adcs[whichAdc].get_data(whichAdcChannel);
	}
//	tof = dragon::NO_DATA; /// \todo Calculate DSSSD tof
	
	const int whichAdc = variables.tac_module;
	assert (whichAdc< Tail::NUM_ADC); ///\todo Don't use an assert here
	const int whichAdcChannel = variables.tac_module;

	tac = adcs[whichAdc].get_data(whichAdcChannel);
}

void dragon::MCP::calculate()
{
  /*!
	 * Uses relative anode signal strengths to calculate spatial position
	 * of hits on the MCP. Position calculation algorithm taken from the MSc thesis of
	 * Michael Lamey, Simon Fraser University, 2001, p. 25, available online at
	 * <a href="http://dragon.triumf.ca/docs/Lamey_thesis.pdf">
	 * dragon.triumf.ca/docs/Lamey_thesis.pdf</a>
	 */
	if(!is_valid<int16_t>(anode, MAX_CHANNELS)) return;
	const double Lhalf = 25.;  // half the length of a single side of the MCP (50/2 [mm])
	int32_t sum = 0;
	for(int i=0; i< MAX_CHANNELS; ++i) sum += anode[i];
	if(sum) {
		x = Lhalf * ( (anode[1] + anode[2]) - (anode[0] + anode[3]) ) / (double)sum;
		y = Lhalf * ( (anode[0] + anode[1]) - (anode[2] + anode[3]) ) / (double)sum;
	}
}

// ====== class dragon::MCP::Variables ====== //

dragon::MCP::Variables::Variables()
{
	for(int i=0; i< MAX_CHANNELS; ++i) {
		anode_module[i] = 1;
		anode_ch[i] = i;
	}
	tac_module = 1;
	tac_ch = MAX_CHANNELS;
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

	midas::Database database(odb);
	database.ReadArray(pathAnodeModule, anode_module, MAX_CHANNELS);
	database.ReadArray(pathAnodeCh, anode_ch, MAX_CHANNELS);
	database.ReadValue(pathTacModule, tac_module);
	database.ReadValue(pathTacCh, tac_ch);
}

