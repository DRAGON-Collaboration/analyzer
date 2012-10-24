/// \file Functions.hxx
/// \author G. Christian
/// \brief Defines various inlined "utility" functions operating on vme classes.
#ifndef DRAGON_VME_FUNCTIONS_HXX
#define DRAGON_VME_FUNCTIONS_HXX
#include <cmath>
#include <vector>

namespace vme {

/// Fills an array with it's index values
template <class T>
inline void index_fill_n(T begin, int n)
{
	/*!
	 * Example:
	 * \code
	 * double arr[4];
	 * vme::index_fill_n(arr,4);
	 * // arr = { 0., 1., 2., 3. }
	 * \endcode
	 * \param begin Beginning of the array (vector, etc.)
	 * \param n Number of indices past begin to fill
	 */
	for(int i=0; i< n; ++i, ++begin)
		*begin = i;
}


/// Maps raw vme data into another array
/*!
 * \tparam T Basic type of the output array
 * \tparam M Type of the vme module
 * \param [out] output Array containing the mapped values
 * \param [in] numch Number of channels in the output array
 * \param [in] channels Array containing the channel mapping
 * \param [in] module Vme module class containing the raw data
 * 
 * Example:
 * \code
 * vme::V792 adc;
 * adc.unpack(someEvent, "ADC0"); // adc.data = { 21, 24, 26, ... }
 *
 * int channels = { 2, 0, 1, ... };
 * double output[32];
 * channel_map(output, 32, channels, adc); // output = { 26, 21, 24, ... }
 * \endcode
 */
template <class T, class M>
inline void channel_map(T* output, int numch, const int* channels, const M& module)
{
	for (int i=0; i< numch; ++i) {
		output[i] = module.get_data( channels[i] );
	}
}

/// Maps raw vme data into another array, from an array of possible modules
/*!
 * \tparam T Basic type of the output array
 * \tparam M Type of the vme module
 * \param [out] output Array containing the mapped values
 * \param [in] numch Number of channels in the 
 * \param [in] channels Array containing the channel mapping
 * \param [in] channels Array containing the module mapping
 * \param [in] moduleArr Array of vme modules containing the raw data
 * 
 * Example:
 * \code
 * vme::V792[2] adc;
 * adc[0].unpack(someEvent, "ADC0"); // adc[0].data = { 21, 24, 26, ... }
 * adc[1].unpack(someEvent, "ADC1"); // adc[1].data = { 27, 22, 19, ... }
 *
 * int channels = { 2, 0, 1, ... };
 * int modules = { 0, 1, 0, ... };
 * double output[32];
 * channel_map(output, 32, channels, modules, adc); // output = { 26, 27, 24, ... }
 * \endcode
 */
template <class T, class M>
inline void channel_map(T* output, int numch, const int* channels, const int* modules, const M* moduleArr)
{
	for (int i=0; i< numch; ++i) {
		output[i] = moduleArr[ modules[i] ].get_data( channels[i] );
	}
}


/// Channel maps a single value
template <class T, class M>
inline void channel_map(T& output, int channel, const M& module)
{
	/*!
	 * See array version of channel_map()
	 */
	output = module.get_data(channel);
}

/// Channel maps a single value
template <class T, class M>
inline void channel_map(T& output, int channel, int module, const M* moduleArr)
{
	/*!
	 * See array version of channel_map()
	 */
	output = moduleArr[module].get_data(channel);
}

/// Transforms data within an array
/*!
 * \tparam T Basic type of the output array
 * \tparam TR Type of the transformation, should either be a function returning void
 *  and taking as arguments a \c double& (the value to transform) and an \c int (the
 *  channel number) or a class overloading \c operator() to do the same.
 * \param [out] output Array containing the values to transform
 * \param [in] numch Number of channels in the output array
 * \param [in] transform Instance of the transformation class (or function)
 * 
 * Example:
 * \code
 * class LinearCalibrate {
 *   int slope, offset;
 * public:
 *   LinearCalibrate(int slope_, int offset_):
 *     slope(slope_), offset(offset_) { }
 *   void operator() (double value, int channel)
 *     { value = value * slope_ + offset_; }
 * };
 *
 * vme::V792 adc;
 * adc.unpack(someEvent, "ADC0"); // adc.data = { 21, 24, 26, ... }
 *
 * int channels = { 2, 0, 1, ... };
 * double output[32] = { 26., 21., 24. };
 * transform(output, 32, LinearCalibrate(2., 1.)); // output = { 53., 43., 49, ... }
 * \endcode
 */
template <class T, class TR>
inline void transform(T* output, int numch, TR transform)
{
	for (int i=0; i< numch; ++i) {
		transform(output[i], i);
	}
}

/// Transforms a single value
template <class T, class TR>
inline void transform(T& output, TR transform)
{
	/*!
	 * Same as the array transform(), except operating only on a single value
	 * (passed by reference);
	 */
	transform(output, 0);
}

/// Class to perform a linear calibration of a parameter
/*!
 * Usage example:
 * \code
 * double offsets[3] = { 0., 1., 2. };
 * double slopes[3] = { 1., 2., 3. };
 * double values[3] = { 1., 1., 1. };
 * LinearCalibrate linCal(offsets, slopes);
 * for(int i=0; i< 3; ++i) {
 *   linCal(values[i], i);
 * }
 * // values = { 1., 3., 5. }
 * \endcode
 *
 * \note Can be used as the \e transform argument to transform().
 */
class LinearCalibrate {
private:
	const double* const fOffsets; ///< Array of offset values
	const double* const fSlopes; ///< Array of slope values
public:
	/// Sets internal array values
	LinearCalibrate(const double* const offsets, const double* const slopes):
		fOffsets(offsets), fSlopes(slopes) { }

	/// Sets internal array values
	LinearCalibrate(const double& offset, const double& slope):
		fOffsets(&offset), fSlopes(&slope)
		{
			/*!
			 * This version of the constructor is intended for transforming single
			 * values instead of arrays.
			 */ 
		}

	/// Performs the linear calibration
	void operator() (double& value, int channel)
		{
			/*!
			 * \code
			 * value = value*fSlopes[channel] + fOffsets[channel];
			 * \endcode
			 *
			 * \param value Initial value to calibrate, final linearly calibrated value on output
			 * \param channel Indexes the value from fSlope and fOffset to use for the calibration
			 * \warning No bounds checking is done on the internal arrays; it is
			 * the responsibility of the user to ensure that operator() is not called
			 * with a \e channel argument greater than the length of the internal arrays.
			 */
			value = value*fSlopes[channel] + fOffsets[channel];
		}
};

/// Class to perform a quadratic calibration of a parameter
/*!
 * Usage and intent is identical to LinearCalibrate, except with
 * the addition of a 2nd order calibration factor. See LinearCalibrate
 * for examples and detailed documentation.
 */
class QuadraticCalibrate {
private:
	const double* const fOffsets; ///< Array of offset values
	const double* const fSlopes1; ///< Array of 1st order slope values
	const double* const fSlopes2; ///< Array of 2nd order slope values
public:
	/// Sets internal arrays
	QuadraticCalibrate(const double* const offsets, const double* const slopes1, const double* const slopes2):
		fOffsets(offsets), fSlopes1(slopes1), fSlopes2(slopes2) { }

	/// Sets internal array values
	QuadraticCalibrate(const double& offset, const double& slope1, const double& slope2):
		fOffsets(&offset), fSlopes1(&slope1), fSlopes2(&slope2)
		{
			/*!
			 * This version of the constructor is intended for transforming single
			 * values instead of arrays.
			 */ 
		}

	/// Performs the calibration
	void operator() (double& value, int channel)
		{
			/*!
			 * \code
			 * value = value*value*fSlopes2[channel] + value*fSlopes[channel] + fOffsets[channel];
			 * \endcode
			 */
			value = value*value*fSlopes2[channel] + value*fSlopes1[channel] + fOffsets[channel];
		}
};

/// Class to perform arbitrary-order calibration of a parameter
/*!
 * Usage and intent is identical to LinearCalibrate, except that
 * this class can calibrate up to arbitrary order. See LinearCalibrate
 * for examples and detailed documentation.
 */
class PolynomialCalibrate {
private:
	const std::vector<double*> fFactors; ///< Vector of arrays of calibration factors
public:
	/// Sets internal calibration factors
	PolynomialCalibrate(const std::vector<double*>& factors):
		fFactors(factors)
		{
			/*!
			 * Example:
			 * \code
			 * double fact0[3] = { 0., 0., 0. };
			 * double fact1[3] = { 2., 1., 3. };
			 * double fact2[3] = { 0.5, 0.8, 0.4 };
			 * double fact3[3] = { 1.1e-3, 1.3e-3, 1.8e-3 };
			 * double* temp[4] = { fact0, fact1, fact2, fact3 };
			 * std::vector<double> factors(temp, temp+4);
			 * PolynomialCalibrate cal3(factors);
			 * // ... use cal3 as you would LinearCalibrate ... //
			 * \endcode
			 */
		}

	/// Performs the calibration
	void operator() (double& value, int channel)
		{
			/*!
			 * \code
			 * double value0 = value;
			 * value = 0;
			 * for (size_t i=0; i< fFactors.size(); ++i)
			 *	 value += value0 * pow(fFactors[i][channel], i);
			 * \endcode
			 */
			double value0 = value;
			value = 0;
			for (size_t i=0; i< fFactors.size(); ++i)
				value += value0 * pow(fFactors[i][channel], i);
		}
};

}

#endif
