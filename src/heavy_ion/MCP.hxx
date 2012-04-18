/// \file MCP.hxx
/// \brief Defines a class for DRAGON Micro Channel Plate (MCP) detectors
#ifndef DRAGON_MCP_HXX
#define DRAGON_MCP_HXX
#include "Modules.hxx"

namespace dragon {

namespace hion {

/// Micro channel plates
struct MCP {
// ==== Const Data ==== //
	 /// Number of anodes
	 static const int nch = 4; //!

// ==== Classes ==== //
	 /// MCP Variables
	 struct Variables {
			/// Maps anode channel to adc module number
			int anode_module[MCP::nch];

			/// Maps anode channel to adc channel number
			int anode_ch[MCP::nch];

			/// Maps TAC to adc module number
			int tac_module;

			/// Maps TAC to adc channel number
			int tac_ch;

			/// Constructor, sets data to generic values
			Variables();

			/// Destructor, nothing to do
			~Variables() { }

			/// Copy constructor
			Variables(const Variables& other);

			/// Equivalency operator
			Variables& operator= (const Variables& other);

			/// \brief Set variable values from an ODB file
			/// \param [in] odb_file Path of the odb file from which you are extracting variable values
			/// \todo Needs to be implemented once ODB is set up
			void set(const char* odb_file);
	 };

// ==== Data ==== //
	 /// Variables instance
	 MCP::Variables variables; //!
	 
	 /// Anode signals
	 int16_t anode[nch]; //#
	 
	 /// TAC (time) signal
	 int16_t tac; //#

	 /// x-position
	 double x; //#

	 /// y-position
	 double y; //#

	 /// Constructor, initialize data
	 MCP();

	 /// Destructor, nothing to do
	 ~MCP() { }

	 /// Copy constructor
	 MCP(const MCP& other);

	 /// Equivalency operator
	 MCP& operator= (const MCP& other);

	 /// Reset all data to VME::none
	 void reset();
	 
	 /// \brief Read midas event data
	 /// \param modules Heavy-ion module structure
	 void read_data(const dragon::hion::Modules& modules);
};

} // namespace hion

} // namespace dragon


#endif
