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
	/// DSSSD Variables Class
	class Variables {
 private:
		/*!\cond*/
 PRIVATE:
  	/*!\endcond*/
		/// Maps DSSSD::e[i] to adc module number
		int adc_module[MAX_CHANNELS];

		/// Maps DSSSD::e[i] to adc channel number
		int adc_channel[MAX_CHANNELS];

		/// Adc calibration slopes
		double adc_slope[MAX_CHANNELS];

		/// Adc calibration offsets
		double adc_offset[MAX_CHANNELS];

		/// Maps tof to tdc channel number
		int tdc_channel;

		/// TDC calibration slope
		double tdc_slope;

		/// TDC calibration offset
		double tdc_offset;

 public:
		/// Constructor, sets data to generic values
		Variables();

		/// Set variable values from an ODB file
		void set(const char* odb_file);

		/// Reset all variable values to defaults
		void reset();

		/// Allows DSSSD class access to internals
		friend class dragon::DSSSD;
	};

	/// Instance of DSSSD::Variables
	DSSSD::Variables variables; //!

private:
	/*!\cond*/
PRIVATE:
	/*!\endcond*/
	/// Calibrated energy signals
	double ecal[MAX_CHANNELS]; //#

	/// Calibrated time signal
	double tcal; //#

public:
	/// Constructor, initialize data
	DSSSD();

	/// Reset all data to VME::none
	void reset();

	/// Read data from vme modules
	void read_data(const vme::V785 adcs[], const vme::V1190& tdc);

	/// Performs energy calibration
	void calculate();
};

} // namespace dragon

#endif
