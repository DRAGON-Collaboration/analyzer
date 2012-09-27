/// \file MCP.hxx
/// \author G. Christian
/// \brief Defines a class for DRAGON Micro Channel Plate (MCP) detectors
#ifndef DRAGON_MCP_HXX
#define DRAGON_MCP_HXX
#include <stdint.h>

namespace vme {
class V785;
class V1190;
}


namespace dragon {

/// Micro channel plates
class MCP {

public:
	/// Number of anodes
	static const int MAX_CHANNELS = 4; //!

public:
	/// MCP Variables
	struct Variables {
		/// Maps anode channel to adc module number
		int anode_module[MAX_CHANNELS];

		/// Maps anode channel to adc channel number
		int anode_ch[MAX_CHANNELS];

		/// Maps TAC to adc module number
		int tac_module;

		/// Maps TAC to adc channel number
		int tac_ch;

		/// Constructor, sets data to generic values
		Variables();

		/// \brief Set variable values from an ODB file
		/// \param [in] odb_file Path of the odb file from which you are extracting variable values
		/// \todo Needs to be implemented once ODB is set up
		void set(const char* odb_file);
	};

// ==== Data ==== //
	/// Variables instance
	Variables variables; //!
	 
	/// Anode signals
	int16_t anode[MAX_CHANNELS]; //#
	 
	/// TAC (time) signal
	int16_t tac; //#

	/// x-position
	double x; //#

	/// y-position
	double y; //#

	/// Constructor, initialize data
	MCP();

	/// Reset all data to VME::none
	void reset();
	 
	/// \brief Read midas event data
	/// \param modules Heavy-ion module structure
	void read_data(const vme::V785 adcs[], const vme::V1190& tdc);

	/// \brief Calculate x and y positions
	/// \details Uses relative anode signal strengths to calculate spatial position
	/// of hits on the MCP. Position calculation algorithm taken from the MSc thesis of
	/// Michael Lamey, Simon Fraser University, 2001, p. 25, available online at
	/// <a href="http://dragon.triumf.ca/docs/Lamey_thesis.pdf">
	/// dragon.triumf.ca/docs/Lamey_thesis.pdf</a>
	void calculate();
};

} // namespace dragon


#endif
