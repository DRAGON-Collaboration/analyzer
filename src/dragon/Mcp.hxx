/// \file Mcp.hxx
/// \author G. Christian
/// \brief Defines a class for DRAGON Micro Channel Plate (MCP) detectors
#ifndef DRAGON_MCP_HXX
#define DRAGON_MCP_HXX
#include "utils/VariableStructs.hxx"


namespace vme {
class V792;
class V1190;
typedef V792 V785;
}

namespace dragon {

class Tail;

/// Micro channel plates
class Mcp {

// Class global constants //
public:
	/// Number of anodes on MCP0
	static const int MAX_CHANNELS = 4;  //!

	/// Number of separate mcp detctors
	static const int NUM_DETECTORS = 2; //!

// Subclasses //
public:
	/// MCP Variables
	class Variables {
		// Class data //
 private:
		/** @cond */
 PRIVATE:
		/** @endcond */

		/// Adc variables for the anode signals
		utils::AdcVariables<MAX_CHANNELS> adc;

		/// Adc variables for the TAC signal
		utils::AdcVariables<1> tac_adc;

		/// Tdc variables
		utils::TdcVariables<NUM_DETECTORS> tdc;

		// Methods //
 public:
		/// Constructor, sets data to generic values
		Variables();

		/// Restet variables to default values
		void reset();

		/// Set variable values from an ODB file
		void set(const char* odb_file);

		/// Allow Mcp class access to internals
		friend class dragon::Mcp;
	};

	/// Variables instance
	Variables variables; //!
	 
private:
	/** @cond */
PRIVATE:
	/** @endcond */
	/// Anode signals
	double anode[MAX_CHANNELS]; //#
	 
	/// TDC signals
	double tcal[NUM_DETECTORS]; //#

	/// Sum of anode signals
	double esum; //#

	/// TAC signal (MCP_TOF).
	double tac;  //#

	/// x-position
	double x;    //#

	/// y-position
	double y;    //#

public:
	/// Constructor, initialize data
	Mcp();

	/// Reset all data to VME::none
	void reset();
	 
	/// Read midas event data
	void read_data(const vme::V785 adcs[], const vme::V1190& tdc);

	/// Calibrate ADC/TDC signals, calculate x and y positions
	void calculate();

	/// Allow tof calculation access to internals
	friend class dragon::Tail;
};

} // namespace dragon


#ifdef __MAKECINT__
#pragma link C++ class utils::AdcVariables<dragon::Mcp::MAX_CHANNELS>;
#pragma link C++ class utils::TdcVariables<dragon::Mcp::NUM_DETECTORS>;
#endif

#endif // include guard
