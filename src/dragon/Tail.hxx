/// \file Tail.hxx
/// \author G. Christian
/// \brief Defines the DRAGON tail (heavy ion) detector classes
#ifndef DRAGON_TAIL_HXX
#define DRAGON_TAIL_HXX
#include "utils/VariableStructs.hxx"
#include "midas/Event.hxx"
#include "vme/IO32.hxx"
#include "vme/V792.hxx"
#include "vme/V1190.hxx"
#include "MCP.hxx"
#include "DSSSD.hxx"
#include "Auxillary.hxx"
#include "IonChamber.hxx"
#include "SurfaceBarrier.hxx"

namespace dragon {

/// Collection of all tail detectors and VME modules
class Tail {
	// Class global constants //
public:
	/// Number of ADC (caen v785) modules
	static const int NUM_ADC = 2;

	/// Midas event header
	midas::Event::Header header; //#

	// Subclasses //
public:
	/// Times-of-flight measured by the tail adc
	class Tof {
 public:
		// Subclasses //
		class Variables {
	 private:
			/** @cond */
	 PRIVATE:
			/** @endcond */
			/// Crossover TDC channel variables
			utils::TdcVariables<1> xtdc;

			// Methods //
	 public:
			/// Sets data to defaults
			Variables();
			/// Sets data to defaults
			void reset();
			/// Sets data from ODB
			void set(const char* odb);

			/// Give Tof class internal access
			friend class dragon::Tail::Tof;
		};
		
		/// Variables instance
		Variables variables; //!

		// Class data //
 private:
		/// "Parent" instance of dragon::Tail
		const dragon::Tail* fParent; //!

		/** @cond */
 PRIVATE:
		/** @endcond */
		/// Crossover tcal value [head trigger]
		double tcalx;
		/// Gamma -> MCP0
		double gamma_mcp;
		/// MCP0 -> MCP1
		double mcp;
#ifndef DRAGON_OMIT_DSSSD
		/// Gamma -> DSSSD
		double gamma_dsssd;
		/// MCP0 -> DSSSD
		double mcp_dsssd;
#endif
#ifndef DRAGON_OMIT_IC
		/// Gamma -> Ion-chamber
		double gamma_ic;
		/// MCP0 -> Ion-chamber
		double mcp_ic;
#endif

		// Class methods //
 public:
		/// Sets data to defaults
		Tof(const dragon::Tail* parent);
		/// Sets data to dragon::NO_DATA
		void reset();
		/// Reads data from raw VME modules
		void read_data(const vme::V785[], const vme::V1190& v1190);
		/// Performs calibration of xover tdc and TOF calculations
		void calculate();
	};

	// Class data //
private:
	/** @cond */
PRIVATE:
	/** @endcond */
	/// Electronics modules
#ifdef DISPLAY_MODULES
	/// IO32 FPGA
	vme::IO32 io32;          //#

	/// CAEN v785 ADC (x2)
	vme::V785 v785[NUM_ADC]; //#

	/// CAEN v1190 TDC
	vme::V1190 v1190;        //#
#else
	/// IO32 FPGA
	vme::IO32 io32;          //!

	/// CAEN v785 ADC (x2)
	vme::V785 v785[NUM_ADC]; //!

	/// CAEN v1190 TDC
	vme::V1190 v1190;        //!
#endif

#ifndef DRAGON_OMIT_DSSSD
	/// DSSSD detector
	DSSSD dsssd;             //
#endif

#ifndef DRAGON_OMIT_IC
	/// Ionization chamber
	IonChamber ic;           //
#endif

	/// MCPs
	MCP mcp;                 //

	/// Surface barrier detectors
	SurfaceBarrier sb;       //

#ifndef DRAGON_OMIT_NAI
	/// NaI detetors
	NaI nai;                 //
#endif

#ifndef DRAGON_OMIT_GE
	/// Germanium detector
	Ge ge;                   //
#endif

	/// Tail TOF
	Tail::Tof tof;              //

	// Methods //
public:
	/// Initializes data values
	Tail();

	/// Sets all data values to vme::NONE or other defaults
	void reset();

	/// Reads all variable values from an ODB (file or online)
	void set_variables(const char* odb_file);

	/// Unpack raw data into VME modules
	void unpack(const midas::Event& event);

	/// Calculate higher-level data for each detector, or across detectors
	void calculate();

	/// Allow 'Tof' class access to internal data
	friend class dragon::Tail::Tof;
};

} // namespace dragon



#endif
