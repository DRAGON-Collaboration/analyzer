///\file VariableStructs.hxx
///\author G. Christian
///\brief Defines some structs wrapping common groupings of variables.
#ifndef DRAGON_VARIABLE_STRUCTS_HXX
#define DRAGON_VARIABLE_STRUCTS_HXX


namespace dragon {

/// Adc variables
/*! \tparam N Number of ADC channels. */
template <int N>
struct AdcVariables {

	/// Maps parameters to adc module number
	int module[N];

	/// Maps parameters to adc channel number
	int channel[N];

	/// Adc pedestal values
	int pedestal[N];

	/// Adc offset values (for linear calibration)
	double offset[N];

	/// Adc slope values (for linear calibration)
	double slope[N];

};

/// Template specialization of AdcVariables<N> for a single channel
/*! Arrays are converted into a single value */
template <>
struct AdcVariables<1> {

	/// Maps parameters to adc module number
	int module;

	/// Maps parameters to adc channel number
	int channel;

	/// Adc pedestal values
	int pedestal;

	/// Adc offset values (for linear calibration)
	double offset;

	/// Adc slope values (for linear calibration)
	double slope;

};

/// Tdc variables
/*! \tparam N Number of TDC channels. */
template <int N>
struct TdcVariables {

	/// Maps parameters to tdc module number
	int module[N];

	/// Maps parameters to tdc channel number
	int channel[N];

	/// Tdc offset values (for linear calibration)
	double offset[N];

	/// Tdc slope values (for linear calibration)
	double slope[N];

};

/// Template specialization of TdcVariables<N> for a single channel
/*! Arrays are converted into a single value */
template <>
struct TdcVariables<1> {

	/// Maps parameters to tdc module number
	int module;

	/// Maps parameters to tdc channel number
	int channel;

	/// Tdc offset values (for linear calibration)
	double offset;

	/// Tdc slope values (for linear calibration)
	double slope;

};

} // namespace dragon


#endif
