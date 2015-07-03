///
/// \file Sonik.hxx
/// \author G. Christian
/// \brief Defines SONIK detector class
/// \attention The '//!' or '//#' comments following variable declarations are not
///  optional! They are actually used in processing of the file with ROOTCINT.
///
#ifndef HAVE_SONIK_HXX
#define HAVE_SONIK_HXX
#include "utils/IntTypes.h"
#include "utils/VariableStructs.hxx"
#include "Vme.hxx"

// Forward declare midas coincidence event //
namespace midas { class Database; }



///
/// SONIK Detectors
///
class Sonik {
public: // Constants
/// Number of channels
	static const int MAX_CHANNELS = 32; //!

public: // Methods
	/// Constructor, initialize data
	Sonik();
	/// Destructor
	~Sonik();
	/// Reset all data to VME::none
	void reset();
	/// Read data from vme modules
	void read_data(const vme::V785 adcs[], const vme::V1190& tdc);
	/// Performs energy and time calibrations
	void calculate();

public:
	/// Calibrated energy signals
	double ecal[MAX_CHANNELS]; //#
	/// Energy signal of the hit detector (the one w/ highest energy)
	double ehit;        //#
	/// Index of the hit (highest-energy) detector
	uint32_t hit;       //#
	/// Calibrated time signal of the trigger (OR of all detectors)
	double thit;        //#

public: // Subclasses
	///
	/// Sonik Variables Class
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
		/// Tdc variables
		dragon::utils::TdcVariables<1> tdc;
	};

public: // Subclass instances
	/// Instance of Dsssd::Variables
	Sonik::Variables variables; //!
};




#endif
