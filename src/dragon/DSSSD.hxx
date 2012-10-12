/// \file DSSSD.hxx
/// \brief Defines the DRAGON Double Sided Silicon Strip Detector (DSSSD) class.
#ifndef DRAGON_DSSSD_HXX
#define DRAGON_DSSSD_HXX


namespace vme {
class V792;
class V1190;
typedef V792 V785;
}


namespace dragon {

/// Double-Sided Silicon Strip Detector
class DSSSD {

public:
	/// Number of channels
	static const int MAX_CHANNELS = 32; //!

public:
	/// DSSSD Variables
	struct Variables {
		/// Maps qraw[i] to adc module number
		int qdc_module[MAX_CHANNELS];

		/// Maps qraw[i] to adc channel number
		int qdc_ch[MAX_CHANNELS];

		/// Maps tof to tdc channel number
		int tof_ch;

		/// Constructor, sets data to generic values (TBD)
		Variables();

		/// \brief Set variable values from an ODB file
		/// \param [in] odb_file Path of the odb file from which you are extracting variable values
		/// \todo Needs to be implemented once ODB is set up
		void set(const char* odb_file);
	};

public:
	/// Instance of DSSSD::Variables
	DSSSD::Variables variables; //!

	/// Raw energy signals
	int16_t qraw[MAX_CHANNELS]; //#

	/// Raw time signal (just one??)
	int16_t tof; //#

public:
	/// Constructor, initialize data
	DSSSD();

	/// Reset all data to VME::none
	void reset();

	/// \brief Read midas event data
	/// \param [in] modules Heavy-ion module structure
	/// \param [in] v1190_trigger_ch Channel number of the v1190b trigger
	void read_data(const vme::V785 adcs[], const vme::V1190& v1190);

	/// Calculate (tof??)
	void calculate();
};

} // namespace dragon

#endif
