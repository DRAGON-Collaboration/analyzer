//! \file Auxillary.hxx
//! \brief Defines classes for Auxillary Heavy-Ion detectors.
#ifndef DRAGON_HION_AUXILLARY_HXX
#define DRAGON_HION_AUXILLARY_HXX
#include "dragon/Modules.hxx"
#include "vme/Vme.hxx"

namespace dragon {

namespace hion {

/// Sodium Iodide (NaI) detectors
struct NaI {
// ==== Const Data ==== //
	 /// Number of detectors
	 static const int nch = 2; //!

// ==== Classes ==== //
	 /// NaI variables
	 struct Variables {
			/// Maps detector to adc module number
			int module[NaI::nch];
			
			/// Maps detector to adc channel number
			int ch[NaI::nch];

			/// Calibration slope
			double slope[NaI::nch];

			/// Calibration offset
			double offset[NaI::nch];

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
	 Variables variables; //!

	 /// Raw energy signals
	 int16_t eraw[nch]; //#

	 /// Calibrated energy signals
	 double ecal[nch]; //#

// ==== Methods ==== //
	 /// Constructor, initialize data
	 NaI();

	 /// Destructor, nothing to do
	 ~NaI() { }

	 /// Copy constructor
	 NaI(const NaI& other);

	 /// Equivalency operator
	 NaI& operator= (const NaI& other);

	 /// Reset all data to vme::NONE
	 void reset();

	 /// \brief Read event data from \e modules structure
	 /// \param [in] modules Heavy-ion modules structure
	 void read_data(const dragon::hion::Modules& modules);

	 /// \brief Do energy calibrations
	 void calculate();
	 
#endif
