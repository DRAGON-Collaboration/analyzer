/// \file Tof.hxx
/// \author G. Christian
/// \brief Defines classes for holding time-of-flight parameters
#ifndef DRAGON_TOF_HXX
#define DRAGON_TOF_HXX
#include <string>
#include "utils/VariableStructs.hxx"


namespace utils { class Tof; }

namespace dragon {

class MCP;
class DSSSD;
class IonChamber;
class Bgo;

/// "Crossover" tof class
/*!
 * Holds the tcal information from the other system's trigger signal.
 */
class Xtof {
	// Subclasses //
public:
	class Variables {
	// Class data //
private:
		// Base odb path
		std::string fOdbBase; //!

		/** @cond */
 PRIVATE:
		/** @endcond */

		/// Tdc variables
		utils::TdcVariables<1> tdc;

		// Methods //
 public:
		/// Set to defaults, sets odb base path
		Variables(const char* odbBase);

		/// Set to defaults
		void reset();

		/// Set from ODB
		void set(const char* odb);

		/// Allow Xtof class access to internals
		friend class dragon::Xtof;
	};

	/// Variables instance
	Variables variables; //!
	
	// Class data //
private:
	/** @cond */
PRIVATE:
	/** @endcond */
	/// Calibrated cross-trigger time signal
	double tcal; //#

	// Class methods //
public:
	/// Initializes data, sets base ODB path
	Xtof(const char* odbBase);
	
	/// Resets data to dragon::NO_DATA
	void reset();

	/// Reads TDC data
	void read_data(const vme::V1190& tdc);

	/// Calibrates TOF
	void calculate();	

	/// Allow Tof calculator access to internals
	friend class utils::Tof;
};
		
/// "Tail" TOF class
/*!
 * Holds tof between the various Tail detectors and also the crossover TOF as calculated
 * by the tail's TDC.
 */
class TofTail {
	// Class data //
private:
	/** @cond */
	PRIVATE:
	/** @endcond */

	/// Gamma -> MCP0
	double gamma_mcp;

#ifndef DRAGON_OMIT_DSSSD
	/// Gamma -> DSSSD
	double gamma_dsssd;
#endif

#ifndef DRAGON_OMIT_IC
	/// Gamma -> Ion-chamber
	double gamma_ic;
#endif

	/// MCP0 -> MCP1
	double mcp;

#ifndef DRAGON_OMIT_DSSSD
	/// MCP0 -> DSSSD
	double mcp_dsssd;
#endif

#ifndef DRAGON_OMIT_IC
	/// MCP0 -> Ion-chamber
	double mcp_ic;
#endif

	// Class methods //
public:
	/// Sets all data to defaults
	TofTail();

	/// Reset all data to dragon::NO_DATA
	void reset();

	/// Calculate all of the TOF data
	void calculate(const dragon::MCP& mcp_, const dragon::DSSSD& dsssd_, const dragon::IonChamber& ic_, const dragon::Xtof& xover);
};



/// "Head" TOF class
/*!
 * Holds the crossover TOF as calculated head's TDC.
 */
class TofHead {
	// Class data //
private:
	/** @cond */
	PRIVATE:
	/** @endcond */

	/// Gamma -> Tail trigger
	double gamma_tail;

	// Class methods //
public:
	/// Sets all data to defaults
	TofHead();
	
	/// Reset all data to dragon::NO_DATA
	void reset();

	/// Calculate all the TOF data
	void calculate(const dragon::Bgo& bgo, const dragon::Xtof& xover);
};

} // namespace dragon


#endif // include guard
