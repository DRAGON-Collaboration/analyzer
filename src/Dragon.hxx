///
/// \file Dragon.hxx
/// \author G. Christian,
/// \brief Defines dragon detector classes
/// \attention The '//!' or '//#' comments following variable declarations are not
///  optional! They are actually used in processing of the file with ROOTCINT.
///
#ifndef HAVE_DRAGON_HXX
#define HAVE_DRAGON_HXX
#include "utils/IntTypes.h"
#include <string>
#include <sstream>
#include "utils/VariableStructs.hxx"
#include "midas/Event.hxx"
#include "Vme.hxx"

#ifndef USE_ROOT
typedef const char Option_t;
#endif


// Forward declare midas coincidence event //
namespace midas { struct CoincEvent; class Database; }

// Forward declare vme classes //
namespace vme {
	class V792;
	class V1190;
	typedef V792 V785;
}

///
/// Encloses dragon detector classes
///
namespace dragon {

	class Tail; // forward declaration

	// ======= Class definitions ======== //
	///
	/// Global run parameters
	///
	class RunParameters {
	public: // Constants
		static const int MAX_FRONTENDS = 2; //!
	public: // Methods
		/// Constructor, calls reset()
		RunParameters();
		/// Sets all data to defaults
		void reset();
		/// Reads data from the ODB or a midas file
		bool read_data(const midas::Database* db);

	public: // Data
		/// Run start time from the tsc (in seconds)
		/*! [0]: head, [1]: tail */
		double run_start[MAX_FRONTENDS];
		/// Run stop time from the tsc (in seconds)
		/*! [0]: head, [1]: tail */
		double run_stop[MAX_FRONTENDS];
		/// Trigger start time from the tsc (in seconds)
		/*! [0]: head, [1]: tail */
		double trigger_start[MAX_FRONTENDS];
		/// Trigger stop time from the tsc (in seconds)
		/*! [0]: head, [1]: tail */
		double trigger_stop[MAX_FRONTENDS];
	};

	///
	/// The BGO array
	///
	class Bgo {
	public: // Constants
		/// Number of channels in the BGO array
		static const int MAX_CHANNELS = 30; //!

	public: // Methods
		/// Constructor, initializes data values
		Bgo();
		/// Sets all data values to dragon::NO_DATA
		void reset();
		/// Read adc & tdc data
		void read_data(const vme::V792& adc, const vme::V1190& tdc);
		/// Do higher-level parameter calculations
		void calculate();

	public: // Data
		/// Calibrated energies
		double ecal[MAX_CHANNELS];    //#
		/// Calibrates times
		double tcal[MAX_CHANNELS];    //#
		/// Sorted (high->low) energies
		double esort[MAX_CHANNELS];   //#
		/// Sum of all \e valid energies
		double sum; //#
		/// Which detector was the highest energy hit
		int hit0;  //#
		/// x position of the highest energy hit
		double x0; //#
		/// y position of the highest energy hit
		double y0; //#
		/// z position of the highest energy hit
		double z0; //#
		/// time of the highest energy hit
		double t0; //#

	public: // Subclasses
		///
		/// Bgo variables
		///
		class Variables {
		public: // Methods
			/// Sets data to defaults
			Variables();
			/// Set values to defaults
			void reset();
			///  Set data values from an database (file or online)
			bool set(const char* dbfile);
			///  Set data values from a constructed database
			bool set(const midas::Database* db);

		public: // Data
			/// Adc variables
			dragon::utils::AdcVariables<MAX_CHANNELS> adc;
			/// Tdc variables
			dragon::utils::TdcVariables<MAX_CHANNELS> tdc;
			/// Detector positions in space
			dragon::utils::PositionVariables<MAX_CHANNELS> pos;
		};

	public: // Subclass instances
		/// Instance of Bgo::Variables
		Bgo::Variables variables;   //!
	};

	///
	/// Double-Sided Silicon Strip Detector
	///
	class Dsssd {
	public: // Constants
		/// Number of channels
		static const int MAX_CHANNELS = 32; //!

	public: // Methods
		/// Constructor, initialize data
		Dsssd();
		/// Reset all data to VME::none
		void reset();
		/// Read data from vme modules
		void read_data(const vme::V785 adcs[], const vme::V1190& tdc);
		/// Performs energy and time calibrations
		void calculate();

	public:
		/// Calibrated energy signals
		double ecal[MAX_CHANNELS]; //#
		/// Highest energy signal in the front strips (0 - 15)
		double efront;      //#
		/// Highest energy signal in the back strips (16 - 31)
		double eback;       //#
		/// Which strip was hit in the front strips
		uint32_t hit_front; //#
		/// Which strip was hit in the back strips
		uint32_t hit_back;  //#
		/// Calibrated time signal from the front strips
		double tfront;      //#
		/// Calibrated time signal from the back strips
		double tback;       //#

	public: // Subclasses
		///
		/// Dsssd Variables Class
		///
		class Variables {
		public: // Methods
			/// Constructor, sets data to generic values
			Variables();
			///  Set data values from an database (file or online)
			bool set(const char* dbfile);
			///  Set data values from a constructed database
			bool set(const midas::Database* db);
			/// Reset all variable values to defaults
			void reset();

		public: // Data
			/// Adc variables for the energy signals
			dragon::utils::AdcVariables<32> adc;
			/// Tdc variables (front strips)
			dragon::utils::TdcVariables<1> tdc_front;
			/// Tdc variables (back strips)
			dragon::utils::TdcVariables<1> tdc_back;
		};

	public: // Subclass instances
		/// Instance of Dsssd::Variables
		Dsssd::Variables variables; //!
	};

	///
	/// Ionization chamber
	///
	class IonChamber {
	public: // Constants
		/// Number of anodes
		static const int MAX_CHANNELS = 5; //!
		/// Number of time signals
		static const int MAX_TDC = 4; //!

	public: // Methods
		/// Constructor, initialize data
		IonChamber();
		/// Reset all data to VME::none
		void reset();
		/// Read midas event data
		void read_data(const vme::V785 adcs[], const vme::V1190& tdc);
		/// Calculate higher-level parameters
		void calculate();

	public: // Data
		/// Calibrated anode signals
		double anode[MAX_CHANNELS]; //#
		/// Time signals
		double tcal[MAX_TDC]; //#
		/// Sum of anode signals
		double sum;  //#

	public: // Subclasses
		///
		/// Ion chamber variables
		///
		class Variables {
		public: // Methods
			/// Constructor, sets data to generic values
			Variables();
			/// Reset data to default values
			void reset();
			///  Set data values from an database (file or online)
			bool set(const char* dbfile);
			///  Set data values from a constructed database
			bool set(const midas::Database* db);

		public: // Data
			/// Anode variables
			dragon::utils::AdcVariables<MAX_CHANNELS> adc;
			/// Tdc variables
			dragon::utils::TdcVariables<MAX_TDC> tdc;
		};

	public: // Subclass instances
		/// Variables instance
		IonChamber::Variables variables; //!
	};

	///
	/// Micro channel plates
	///
	class Mcp {
	public: // Constants
		/// Number of anodes on MCP0
		static const int MAX_CHANNELS = 4;  //!
		/// Number of separate mcp detctors
		static const int NUM_DETECTORS = 2; //!

	public: // Methods
		/// Constructor, initialize data
		Mcp();
		/// Reset all data to VME::none
		void reset();
		/// Read midas event data
		void read_data(const vme::V785 adcs[], const vme::V1190& tdc);
		/// Calibrate ADC/TDC signals, calculate x and y positions
		void calculate();

	public: // Data
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

	public: // Subclasses
		///
		/// MCP Variables
		///
		class Variables {
		public: // Methods
			/// Constructor, sets data to generic values
			Variables();
			/// Restet variables to default values
			void reset();
			///  Set data values from an database (file or online)
			bool set(const char* dbfile);
			///  Set data values from a constructed database
			bool set(const midas::Database* db);

		public: // Data
			/// Adc variables for the anode signals
			dragon::utils::AdcVariables<MAX_CHANNELS> adc;
			/// Adc variables for the TAC signal
			dragon::utils::AdcVariables<1> tac_adc;
			/// Tdc variables
			dragon::utils::TdcVariables<NUM_DETECTORS> tdc;
		};

	public: // Subclass instances
		/// Variables instance
		Mcp::Variables variables; //!
	};

	///
	/// Surface barrier detectors
	///
	class SurfaceBarrier {
	public: // Constants
		/// Number of detectors
		static const int MAX_CHANNELS = 2; //!

	public: // Subclasses
		///
		/// Surface Barrier Variables
		///
		class Variables {
		public: // Methods
			/// Constructor, sets data to generic values
			Variables();
			/// Sets data to defaults
			void reset();
			///  Set data values from an database (file or online)
			bool set(const char* dbfile);
			///  Set data values from a constructed database
			bool set(const midas::Database* db);

		public: // Data
			/// Adc variables
			dragon::utils::AdcVariables<MAX_CHANNELS> adc;
		};

	public: // Methods
		/// Constructor, initialize data
		SurfaceBarrier();
		/// Reset all data to vme::NONE
		void reset();
		/// Read midas event data
		void read_data(const vme::V785 adcs[], const vme::V1190&);
		/// Final energy calculation
		void calculate();

	public: // Data
		/// Energy signals
		double ecal[MAX_CHANNELS]; //#
		/// Variables instance
		SurfaceBarrier::Variables variables; //!
	};

	///
	/// Sodium Iodide (NaI) detectors
	///
	class NaI {
	public: // Constants
		/// Number of detectors
		static const int MAX_CHANNELS = 2; //!

	public: // Methods
		/// Constructor, initialize data
		NaI();
		/// Reset all data to vme::NONE
		void reset();
		/// Read event data from vme modules
		void read_data(const vme::V785 adcs[], const vme::V1190&);
		/// Do energy calibrations
		void calculate();

	public: // Data
		/// Calibrated energy signals
		double ecal[MAX_CHANNELS]; //#

	public: // Subclasses
		///
		/// NaI variables
		///
		class Variables {
		public: // Methods
			/// Constructor, sets data to generic values
			Variables();
			/// Set variable values to defaults
			void reset();
			///  Set data values from an database (file or online)
			bool set(const char* dbfile);
			///  Set data values from a constructed database
			bool set(const midas::Database* db);

		public: // Data
			/// Adc variables
			dragon::utils::AdcVariables<MAX_CHANNELS> adc;
		};

	public: // Subclass instances
		/// Variables instance
		NaI::Variables variables; //!
	};

	///
	/// Germanium (Ge) detector
	///
	class Ge {
	public: // Methods
		/// Constructor, initialize data
		Ge();
		/// Reset all data to vme::NONE
		void reset();
		/// Read event data from the ADC
		void read_data(const vme::V785 adcs[], const vme::V1190&);
		/// Do energy calibrations, pedestal subtractions
		void calculate();

	public: // Data
		/// Calibrated energy signal
		double  ecal; //#

	public: // Subclasses
		///
		/// Ge variables
		///
		class Variables {
		public: // Methods
			/// Constructor, sets data to generic values
			Variables();
			/// Reset variables to default values
			void reset();
			///  Set data values from an database (file or online)
			bool set(const char* dbfile);
			///  Set data values from a constructed database
			bool set(const midas::Database* db);

		public: // Data
			/// ADC variables
			dragon::utils::AdcVariables<1> adc;
		};

	public: // Subclass instances
		/// Variables instance
		Ge::Variables variables; //!
	};

	/// Class for tdc stuff
	/*!
	 *  Stores values of multiple leading and trailing edge hits
	 *  \tparam MAX_HITS Maximum number of hits desired for storage
	 */
	template <int MAX_HITS>
	class TdcChannel {
	public: // Methods
		/// Sets data to defaults
		TdcChannel();
		/// Sets data to dragon::NO_DATA
		void reset();
		/// Reads data from TDC module
		void read_data(const vme::V1190& tdc);
		/// Calibrates TDC channels
		void calculate();
		/// Calculate TOF to another TDC hit
		double get_tof(double other, int hitnum, bool edge = vme::V1190::LEADING);

		/// TcdChannel variables
		class Variables {
		public: // Methods
			/// Constructor, sets data to generic values
			Variables();
			/// Reset variables to default values
			void reset();
			///  Set data values from an database (file or online)
			bool set(const char* dbfile, const char* dir);
			///  Set data values from a constructed database
			bool set(const midas::Database* db, const char* dir);

		public: // Data
			/// TDC variables, common for all hits
			dragon::utils::TdcVariables<1> tdc;
		};

	public: // Data
		/// Array of leading edge hits (calibrated)
		double leading[MAX_HITS];
		/// Array of trailing edge hits (calibrated)
		double trailing[MAX_HITS];

	public: // Subclass instances
		Variables variables;
	};

	///
	/// \brief Generic time-of-flight class for heavy-ion detectors
	/// \details Calculates time of flight between all detectors downstream of MCP0.
	///
	class HiTof {
	public: // Methods
		/// Sets data to defaults
		HiTof();
		/// Sets data to dragon::NO_DATA
		void reset();
		/// Performs calibration of xover tdc and TOF calculations
		void calculate(const dragon::Tail* tail);

	public: // Data
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
	};

	///
	/// Collection of all head detectors and VME modules
	///
	class Head {
	public: // Constants
		/// Max number of RF hits to store
		static const int MAX_RF_HITS = 5;

	public: // Methods
		/// Initializes data values
		Head();
		/// Sets all data values to vme::NONE or other defaults
		void reset();
		///  Reads all variable values from an database (file or online)
		bool set_variables(const char* dbfile);
		///  Reads all variable values from a constructed database
		bool set_variables(const midas::Database* db);
		/// Unpack raw data into VME modules
		void unpack(const midas::Event& event);
		/// Calculate higher-level data for each detector, or across detectors
		void calculate();

	public: // Data
		/// Midas event header
		TMidas_EVENT_HEADER header; //#

		// Vme modules
#ifdef DISPLAY_MODULES
		/// IO32 FPGA
		vme::Io32 io32;   //#
		/// CAEN V792 QDC
		vme::V792 v792;   //#
		/// CAEN V1190 TDC
		vme::V1190 v1190; //#
#else // Supress writing of vme modules to TTree
		vme::Io32 io32;   //!
		vme::V792 v792;   //!
		vme::V1190 v1190; //!
#endif // #ifdef DISPLAY_MODULES

		/// Bgo array
		dragon::Bgo bgo;
		/// RF times
		TdcChannel<MAX_RF_HITS> trf;
		/// Head [bgo] trigger time
		double tcal0;
		/// Crossover [tail] trigger time
		double tcalx;
		/// RF time
		double tcal_rf;

	public: // Subclasses
		///
		/// Head variables
		///
		class Variables {
		public: // Data
			/// IO32 bank name
			midas::Bank_t bk_io32;
			/// Timestamp counter bank name
			midas::Bank_t bk_tsc;
			/// ADC bank name
			midas::Bank_t bk_adc;
			/// TDC bank name
			midas::Bank_t bk_tdc;
			/// Crossover TDC channel variables
			dragon::utils::TdcVariables<1> xtdc;
			/// RF TDC channel variables
			dragon::utils::TdcVariables<1> rf_tdc;
			/// Trigger TDC channel variables
			dragon::utils::TdcVariables<1> tdc0;
		public: // Methods
			/// Sets data to defaults
			Variables();
			/// Sets data to defaults
			void reset();
			///  Set data values from an database (file or online)
			bool set(const char* dbfile);
			///  Set data values from a constructed database
			bool set(const midas::Database* db);
		};

	public: // Subclass instances
		/// Variables instance
		Head::Variables variables; //!
	};

	///
	/// Collection of all tail detectors and VME modules
	///
	class Tail {
	public: // Constants
		/// Number of ADC (caen v785) modules
		static const int NUM_ADC = 2;
		/// Max number of RF hits to store
		static const int MAX_RF_HITS = 5;

	public: // Methods
		/// Initializes data values
		Tail();
		/// Sets all data values to vme::NONE or other defaults
		void reset();
		///  Reads all variable values from an database (file or online)
		bool set_variables(const char* dbfile);
		///  Reads all variable values from a constructed database
		bool set_variables(const midas::Database* db);
		/// Unpack raw data into VME modules
		void unpack(const midas::Event& event);
		/// Calculate higher-level data for each detector, or across detectors
		void calculate();

	public: // Class data
		/// Midas event header
		TMidas_EVENT_HEADER header;   //#

		// Vme modules (write to TTree)
#ifdef DISPLAY_MODULES
		/// IO32 FPGA
		vme::Io32 io32;          //#
		/// CAEN v785 ADC (x2)
		vme::V785 v785[NUM_ADC]; //#
		/// CAEN v1190 TDC
		vme::V1190 v1190;        //#
#else // Supress writing of vme modules to TTree
		vme::Io32 io32;          //!
		vme::V785 v785[NUM_ADC]; //!
		vme::V1190 v1190;        //!
#endif
#ifndef DRAGON_OMIT_DSSSD  // Optional suppression of DSSSD
		/// DSSSD detector
		Dsssd dsssd;             //
#endif
#ifndef DRAGON_OMIT_IC     // Optional suppresion of IonChamber
		/// Ionization chamber
		IonChamber ic;           //
#endif
#ifndef DRAGON_OMIT_NAI    // Optional suppression of NaI detectors
		/// NaI detetors
		NaI nai;                 //
#endif
#ifndef DRAGON_OMIT_GE     // Optional suppression of Ge detector
		/// Germanium detector
		Ge ge;                   //
#endif

		/// MCPs
		Mcp mcp;                 //
		/// Surface barrier detectors
		SurfaceBarrier sb;       //
		/// Time-of-flights
		HiTof tof;               //
		/// RF times
		TdcChannel<MAX_RF_HITS> trf;       //
		/// RF tdc value
		double tcal_rf;
		/// Trigger [tail] tdc value
		double tcal0;
		/// Crossover [head] tdc value
		double tcalx;

	public: // Subclasses
		// Subclasses //
		class Variables {
		public: // Data
			/// IO32 bank name
			midas::Bank_t bk_io32;
			/// Timestamp counter bank name
			midas::Bank_t bk_tsc;
			/// ADC bank names
			midas::Bank_t bk_adc[NUM_ADC];
			/// TDC bank name
			midas::Bank_t bk_tdc;
			/// Crossover TDC channel variables
			dragon::utils::TdcVariables<1> xtdc;
			/// RF TDC channel variables
			dragon::utils::TdcVariables<1> rf_tdc;
			/// Trigger TDC channel variables
			dragon::utils::TdcVariables<1> tdc0;

		public: // Methods
			/// Sets data to defaults
			Variables();
			/// Sets data to defaults
			void reset();
			///  Set data values from an database (file or online)
			bool set(const char* dbfile);
			///  Set data values from a constructed database
			bool set(const midas::Database* db);
		};

	public: // Subclass instances
		/// Variables instance
		Variables variables; //!
	};

	///
	/// A complete Dragon (coincidence) event
	///
	class Coinc {
	public: // Subclasses
		///
		/// Coinc variables
		///
		class Variables {
		public: // Methods
			/// Set data to global defaults
			Variables();
			/// Set data to global defaults
			void reset();
			///  Set data values from an database (file or online)
			bool set(const char* dbfile);
			///  Set data values from a constructed database
			bool set(const midas::Database* db);

		public: // Data
			/// Coincidence time window [microseconds].
			double window;
			/// Time for which potential coincidence events are buffered [seconds].
			double buffer_time;
		};

	public: // Methods
		/// Empty
		Coinc();
		/// Construct from a head and tail event
		Coinc(const Head& head_, const Tail& tail_);
		/// Reset all modules (set data to defaults)
		void reset();
		///  Reads all variable values from an database (file or online)
		bool set_variables(const char* dbfile);
		///  Reads all variable values from a constructed database
		bool set_variables(const midas::Database* db);
		/// Copy data from head and tail coincidence events
		void compose_event(const Head& head_, const Tail& tail_);
		/// Unpack raw data from a midas::CoincEvent
		void unpack(const midas::CoincEvent& coincEvent);
		/// Calculates both singles and coincidence parameters
		void calculate();

	public: // Data
		/// Head (gamma-ray) part of the event
		Head head;
		/// Tail (heavy-ion) part of the event
		Tail tail;
		/// (tail - head) io32 trigger times (usec)
		double xtrig;
		/// Crossover time-of-flight from the head TDC
		double xtofh;
		/// Crossover time-of-flight from the tail TDC
		double xtoft;

	public: // Subclass instances
		/// Variables instance
		Variables variables; //!
	};

	///
	/// Generic dragon scaler class
	///
	class Scaler {
	public: // Constants
		/// Number of scaler channels
		static const int MAX_CHANNELS = 17; //!

	public: // Methods
		/// Initialize data
		Scaler();
		/// Reset all data to zero
		void reset();
		/// Unpack Midas event data into scalar data structiures
		void unpack(const midas::Event& event);
		/// Returns the name of a given scaler channel
		const std::string& channel_name(int ch) const;
		///  Reads all variable values from an database (file or online)
		bool set_variables(const char* dbfile, const char* dir);
		///  Reads all variable values from a constructed database
		bool set_variables(const midas::Database* db, const char* dir);
		/// Set branch alises in a ROOT TTree.
		template <class T>
		void set_aliases(T* t, bool print = false) const;
		/// Plot traditional scaler histograms using 1D bins
		template <class T, class H>
		static int64_t plot(T* tree, const char* varexp, const char* selection = "", Option_t* option = "");

	public: // Data
		/// Number of counts in a single read period
		uint32_t count[MAX_CHANNELS];    //#
		/// Number of counts over the course of a run
		uint32_t sum[MAX_CHANNELS];      //#
		/// Average count rate over the course of a run
		double rate[MAX_CHANNELS];       //#

	public: // Subclasses
		///
		/// Scalar variables
		///
		class Variables {
		public: // Data
			/// Name of a given channel
			std::string names[MAX_CHANNELS];
			/// `count` bank name
			midas::Bank_t bk_count;
			/// `rate` bank name
			midas::Bank_t bk_rate;
			/// `sum` bank name
			midas::Bank_t bk_sum;
			/// Base odb path
			std::string odb_path;

		public: // Methods
			/// Constuctor
			Variables();
			/// Resets names to default values
			void reset();
			///  Set data values from an database (file or online)
			bool set(const char* dbfile, const char* dir);
			///  Set data values from a constructed database
			bool set(const midas::Database* db, const char* dir);
		};

	public: // Subclass instances
		/// Variables instance
		Variables variables; //!
	};

	///
	/// DRAGON EPICS parameters
	///
	class Epics {
	public: // Constants
	public: // Methods
		/// Initialize data
		Epics();
		/// Reset all data to zero
		void reset();
		/// Unpack Midas event data into scalar data structiures
		void unpack(const midas::Event& event);
		/// Returns the name of a given scaler channel
		const std::string& channel_name(int ch) const;
		///  Reads all variable values from an database (file or online)
		bool set_variables(const char* dbfile);
		///  Reads all variable values from a constructed database
		bool set_variables(const midas::Database* db);
		/// Set branch alises in a ROOT TTree.
		template <class T>
		void set_aliases(T* t) const;

	public: // Data
		/// Midas event header
		TMidas_EVENT_HEADER header; //#
		/// Epics channel number for this event
		int32_t ch; //#
		/// EPICS value at `ch` for this event
		float val;  //#

	public: // Subclasses
		///
		/// EPICS variables
		///
		class Variables {
		public: // Data
			/// Name of a given channel
			std::vector<std::string> names;
			/// Bank name
			midas::Bank_t bkname;

		public: // Methods
			/// Constuctor
			Variables();
			/// Resets names to default values
			void reset();
			///  Set data values from an database (file or online)
			bool set(const char* dbfile);
			///  Set data values from a constructed database
			bool set(const midas::Database* db);
		};

	public: // Subclass instances
		/// Variables instance
		Variables variables; //!
	};

} // namespace dragon

// ======= Inlined Implementations ======== //

template <class T>
inline void dragon::Scaler::set_aliases(T* t, bool print) const
{
	/*!
	 * Sets tree branch aliases based on variable values - results in easier to use
	 * branch names (e.g. something descriptive instead of 'scaler.sum[0]', etc.)
	 *
	 * \param t Pointer to a class having a "SetAlias(const char*, const char*) method
	 * (e.g. TTree) and for which you want to set aliases.
	 * \param branchName "Base" name of the branch
	 * \tparam T Some class with a "SetAlias" method.
	 *
	 * \note Intended for TTree, but defined as a template
	 * function to maintain compatability with systems that don't use ROOT. In the case that ROOT is
	 * not available and this function is never used, it simply gets ignored by the compiler. Note also,
	 * that for systems with ROOT, a CINT dictionary definition is automoatically provided for TTree
	 * as the template parameter.
	 *
	 * Example:
	 * \code
	 * // (with a dragon root file loaded)
	 * dragon::Scaler scaler;
	 * scaler.set_variables(variables, "head")
	 * scaler.set_aliases(t2);
	 * t2->Draw("count_triggers_acquired/count_triggers_presented1:Entry$","","L")
	 * \endcode
	 *
	 * \param t Pointer to the TTree for which you want to set aliases.
	 * \param print If true, prints each { alias, source } pair that's set.
	 */
	const std::string chNames[3] = { "count", "sum", "rate" };
	for(int i=0; i< MAX_CHANNELS; ++i) {
		for(int j=0; j< 3; ++j) {
			std::stringstream oldName, newName;
			oldName << chNames[j] << "[" << i << "]";
			newName << chNames[j] << "_" << variables.names[i];
			t->SetAlias(newName.str().c_str(), oldName.str().c_str());
			if(print) {
				std::cout << "Set alias \"" << newName.str() << "\" == \"" << oldName.str() << "\"\n";
			}
		}
	}
}

template <class T, class H>
inline int64_t dragon::Scaler::plot(T* tree, const char* varexp, const char* selection, Option_t* option)
{
	///
	/// This function creates a "traditional" DRAGON scaler histograms, where the x-axis is
	/// time and the bin counts is the number of scaler counts. If one of the `count[]` variables
	/// is plotted, then the integral of the histogram will be the integral of scaler counts across the
	/// whole run
	/// \param tree Tree from which to plot (e.g. `t2` for head `t4` for tail)
	/// \param varexp Same as you would use in TTree::Draw(), i.e. `"count[10]>>hst(3600,0,3600)"`
	/// \param selection Cut selection, same as for TTree::Draw()
	/// \param option DrawOption, same as for TTree::Draw()
	/// \returns Number of events plotted
	///

	tree->SetEstimate(tree->GetEntries());
	int64_t nevts = tree->Draw(varexp, selection, "goff");

	H* hst = tree->GetHistogram();
	if(hst == 0) return 0;
	for(int bin = 0; bin< hst->GetNbinsX() + 1; ++bin)
		hst->SetBinContent(bin, 0);

	for(int64_t i=0; i< nevts; ++i) {
		int64_t bin = hst->FindBin(i);
		hst->SetBinContent(bin, hst->GetBinContent(bin) + tree->GetV1()[i]);
	}
	hst->Draw(option);
	return nevts;
}

template <class T>
inline void dragon::Epics::set_aliases(T* t) const
{
	/*!
	 * See dragon::Scaler::set_aliases()
	 */
	for(int i=0; i< variables.names.size(); ++i) {
		std::stringstream oldName;
		oldName << "1*" << i << "";

		std::string newName1 = variables.names[i];
		for(size_t j=0; j< newName1.size(); ++j) {
			if(newName1[j] == ' ') newName1[j] = '_';
		}
		t->SetAlias(newName1.c_str(), oldName.str().c_str());
	}
}

#include "Defaults.hxx"
#include "midas/Database.hxx"
#include "utils/Functions.hxx"

// ====================== Class dragon::TdcChannel ====================== //

template <int MAX_HITS>
inline dragon::TdcChannel<MAX_HITS>::TdcChannel()
{
	/// ::
	reset();
}

template <int MAX_HITS>
inline void dragon::TdcChannel<MAX_HITS>::reset()
{
	/// ::
	dragon::utils::reset_array(MAX_HITS, leading);
	dragon::utils::reset_array(MAX_HITS, trailing);
}

template <int MAX_HITS>
inline void dragon::TdcChannel<MAX_HITS>::read_data(const vme::V1190& tdc)
{
	/// ::
	for (int i=0; i< MAX_HITS; ++i) {
		leading[i] = tdc.get_leading(variables.tdc.channel, i);
		trailing[i] = tdc.get_trailing(variables.tdc.channel, i);
	}
}

template <int MAX_HITS>
inline void dragon::TdcChannel<MAX_HITS>::calculate()
{
	/// ::
	for (int i=0; i< MAX_HITS; ++i) {
		dragon::utils::linear_calibrate(leading[i], variables.tdc);
		dragon::utils::linear_calibrate(trailing[i], variables.tdc);
	}
}

template <int MAX_HITS>
inline double dragon::TdcChannel<MAX_HITS>::get_tof(double other, int hitnum, bool edge)
{
	/// \param other Value of the other TDC channel for TOF calculation
	/// \param hitnum Desired hit number
	/// \param edge leading (0) or trailing (1) edge
	/// \returns `this - other` (time difference) if hitnum is vald; -1 otherwise

	if((unsigned)hitnum >= MAX_HITS) return -1;

	return edge == vme::V1190::LEADING ?
		leading[hitnum] - other :	trailing[hitnum] - other;
}

// ====================== Class dragon::TdcChannel::Variables ====================== //
template <int MAX_HITS>
inline dragon::TdcChannel<MAX_HITS>::Variables::Variables()
{
	/// ::
	reset();
}

template <int MAX_HITS>
inline void dragon::TdcChannel<MAX_HITS>::Variables::reset()
{
	/// Implementation:
	tdc.channel = HEAD_CROSS_TDC - 2;
	tdc.slope  = 1.;
	tdc.offset = 0.;
}

template <int MAX_HITS>
inline bool dragon::TdcChannel<MAX_HITS>::Variables::set(const char* dbfile, const char* dir)
{
	/// \param dbfile Name of midas database file (or "online")
	/// \param dir ODB path of the variables for this TDC channel
	///
	midas::Database db(dbfile);
	if(db.IsZombie()) {
		dragon::utils::Error("TdcChannel::Variables::set", __FILE__, __LINE__)
			<< "Zombie database: " << dbfile;
		return false;
	}

	return set(&db, dir);
}

template <int MAX_HITS>
inline bool dragon::TdcChannel<MAX_HITS>::Variables::set(const midas::Database* db, const char* dir)
{
	/// \param db Pointer to valid midas database
	/// \param dir ODB path of the variables for this TDC channel (note: no trailing slash).
	///
	std::string dirname = dir + std::string("/");

	bool success = db && !db->IsZombie();
	if(!success) {
		dragon::utils::Error("dragon::TdcChannel::Variables::Set", __FILE__, __LINE__)
			<< "Invalid database: 0x" << db;
	}

	if(success) success = db->ReadValue( (dirname + std::string("channel")).c_str(), tdc.channel);
	if(success) success = db->ReadValue( (dirname + std::string("slope")).c_str(),   tdc.slope);
	if(success) success = db->ReadValue( (dirname + std::string("offset")).c_str(),  tdc.offset);

	return success;
}

#endif // #ifndef HAVE_DRAGON_HXX
