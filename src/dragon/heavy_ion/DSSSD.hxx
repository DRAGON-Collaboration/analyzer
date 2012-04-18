/// \file DSSSD.hxx
/// \brief Defines the DRAGON Double Sided Silicon Strip Detector (DSSSD) class.
#ifndef DRAGON_DSSSD_HXX
#define DRAGON_DSSSD_HXX
#include "dragon/Modules.hxx"

namespace dragon {

namespace hion {

/// Double-Sided Silicon Strip Detector
struct DSSSD {
// ==== Const Data ==== //
	 /// Number of channels
	 static const int nch = 32; //!

// ==== Classes ==== //	 
	 /// DSSSD Variables
	 struct Variables {
			/// Maps qraw[i] to adc module number (in dragon::hion::Modules)
			int qdc_module[DSSSD::nch];

			/// Maps qraw[i] to adc channel number (in dragon::hion::Modules)
			int qdc_ch[DSSSD::nch];

			/// Maps tof to tdc channel number
			int tof_ch;

			/// Constructor, sets data to generic values (TBD)
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
	 /// Instance of DSSSD::Variables
	 DSSSD::Variables variables; //!

	 /// Raw energy signals
	 int16_t qraw[DSSSD::nch]; //#

	 /// Raw time signal (just one??)
	 int16_t tof; //#

// ==== Methods ==== //
	 /// Constructor, initialize data
	 DSSSD();

	 /// Destructor, nothing to do
	 ~DSSSD() { }

	 /// Copy constructor
	 DSSSD(const DSSSD& other);

	 ///Equivalency operator
	 DSSSD& operator= (const DSSSD& other);

	 /// Reset all data to VME::none
	 void reset();

	 /// \brief Read midas event data
	 /// \param [in] modules Heavy-ion module structure
	 /// \param [in] v1190_trigger_ch Channel number of the v1190b trigger
	 void read_data(const dragon::hion::Modules& modules, int v1190_trigger_ch);
};

} // namespace hion

} // namespace dragon

#endif
