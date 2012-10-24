/// \file MCP.hxx
/// \author G. Christian
/// \brief Defines a class for DRAGON Micro Channel Plate (MCP) detectors
#ifndef DRAGON_MCP_HXX
#define DRAGON_MCP_HXX
#include <stdint.h>

namespace vme {
class V792;
class V1190;
typedef V792 V785;
}


namespace dragon {

/// Micro channel plates
class MCP {

// Class global constants //
public:
	/// Number of anodes
	static const int MAX_CHANNELS = 4; //!

// Subclasses //
public:
	/// MCP Variables
	class Variables {
		// Class data //
 private:
		/** @cond */
 PRIVATE:
		/** @endcond */
		/// Maps anode channel to adc module number
		int anode_module[MAX_CHANNELS];

		/// Maps anode channel to adc channel number
		int anode_ch[MAX_CHANNELS];

		/// Maps TAC to adc module number
		int tac_module;

		/// Maps TAC to adc channel number
		int tac_ch;

		// Methods //
 public:
		/// Constructor, sets data to generic values
		Variables();

		/// Set variable values from an ODB file
		void set(const char* odb_file);

		/// Allow Mcp class access to internals
		friend class dragon::MCP;
	};

	/// Variables instance
	Variables variables; //!
	 
private:
	/** @cond */
PRIVATE:
	/** @endcond */
	/// Anode signals
	int16_t anode[MAX_CHANNELS]; //#
	 
	/// TAC signal (MCP_TOF).
	int16_t tac; //#

	/// x-position
	double x; //#

	/// y-position
	double y; //#

public:
	/// Constructor, initialize data
	MCP();

	/// Reset all data to VME::none
	void reset();
	 
	/// Read midas event data
	void read_data(const vme::V785 adcs[], const vme::V1190& tdc);

	/// Calculate x and y positions
	void calculate();
};

} // namespace dragon


#endif
