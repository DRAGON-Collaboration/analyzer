///\file VariableStructs.hxx
///\author G. Christian
///\brief Defines some structs wrapping common groupings of variables.
#ifndef DRAGON_VARIABLE_STRUCTS_HXX
#define DRAGON_VARIABLE_STRUCTS_HXX


namespace dragon {

//\\\\\\\\\\\\\//
// FIRST ORDER //
//\\\\\\\\\\\\\//

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



//\\\\\\\\\\\\\\//
// SECOND ORDER //
//\\\\\\\\\\\\\\//

/// Adc variables
/*! \tparam N Number of ADC channels. */
template <int N>
struct AdcVariables2 {

	/// Maps parameters to adc module number
	int module[N];

	/// Maps parameters to adc channel number
	int channel[N];

	/// Adc pedestal values
	int pedestal[N];

	/// Adc constant values (for quadratic calibration)
	double offset[N];

	/// Adc 1st order values (for quadratic calibration)
	double slope[N];

	/// Adc 2nd order values (for quadratic calibration)
	double slope2[N];

};

/// Template specialization of AdcVariables2<N> for a single channel
/*! Arrays are converted into a single value */
template <>
struct AdcVariables2<1> {

	/// Maps parameters to adc module number
	int module;

	/// Maps parameters to adc channel number
	int channel;

	/// Adc pedestal values
	int pedestal;

	/// Adc constant values (for quadratic calibration)
	double offset;

	/// Adc 1st order values (for quadratic calibration)
	double slope;

	/// Adc 2nd order values (for quadratic calibration)
	double slope2;

};

/// Tdc variables
/*! \tparam N Number of TDC channels. */
template <int N>
struct TdcVariables2 {

	/// Maps parameters to tdc module number
	int module[N];

	/// Maps parameters to tdc channel number
	int channel[N];

	/// Tdc constant values (for quadratic calibration)
	double offset[N];

	/// Tdc 1st order values (for quadratic calibration)
	double slope[N];

	/// Tdc 2nd order values (for quadratic calibration)
	double slope2[N];

};

/// Template specialization of TdcVariables<N> for a single channel
/*! Arrays are converted into a single value */
template <>
struct TdcVariables2<1> {

	/// Maps parameters to tdc module number
	int module;

	/// Maps parameters to tdc channel number
	int channel;

	/// Tdc constant values (for quadratic calibration)
	double offset;

	/// Tdc 1st order values (for quadratic calibration)
	double slope;

	/// Tdc 2nd order values (for quadratic calibration)
	double slope2;

};

//\\\\\\\\\\\\\\\\\//
// ARBITRARY ORDER //
//\\\\\\\\\\\\\\\\\//

/// Adc variables
/*!
 * \tparam N Number of ADC channels.
 * \tparam Order Order of the ADC calibration
 */
template <int N, int Order>
struct AdcVariablesN {

	/// Maps parameters to adc module number
	int module[N];

	/// Maps parameters to adc channel number
	int channel[N];

	/// Adc pedestal values
	int pedestal[N];

	/// Adc calibration coefficient values
	double coeff[Order][N];

};

/// Template specialization of AdcVariablesN<N> for a single channel
/*! Arrays are converted into a single value */
template <int Order>
struct AdcVariablesN<1, Order> {

	/// Maps parameters to adc module number
	int module;

	/// Maps parameters to adc channel number
	int channel;

	/// Adc pedestal values
	int pedestal;

	/// Adc calibration coefficient values
	double coeff[Order];
};

/// Tdc variables
/*!
 * \tparam N Number of ADC channels.
 * \tparam Order Order of the ADC calibration
 */
template <int N, int Order>
struct TdcVariablesN {

	/// Maps parameters to tdc module number
	int module[N];

	/// Maps parameters to tdc channel number
	int channel[N];

	/// Tdc calibration coefficient values
	double coeff[Order][N];
};

/// Template specialization of TdcVariables<N> for a single channel
/*! Arrays are converted into a single value */
template <int Order>
struct TdcVariablesN<1, Order> {

	/// Maps parameters to tdc module number
	int module;

	/// Maps parameters to tdc channel number
	int channel;

	/// Tdc calibration coefficient values
	double coeff[Order];

};

} // namespace dragon


#endif
