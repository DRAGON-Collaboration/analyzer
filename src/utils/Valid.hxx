/*!
 * \file Valid.hxx
 * \author G. Christian
 * \brief Provides functions for setting and checking if data fields
 *  are invalid.
 * \details Since ROOT trees require a valid entry for every parameter
 *  in every event, we use a "throw away" value to signify when a particular
 *  piece of data wasn't present in an event. In this file, the throw away
 *  value is set, and some functions are provided to check if data fields
 *  are equal to the value, and to set fields equal to the value.
 *
 * \note Most of the template functions were meta-programmed using utils/gen.py
 */
#ifndef  DRAGON_VALID_HXX
#define  DRAGON_VALID_HXX
#include <stdint.h>
#include <numeric>
#include <algorithm>



#ifndef DOXYGEN_SKIP

namespace no_data_impl {

template <class T, bool Signed>
struct Impl {
	static const T value() { return -1; }
};

template <class T>
struct Impl <T, false> {
	static const T value() { return std::numeric_limits<T>::max(); }
};

} // namespace no_data_impl

#endif

namespace dragon {

/// Determines "throwaway" data value when some parameter is absent
template <class T>
struct NoData {
	/// Get the throwaway value
	static const T value()
		{ 
			/// \returns -1 if a signed type, maximim value if an unsigned type
			return no_data_impl::Impl<T, std::numeric_limits<T>::is_signed>::value();
		}
};

namespace utils {

/// Reset all elements of an array to NO_DATA
template <class T>
inline void reset_array(int len, T* array)
{
	/// Implementation:
	std::fill_n(array, len, dragon::NoData<T>::value());
}

/// Check if a single value is valid
template <class T0>
inline bool is_valid (const T0& t0)
{
	/// Implementation:
  return ( t0 != dragon::NoData<T0>::value() );
}

/// Check if all elements in an array are valid
template <class T>
inline bool is_valid_all(T* const tArray, int len)
{
	/// Implementation:
	T* const end = tArray + len;
	return std::find(tArray, end, dragon::NoData<T>::value()) == end;
}

/// Check if any element in an array is valid
template <class T>
inline bool is_valid_any(T* const tArray, int len)
{
	/// Implementation:
	T* const end = tArray + len;
	return std::find_if(tArray, end, is_valid<T>) != end;
}

/// Reset one datum to NO_DATA
template <class T0>
inline void reset_data (T0& t0)
{
  /// Implementation:
  t0 = dragon::NoData<T0>::value();
}

/// Check if 2 data points are valid
template <class T0, class T1>
inline bool is_valid (const T0& t0, const T1& t1)
{
  /// Implementation:
  return ( t0 != dragon::NoData<T0>::value() && t1 != dragon::NoData<T1>::value() );
}

/// Reset 2 data points
template <class T0, class T1>
inline void reset_data (T0& t0, T1& t1)
{
  /// Implementation:
  t0 = dragon::NoData<T0>::value();
  t1 = dragon::NoData<T1>::value();
}

/// Check if 3 data points are valid
template <class T0, class T1, class T2>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2)
{
  /// Implementation:
  return ( t0 != dragon::NoData<T0>::value() && t1 != dragon::NoData<T1>::value() && t2 != dragon::NoData<T2>::value() );
}

/// Reset 3 data points
template <class T0, class T1, class T2>
inline void reset_data (T0& t0, T1& t1, T2& t2)
{
  /// Implementation:
  t0 = dragon::NoData<T0>::value();
  t1 = dragon::NoData<T1>::value();
  t2 = dragon::NoData<T2>::value();
}

/// Check if 4 data points are valid
template <class T0, class T1, class T2, class T3>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3)
{
  /// Implementation:
  return ( t0 != dragon::NoData<T0>::value() && t1 != dragon::NoData<T1>::value() && t2 != dragon::NoData<T2>::value() && t3 != dragon::NoData<T3>::value() );
}

/// Reset 4 data points
template <class T0, class T1, class T2, class T3>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3)
{
  /// Implementation:
  t0 = dragon::NoData<T0>::value();
  t1 = dragon::NoData<T1>::value();
  t2 = dragon::NoData<T2>::value();
  t3 = dragon::NoData<T3>::value();
}

/// Check if 5 data points are valid
template <class T0, class T1, class T2, class T3, class T4>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4)
{
  /// Implementation:
  return ( t0 != dragon::NoData<T0>::value() && t1 != dragon::NoData<T1>::value() && t2 != dragon::NoData<T2>::value() && t3 != dragon::NoData<T3>::value() && t4 != dragon::NoData<T4>::value() );
}

/// Reset 5 data points
template <class T0, class T1, class T2, class T3, class T4>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4)
{
  /// Implementation:
  t0 = dragon::NoData<T0>::value();
  t1 = dragon::NoData<T1>::value();
  t2 = dragon::NoData<T2>::value();
  t3 = dragon::NoData<T3>::value();
  t4 = dragon::NoData<T4>::value();
}

/// Check if 6 data points are valid
template <class T0, class T1, class T2, class T3, class T4, class T5>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5)
{
  /// Implementation:
  return ( t0 != dragon::NoData<T0>::value() && t1 != dragon::NoData<T1>::value() && t2 != dragon::NoData<T2>::value() && t3 != dragon::NoData<T3>::value() && t4 != dragon::NoData<T4>::value() && t5 != dragon::NoData<T5>::value() );
}

/// Reset 6 data points
template <class T0, class T1, class T2, class T3, class T4, class T5>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5)
{
  /// Implementation:
  t0 = dragon::NoData<T0>::value();
  t1 = dragon::NoData<T1>::value();
  t2 = dragon::NoData<T2>::value();
  t3 = dragon::NoData<T3>::value();
  t4 = dragon::NoData<T4>::value();
  t5 = dragon::NoData<T5>::value();
}

/// Check if 7 data points are valid
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6)
{
  /// Implementation:
  return ( t0 != dragon::NoData<T0>::value() && t1 != dragon::NoData<T1>::value() && t2 != dragon::NoData<T2>::value() && t3 != dragon::NoData<T3>::value() && t4 != dragon::NoData<T4>::value() && t5 != dragon::NoData<T5>::value() && t6 != dragon::NoData<T6>::value() );
}

/// Reset 7 data points
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6)
{
  /// Implementation:
  t0 = dragon::NoData<T0>::value();
  t1 = dragon::NoData<T1>::value();
  t2 = dragon::NoData<T2>::value();
  t3 = dragon::NoData<T3>::value();
  t4 = dragon::NoData<T4>::value();
  t5 = dragon::NoData<T5>::value();
  t6 = dragon::NoData<T6>::value();
}

/// Check if 8 data points are valid
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7)
{
  /// Implementation:
  return ( t0 != dragon::NoData<T0>::value() && t1 != dragon::NoData<T1>::value() && t2 != dragon::NoData<T2>::value() && t3 != dragon::NoData<T3>::value() && t4 != dragon::NoData<T4>::value() && t5 != dragon::NoData<T5>::value() && t6 != dragon::NoData<T6>::value() && t7 != dragon::NoData<T7>::value() );
}

/// Reset 8 data points
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7)
{
  /// Implementation:
  t0 = dragon::NoData<T0>::value();
  t1 = dragon::NoData<T1>::value();
  t2 = dragon::NoData<T2>::value();
  t3 = dragon::NoData<T3>::value();
  t4 = dragon::NoData<T4>::value();
  t5 = dragon::NoData<T5>::value();
  t6 = dragon::NoData<T6>::value();
  t7 = dragon::NoData<T7>::value();
}

/// Check if 9 data points are valid
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8)
{
  /// Implementation:
  return ( t0 != dragon::NoData<T0>::value() && t1 != dragon::NoData<T1>::value() && t2 != dragon::NoData<T2>::value() && t3 != dragon::NoData<T3>::value() && t4 != dragon::NoData<T4>::value() && t5 != dragon::NoData<T5>::value() && t6 != dragon::NoData<T6>::value() && t7 != dragon::NoData<T7>::value() && t8 != dragon::NoData<T8>::value() );
}

/// Reset 9 data points
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8)
{
  /// Implementation:
  t0 = dragon::NoData<T0>::value();
  t1 = dragon::NoData<T1>::value();
  t2 = dragon::NoData<T2>::value();
  t3 = dragon::NoData<T3>::value();
  t4 = dragon::NoData<T4>::value();
  t5 = dragon::NoData<T5>::value();
  t6 = dragon::NoData<T6>::value();
  t7 = dragon::NoData<T7>::value();
  t8 = dragon::NoData<T8>::value();
}

/// Check if 10 data points are valid
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9)
{
  /// Implementation:
  return ( t0 != dragon::NoData<T0>::value() && t1 != dragon::NoData<T1>::value() && t2 != dragon::NoData<T2>::value() && t3 != dragon::NoData<T3>::value() && t4 != dragon::NoData<T4>::value() && t5 != dragon::NoData<T5>::value() && t6 != dragon::NoData<T6>::value() && t7 != dragon::NoData<T7>::value() && t8 != dragon::NoData<T8>::value() && t9 != dragon::NoData<T9>::value() );
}

/// Reset 10 data points
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9)
{
  /// Implementation:
  t0 = dragon::NoData<T0>::value();
  t1 = dragon::NoData<T1>::value();
  t2 = dragon::NoData<T2>::value();
  t3 = dragon::NoData<T3>::value();
  t4 = dragon::NoData<T4>::value();
  t5 = dragon::NoData<T5>::value();
  t6 = dragon::NoData<T6>::value();
  t7 = dragon::NoData<T7>::value();
  t8 = dragon::NoData<T8>::value();
  t9 = dragon::NoData<T9>::value();
}

/// Check if 11 data points are valid
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10)
{
  /// Implementation:
  return ( t0 != dragon::NoData<T0>::value() && t1 != dragon::NoData<T1>::value() && t2 != dragon::NoData<T2>::value() && t3 != dragon::NoData<T3>::value() && t4 != dragon::NoData<T4>::value() && t5 != dragon::NoData<T5>::value() && t6 != dragon::NoData<T6>::value() && t7 != dragon::NoData<T7>::value() && t8 != dragon::NoData<T8>::value() && t9 != dragon::NoData<T9>::value() && t10 != dragon::NoData<T10>::value() );
}

/// Reset 11 data points
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10)
{
  /// Implementation:
  t0 = dragon::NoData<T0>::value();
  t1 = dragon::NoData<T1>::value();
  t2 = dragon::NoData<T2>::value();
  t3 = dragon::NoData<T3>::value();
  t4 = dragon::NoData<T4>::value();
  t5 = dragon::NoData<T5>::value();
  t6 = dragon::NoData<T6>::value();
  t7 = dragon::NoData<T7>::value();
  t8 = dragon::NoData<T8>::value();
  t9 = dragon::NoData<T9>::value();
  t10 = dragon::NoData<T10>::value();
}

/// Check if 12 data points are valid
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11)
{
  /// Implementation:
  return ( t0 != dragon::NoData<T0>::value() && t1 != dragon::NoData<T1>::value() && t2 != dragon::NoData<T2>::value() && t3 != dragon::NoData<T3>::value() && t4 != dragon::NoData<T4>::value() && t5 != dragon::NoData<T5>::value() && t6 != dragon::NoData<T6>::value() && t7 != dragon::NoData<T7>::value() && t8 != dragon::NoData<T8>::value() && t9 != dragon::NoData<T9>::value() && t10 != dragon::NoData<T10>::value() && t11 != dragon::NoData<T11>::value() );
}

/// Reset 12 data points
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11)
{
  /// Implementation:
  t0 = dragon::NoData<T0>::value();
  t1 = dragon::NoData<T1>::value();
  t2 = dragon::NoData<T2>::value();
  t3 = dragon::NoData<T3>::value();
  t4 = dragon::NoData<T4>::value();
  t5 = dragon::NoData<T5>::value();
  t6 = dragon::NoData<T6>::value();
  t7 = dragon::NoData<T7>::value();
  t8 = dragon::NoData<T8>::value();
  t9 = dragon::NoData<T9>::value();
  t10 = dragon::NoData<T10>::value();
  t11 = dragon::NoData<T11>::value();
}

/// Check if 13 data points are valid
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11, const T12& t12)
{
  /// Implementation:
  return ( t0 != dragon::NoData<T0>::value() && t1 != dragon::NoData<T1>::value() && t2 != dragon::NoData<T2>::value() && t3 != dragon::NoData<T3>::value() && t4 != dragon::NoData<T4>::value() && t5 != dragon::NoData<T5>::value() && t6 != dragon::NoData<T6>::value() && t7 != dragon::NoData<T7>::value() && t8 != dragon::NoData<T8>::value() && t9 != dragon::NoData<T9>::value() && t10 != dragon::NoData<T10>::value() && t11 != dragon::NoData<T11>::value() && t12 != dragon::NoData<T12>::value() );
}

/// Reset 13 data points
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11, T12& t12)
{
  /// Implementation:
  t0 = dragon::NoData<T0>::value();
  t1 = dragon::NoData<T1>::value();
  t2 = dragon::NoData<T2>::value();
  t3 = dragon::NoData<T3>::value();
  t4 = dragon::NoData<T4>::value();
  t5 = dragon::NoData<T5>::value();
  t6 = dragon::NoData<T6>::value();
  t7 = dragon::NoData<T7>::value();
  t8 = dragon::NoData<T8>::value();
  t9 = dragon::NoData<T9>::value();
  t10 = dragon::NoData<T10>::value();
  t11 = dragon::NoData<T11>::value();
  t12 = dragon::NoData<T12>::value();
}

/// Check if 14 data points are valid
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11, const T12& t12, const T13& t13)
{
  /// Implementation:
  return ( t0 != dragon::NoData<T0>::value() && t1 != dragon::NoData<T1>::value() && t2 != dragon::NoData<T2>::value() && t3 != dragon::NoData<T3>::value() && t4 != dragon::NoData<T4>::value() && t5 != dragon::NoData<T5>::value() && t6 != dragon::NoData<T6>::value() && t7 != dragon::NoData<T7>::value() && t8 != dragon::NoData<T8>::value() && t9 != dragon::NoData<T9>::value() && t10 != dragon::NoData<T10>::value() && t11 != dragon::NoData<T11>::value() && t12 != dragon::NoData<T12>::value() && t13 != dragon::NoData<T13>::value() );
}

/// Reset 14 data points
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11, T12& t12, T13& t13)
{
  /// Implementation:
  t0 = dragon::NoData<T0>::value();
  t1 = dragon::NoData<T1>::value();
  t2 = dragon::NoData<T2>::value();
  t3 = dragon::NoData<T3>::value();
  t4 = dragon::NoData<T4>::value();
  t5 = dragon::NoData<T5>::value();
  t6 = dragon::NoData<T6>::value();
  t7 = dragon::NoData<T7>::value();
  t8 = dragon::NoData<T8>::value();
  t9 = dragon::NoData<T9>::value();
  t10 = dragon::NoData<T10>::value();
  t11 = dragon::NoData<T11>::value();
  t12 = dragon::NoData<T12>::value();
  t13 = dragon::NoData<T13>::value();
}

/// Check if 15 data points are valid
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11, const T12& t12, const T13& t13, const T14& t14)
{
  /// Implementation:
  return ( t0 != dragon::NoData<T0>::value() && t1 != dragon::NoData<T1>::value() && t2 != dragon::NoData<T2>::value() && t3 != dragon::NoData<T3>::value() && t4 != dragon::NoData<T4>::value() && t5 != dragon::NoData<T5>::value() && t6 != dragon::NoData<T6>::value() && t7 != dragon::NoData<T7>::value() && t8 != dragon::NoData<T8>::value() && t9 != dragon::NoData<T9>::value() && t10 != dragon::NoData<T10>::value() && t11 != dragon::NoData<T11>::value() && t12 != dragon::NoData<T12>::value() && t13 != dragon::NoData<T13>::value() && t14 != dragon::NoData<T14>::value() );
}

/// Reset 15 data points
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11, T12& t12, T13& t13, T14& t14)
{
  /// Implementation:
  t0 = dragon::NoData<T0>::value();
  t1 = dragon::NoData<T1>::value();
  t2 = dragon::NoData<T2>::value();
  t3 = dragon::NoData<T3>::value();
  t4 = dragon::NoData<T4>::value();
  t5 = dragon::NoData<T5>::value();
  t6 = dragon::NoData<T6>::value();
  t7 = dragon::NoData<T7>::value();
  t8 = dragon::NoData<T8>::value();
  t9 = dragon::NoData<T9>::value();
  t10 = dragon::NoData<T10>::value();
  t11 = dragon::NoData<T11>::value();
  t12 = dragon::NoData<T12>::value();
  t13 = dragon::NoData<T13>::value();
  t14 = dragon::NoData<T14>::value();
}

/// Check if 16 data points are valid
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11, const T12& t12, const T13& t13, const T14& t14, const T15& t15)
{
  /// Implementation:
  return ( t0 != dragon::NoData<T0>::value() && t1 != dragon::NoData<T1>::value() && t2 != dragon::NoData<T2>::value() && t3 != dragon::NoData<T3>::value() && t4 != dragon::NoData<T4>::value() && t5 != dragon::NoData<T5>::value() && t6 != dragon::NoData<T6>::value() && t7 != dragon::NoData<T7>::value() && t8 != dragon::NoData<T8>::value() && t9 != dragon::NoData<T9>::value() && t10 != dragon::NoData<T10>::value() && t11 != dragon::NoData<T11>::value() && t12 != dragon::NoData<T12>::value() && t13 != dragon::NoData<T13>::value() && t14 != dragon::NoData<T14>::value() && t15 != dragon::NoData<T15>::value() );
}

/// Reset 16 data points
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11, T12& t12, T13& t13, T14& t14, T15& t15)
{
  /// Implementation:
  t0 = dragon::NoData<T0>::value();
  t1 = dragon::NoData<T1>::value();
  t2 = dragon::NoData<T2>::value();
  t3 = dragon::NoData<T3>::value();
  t4 = dragon::NoData<T4>::value();
  t5 = dragon::NoData<T5>::value();
  t6 = dragon::NoData<T6>::value();
  t7 = dragon::NoData<T7>::value();
  t8 = dragon::NoData<T8>::value();
  t9 = dragon::NoData<T9>::value();
  t10 = dragon::NoData<T10>::value();
  t11 = dragon::NoData<T11>::value();
  t12 = dragon::NoData<T12>::value();
  t13 = dragon::NoData<T13>::value();
  t14 = dragon::NoData<T14>::value();
  t15 = dragon::NoData<T15>::value();
}

/// Check if 17 data points are valid
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11, const T12& t12, const T13& t13, const T14& t14, const T15& t15, const T16& t16)
{
  /// Implementation:
  return ( t0 != dragon::NoData<T0>::value() && t1 != dragon::NoData<T1>::value() && t2 != dragon::NoData<T2>::value() && t3 != dragon::NoData<T3>::value() && t4 != dragon::NoData<T4>::value() && t5 != dragon::NoData<T5>::value() && t6 != dragon::NoData<T6>::value() && t7 != dragon::NoData<T7>::value() && t8 != dragon::NoData<T8>::value() && t9 != dragon::NoData<T9>::value() && t10 != dragon::NoData<T10>::value() && t11 != dragon::NoData<T11>::value() && t12 != dragon::NoData<T12>::value() && t13 != dragon::NoData<T13>::value() && t14 != dragon::NoData<T14>::value() && t15 != dragon::NoData<T15>::value() && t16 != dragon::NoData<T16>::value() );
}

/// Reset 17 data points
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11, T12& t12, T13& t13, T14& t14, T15& t15, T16& t16)
{
  /// Implementation:
  t0 = dragon::NoData<T0>::value();
  t1 = dragon::NoData<T1>::value();
  t2 = dragon::NoData<T2>::value();
  t3 = dragon::NoData<T3>::value();
  t4 = dragon::NoData<T4>::value();
  t5 = dragon::NoData<T5>::value();
  t6 = dragon::NoData<T6>::value();
  t7 = dragon::NoData<T7>::value();
  t8 = dragon::NoData<T8>::value();
  t9 = dragon::NoData<T9>::value();
  t10 = dragon::NoData<T10>::value();
  t11 = dragon::NoData<T11>::value();
  t12 = dragon::NoData<T12>::value();
  t13 = dragon::NoData<T13>::value();
  t14 = dragon::NoData<T14>::value();
  t15 = dragon::NoData<T15>::value();
  t16 = dragon::NoData<T16>::value();
}

/// Check if 18 data points are valid
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11, const T12& t12, const T13& t13, const T14& t14, const T15& t15, const T16& t16, const T17& t17)
{
  /// Implementation:
  return ( t0 != dragon::NoData<T0>::value() && t1 != dragon::NoData<T1>::value() && t2 != dragon::NoData<T2>::value() && t3 != dragon::NoData<T3>::value() && t4 != dragon::NoData<T4>::value() && t5 != dragon::NoData<T5>::value() && t6 != dragon::NoData<T6>::value() && t7 != dragon::NoData<T7>::value() && t8 != dragon::NoData<T8>::value() && t9 != dragon::NoData<T9>::value() && t10 != dragon::NoData<T10>::value() && t11 != dragon::NoData<T11>::value() && t12 != dragon::NoData<T12>::value() && t13 != dragon::NoData<T13>::value() && t14 != dragon::NoData<T14>::value() && t15 != dragon::NoData<T15>::value() && t16 != dragon::NoData<T16>::value() && t17 != dragon::NoData<T17>::value() );
}

/// Reset 18 data points
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11, T12& t12, T13& t13, T14& t14, T15& t15, T16& t16, T17& t17)
{
  /// Implementation:
  t0 = dragon::NoData<T0>::value();
  t1 = dragon::NoData<T1>::value();
  t2 = dragon::NoData<T2>::value();
  t3 = dragon::NoData<T3>::value();
  t4 = dragon::NoData<T4>::value();
  t5 = dragon::NoData<T5>::value();
  t6 = dragon::NoData<T6>::value();
  t7 = dragon::NoData<T7>::value();
  t8 = dragon::NoData<T8>::value();
  t9 = dragon::NoData<T9>::value();
  t10 = dragon::NoData<T10>::value();
  t11 = dragon::NoData<T11>::value();
  t12 = dragon::NoData<T12>::value();
  t13 = dragon::NoData<T13>::value();
  t14 = dragon::NoData<T14>::value();
  t15 = dragon::NoData<T15>::value();
  t16 = dragon::NoData<T16>::value();
  t17 = dragon::NoData<T17>::value();
}

/// Check if 19 data points are valid
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11, const T12& t12, const T13& t13, const T14& t14, const T15& t15, const T16& t16, const T17& t17, const T18& t18)
{
  /// Implementation:
  return ( t0 != dragon::NoData<T0>::value() && t1 != dragon::NoData<T1>::value() && t2 != dragon::NoData<T2>::value() && t3 != dragon::NoData<T3>::value() && t4 != dragon::NoData<T4>::value() && t5 != dragon::NoData<T5>::value() && t6 != dragon::NoData<T6>::value() && t7 != dragon::NoData<T7>::value() && t8 != dragon::NoData<T8>::value() && t9 != dragon::NoData<T9>::value() && t10 != dragon::NoData<T10>::value() && t11 != dragon::NoData<T11>::value() && t12 != dragon::NoData<T12>::value() && t13 != dragon::NoData<T13>::value() && t14 != dragon::NoData<T14>::value() && t15 != dragon::NoData<T15>::value() && t16 != dragon::NoData<T16>::value() && t17 != dragon::NoData<T17>::value() && t18 != dragon::NoData<T18>::value() );
}

/// Reset 19 data points
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11, T12& t12, T13& t13, T14& t14, T15& t15, T16& t16, T17& t17, T18& t18)
{
  /// Implementation:
  t0 = dragon::NoData<T0>::value();
  t1 = dragon::NoData<T1>::value();
  t2 = dragon::NoData<T2>::value();
  t3 = dragon::NoData<T3>::value();
  t4 = dragon::NoData<T4>::value();
  t5 = dragon::NoData<T5>::value();
  t6 = dragon::NoData<T6>::value();
  t7 = dragon::NoData<T7>::value();
  t8 = dragon::NoData<T8>::value();
  t9 = dragon::NoData<T9>::value();
  t10 = dragon::NoData<T10>::value();
  t11 = dragon::NoData<T11>::value();
  t12 = dragon::NoData<T12>::value();
  t13 = dragon::NoData<T13>::value();
  t14 = dragon::NoData<T14>::value();
  t15 = dragon::NoData<T15>::value();
  t16 = dragon::NoData<T16>::value();
  t17 = dragon::NoData<T17>::value();
  t18 = dragon::NoData<T18>::value();
}

/// Check if 20 data points are valid
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11, const T12& t12, const T13& t13, const T14& t14, const T15& t15, const T16& t16, const T17& t17, const T18& t18, const T19& t19)
{
  /// Implementation:
  return ( t0 != dragon::NoData<T0>::value() && t1 != dragon::NoData<T1>::value() && t2 != dragon::NoData<T2>::value() && t3 != dragon::NoData<T3>::value() && t4 != dragon::NoData<T4>::value() && t5 != dragon::NoData<T5>::value() && t6 != dragon::NoData<T6>::value() && t7 != dragon::NoData<T7>::value() && t8 != dragon::NoData<T8>::value() && t9 != dragon::NoData<T9>::value() && t10 != dragon::NoData<T10>::value() && t11 != dragon::NoData<T11>::value() && t12 != dragon::NoData<T12>::value() && t13 != dragon::NoData<T13>::value() && t14 != dragon::NoData<T14>::value() && t15 != dragon::NoData<T15>::value() && t16 != dragon::NoData<T16>::value() && t17 != dragon::NoData<T17>::value() && t18 != dragon::NoData<T18>::value() && t19 != dragon::NoData<T19>::value() );
}

/// Reset 20 data points
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11, T12& t12, T13& t13, T14& t14, T15& t15, T16& t16, T17& t17, T18& t18, T19& t19)
{
  /// Implementation:
  t0 = dragon::NoData<T0>::value();
  t1 = dragon::NoData<T1>::value();
  t2 = dragon::NoData<T2>::value();
  t3 = dragon::NoData<T3>::value();
  t4 = dragon::NoData<T4>::value();
  t5 = dragon::NoData<T5>::value();
  t6 = dragon::NoData<T6>::value();
  t7 = dragon::NoData<T7>::value();
  t8 = dragon::NoData<T8>::value();
  t9 = dragon::NoData<T9>::value();
  t10 = dragon::NoData<T10>::value();
  t11 = dragon::NoData<T11>::value();
  t12 = dragon::NoData<T12>::value();
  t13 = dragon::NoData<T13>::value();
  t14 = dragon::NoData<T14>::value();
  t15 = dragon::NoData<T15>::value();
  t16 = dragon::NoData<T16>::value();
  t17 = dragon::NoData<T17>::value();
  t18 = dragon::NoData<T18>::value();
  t19 = dragon::NoData<T19>::value();
}

/// Check if 21 data points are valid
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11, const T12& t12, const T13& t13, const T14& t14, const T15& t15, const T16& t16, const T17& t17, const T18& t18, const T19& t19, const T20& t20)
{
  /// Implementation:
  return ( t0 != dragon::NoData<T0>::value() && t1 != dragon::NoData<T1>::value() && t2 != dragon::NoData<T2>::value() && t3 != dragon::NoData<T3>::value() && t4 != dragon::NoData<T4>::value() && t5 != dragon::NoData<T5>::value() && t6 != dragon::NoData<T6>::value() && t7 != dragon::NoData<T7>::value() && t8 != dragon::NoData<T8>::value() && t9 != dragon::NoData<T9>::value() && t10 != dragon::NoData<T10>::value() && t11 != dragon::NoData<T11>::value() && t12 != dragon::NoData<T12>::value() && t13 != dragon::NoData<T13>::value() && t14 != dragon::NoData<T14>::value() && t15 != dragon::NoData<T15>::value() && t16 != dragon::NoData<T16>::value() && t17 != dragon::NoData<T17>::value() && t18 != dragon::NoData<T18>::value() && t19 != dragon::NoData<T19>::value() && t20 != dragon::NoData<T20>::value() );
}

/// Reset 21 data points
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11, T12& t12, T13& t13, T14& t14, T15& t15, T16& t16, T17& t17, T18& t18, T19& t19, T20& t20)
{
  /// Implementation:
  t0 = dragon::NoData<T0>::value();
  t1 = dragon::NoData<T1>::value();
  t2 = dragon::NoData<T2>::value();
  t3 = dragon::NoData<T3>::value();
  t4 = dragon::NoData<T4>::value();
  t5 = dragon::NoData<T5>::value();
  t6 = dragon::NoData<T6>::value();
  t7 = dragon::NoData<T7>::value();
  t8 = dragon::NoData<T8>::value();
  t9 = dragon::NoData<T9>::value();
  t10 = dragon::NoData<T10>::value();
  t11 = dragon::NoData<T11>::value();
  t12 = dragon::NoData<T12>::value();
  t13 = dragon::NoData<T13>::value();
  t14 = dragon::NoData<T14>::value();
  t15 = dragon::NoData<T15>::value();
  t16 = dragon::NoData<T16>::value();
  t17 = dragon::NoData<T17>::value();
  t18 = dragon::NoData<T18>::value();
  t19 = dragon::NoData<T19>::value();
  t20 = dragon::NoData<T20>::value();
}

/// Check if 22 data points are valid
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11, const T12& t12, const T13& t13, const T14& t14, const T15& t15, const T16& t16, const T17& t17, const T18& t18, const T19& t19, const T20& t20, const T21& t21)
{
  /// Implementation:
  return ( t0 != dragon::NoData<T0>::value() && t1 != dragon::NoData<T1>::value() && t2 != dragon::NoData<T2>::value() && t3 != dragon::NoData<T3>::value() && t4 != dragon::NoData<T4>::value() && t5 != dragon::NoData<T5>::value() && t6 != dragon::NoData<T6>::value() && t7 != dragon::NoData<T7>::value() && t8 != dragon::NoData<T8>::value() && t9 != dragon::NoData<T9>::value() && t10 != dragon::NoData<T10>::value() && t11 != dragon::NoData<T11>::value() && t12 != dragon::NoData<T12>::value() && t13 != dragon::NoData<T13>::value() && t14 != dragon::NoData<T14>::value() && t15 != dragon::NoData<T15>::value() && t16 != dragon::NoData<T16>::value() && t17 != dragon::NoData<T17>::value() && t18 != dragon::NoData<T18>::value() && t19 != dragon::NoData<T19>::value() && t20 != dragon::NoData<T20>::value() && t21 != dragon::NoData<T21>::value() );
}

/// Reset 22 data points
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11, T12& t12, T13& t13, T14& t14, T15& t15, T16& t16, T17& t17, T18& t18, T19& t19, T20& t20, T21& t21)
{
  /// Implementation:
  t0 = dragon::NoData<T0>::value();
  t1 = dragon::NoData<T1>::value();
  t2 = dragon::NoData<T2>::value();
  t3 = dragon::NoData<T3>::value();
  t4 = dragon::NoData<T4>::value();
  t5 = dragon::NoData<T5>::value();
  t6 = dragon::NoData<T6>::value();
  t7 = dragon::NoData<T7>::value();
  t8 = dragon::NoData<T8>::value();
  t9 = dragon::NoData<T9>::value();
  t10 = dragon::NoData<T10>::value();
  t11 = dragon::NoData<T11>::value();
  t12 = dragon::NoData<T12>::value();
  t13 = dragon::NoData<T13>::value();
  t14 = dragon::NoData<T14>::value();
  t15 = dragon::NoData<T15>::value();
  t16 = dragon::NoData<T16>::value();
  t17 = dragon::NoData<T17>::value();
  t18 = dragon::NoData<T18>::value();
  t19 = dragon::NoData<T19>::value();
  t20 = dragon::NoData<T20>::value();
  t21 = dragon::NoData<T21>::value();
}

/// Check if 23 data points are valid
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11, const T12& t12, const T13& t13, const T14& t14, const T15& t15, const T16& t16, const T17& t17, const T18& t18, const T19& t19, const T20& t20, const T21& t21, const T22& t22)
{
  /// Implementation:
  return ( t0 != dragon::NoData<T0>::value() && t1 != dragon::NoData<T1>::value() && t2 != dragon::NoData<T2>::value() && t3 != dragon::NoData<T3>::value() && t4 != dragon::NoData<T4>::value() && t5 != dragon::NoData<T5>::value() && t6 != dragon::NoData<T6>::value() && t7 != dragon::NoData<T7>::value() && t8 != dragon::NoData<T8>::value() && t9 != dragon::NoData<T9>::value() && t10 != dragon::NoData<T10>::value() && t11 != dragon::NoData<T11>::value() && t12 != dragon::NoData<T12>::value() && t13 != dragon::NoData<T13>::value() && t14 != dragon::NoData<T14>::value() && t15 != dragon::NoData<T15>::value() && t16 != dragon::NoData<T16>::value() && t17 != dragon::NoData<T17>::value() && t18 != dragon::NoData<T18>::value() && t19 != dragon::NoData<T19>::value() && t20 != dragon::NoData<T20>::value() && t21 != dragon::NoData<T21>::value() && t22 != dragon::NoData<T22>::value() );
}

/// Reset 23 data points
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11, T12& t12, T13& t13, T14& t14, T15& t15, T16& t16, T17& t17, T18& t18, T19& t19, T20& t20, T21& t21, T22& t22)
{
  /// Implementation:
  t0 = dragon::NoData<T0>::value();
  t1 = dragon::NoData<T1>::value();
  t2 = dragon::NoData<T2>::value();
  t3 = dragon::NoData<T3>::value();
  t4 = dragon::NoData<T4>::value();
  t5 = dragon::NoData<T5>::value();
  t6 = dragon::NoData<T6>::value();
  t7 = dragon::NoData<T7>::value();
  t8 = dragon::NoData<T8>::value();
  t9 = dragon::NoData<T9>::value();
  t10 = dragon::NoData<T10>::value();
  t11 = dragon::NoData<T11>::value();
  t12 = dragon::NoData<T12>::value();
  t13 = dragon::NoData<T13>::value();
  t14 = dragon::NoData<T14>::value();
  t15 = dragon::NoData<T15>::value();
  t16 = dragon::NoData<T16>::value();
  t17 = dragon::NoData<T17>::value();
  t18 = dragon::NoData<T18>::value();
  t19 = dragon::NoData<T19>::value();
  t20 = dragon::NoData<T20>::value();
  t21 = dragon::NoData<T21>::value();
  t22 = dragon::NoData<T22>::value();
}

/// Check if 24 data points are valid
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11, const T12& t12, const T13& t13, const T14& t14, const T15& t15, const T16& t16, const T17& t17, const T18& t18, const T19& t19, const T20& t20, const T21& t21, const T22& t22, const T23& t23)
{
  /// Implementation:
  return ( t0 != dragon::NoData<T0>::value() && t1 != dragon::NoData<T1>::value() && t2 != dragon::NoData<T2>::value() && t3 != dragon::NoData<T3>::value() && t4 != dragon::NoData<T4>::value() && t5 != dragon::NoData<T5>::value() && t6 != dragon::NoData<T6>::value() && t7 != dragon::NoData<T7>::value() && t8 != dragon::NoData<T8>::value() && t9 != dragon::NoData<T9>::value() && t10 != dragon::NoData<T10>::value() && t11 != dragon::NoData<T11>::value() && t12 != dragon::NoData<T12>::value() && t13 != dragon::NoData<T13>::value() && t14 != dragon::NoData<T14>::value() && t15 != dragon::NoData<T15>::value() && t16 != dragon::NoData<T16>::value() && t17 != dragon::NoData<T17>::value() && t18 != dragon::NoData<T18>::value() && t19 != dragon::NoData<T19>::value() && t20 != dragon::NoData<T20>::value() && t21 != dragon::NoData<T21>::value() && t22 != dragon::NoData<T22>::value() && t23 != dragon::NoData<T23>::value() );
}

/// Reset 24 data points
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11, T12& t12, T13& t13, T14& t14, T15& t15, T16& t16, T17& t17, T18& t18, T19& t19, T20& t20, T21& t21, T22& t22, T23& t23)
{
  /// Implementation:
  t0 = dragon::NoData<T0>::value();
  t1 = dragon::NoData<T1>::value();
  t2 = dragon::NoData<T2>::value();
  t3 = dragon::NoData<T3>::value();
  t4 = dragon::NoData<T4>::value();
  t5 = dragon::NoData<T5>::value();
  t6 = dragon::NoData<T6>::value();
  t7 = dragon::NoData<T7>::value();
  t8 = dragon::NoData<T8>::value();
  t9 = dragon::NoData<T9>::value();
  t10 = dragon::NoData<T10>::value();
  t11 = dragon::NoData<T11>::value();
  t12 = dragon::NoData<T12>::value();
  t13 = dragon::NoData<T13>::value();
  t14 = dragon::NoData<T14>::value();
  t15 = dragon::NoData<T15>::value();
  t16 = dragon::NoData<T16>::value();
  t17 = dragon::NoData<T17>::value();
  t18 = dragon::NoData<T18>::value();
  t19 = dragon::NoData<T19>::value();
  t20 = dragon::NoData<T20>::value();
  t21 = dragon::NoData<T21>::value();
  t22 = dragon::NoData<T22>::value();
  t23 = dragon::NoData<T23>::value();
}

/// Check if 25 data points are valid
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11, const T12& t12, const T13& t13, const T14& t14, const T15& t15, const T16& t16, const T17& t17, const T18& t18, const T19& t19, const T20& t20, const T21& t21, const T22& t22, const T23& t23, const T24& t24)
{
  /// Implementation:
  return ( t0 != dragon::NoData<T0>::value() && t1 != dragon::NoData<T1>::value() && t2 != dragon::NoData<T2>::value() && t3 != dragon::NoData<T3>::value() && t4 != dragon::NoData<T4>::value() && t5 != dragon::NoData<T5>::value() && t6 != dragon::NoData<T6>::value() && t7 != dragon::NoData<T7>::value() && t8 != dragon::NoData<T8>::value() && t9 != dragon::NoData<T9>::value() && t10 != dragon::NoData<T10>::value() && t11 != dragon::NoData<T11>::value() && t12 != dragon::NoData<T12>::value() && t13 != dragon::NoData<T13>::value() && t14 != dragon::NoData<T14>::value() && t15 != dragon::NoData<T15>::value() && t16 != dragon::NoData<T16>::value() && t17 != dragon::NoData<T17>::value() && t18 != dragon::NoData<T18>::value() && t19 != dragon::NoData<T19>::value() && t20 != dragon::NoData<T20>::value() && t21 != dragon::NoData<T21>::value() && t22 != dragon::NoData<T22>::value() && t23 != dragon::NoData<T23>::value() && t24 != dragon::NoData<T24>::value() );
}

/// Reset 25 data points
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11, T12& t12, T13& t13, T14& t14, T15& t15, T16& t16, T17& t17, T18& t18, T19& t19, T20& t20, T21& t21, T22& t22, T23& t23, T24& t24)
{
  /// Implementation:
  t0 = dragon::NoData<T0>::value();
  t1 = dragon::NoData<T1>::value();
  t2 = dragon::NoData<T2>::value();
  t3 = dragon::NoData<T3>::value();
  t4 = dragon::NoData<T4>::value();
  t5 = dragon::NoData<T5>::value();
  t6 = dragon::NoData<T6>::value();
  t7 = dragon::NoData<T7>::value();
  t8 = dragon::NoData<T8>::value();
  t9 = dragon::NoData<T9>::value();
  t10 = dragon::NoData<T10>::value();
  t11 = dragon::NoData<T11>::value();
  t12 = dragon::NoData<T12>::value();
  t13 = dragon::NoData<T13>::value();
  t14 = dragon::NoData<T14>::value();
  t15 = dragon::NoData<T15>::value();
  t16 = dragon::NoData<T16>::value();
  t17 = dragon::NoData<T17>::value();
  t18 = dragon::NoData<T18>::value();
  t19 = dragon::NoData<T19>::value();
  t20 = dragon::NoData<T20>::value();
  t21 = dragon::NoData<T21>::value();
  t22 = dragon::NoData<T22>::value();
  t23 = dragon::NoData<T23>::value();
  t24 = dragon::NoData<T24>::value();
}

/// Check if 26 data points are valid
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11, const T12& t12, const T13& t13, const T14& t14, const T15& t15, const T16& t16, const T17& t17, const T18& t18, const T19& t19, const T20& t20, const T21& t21, const T22& t22, const T23& t23, const T24& t24, const T25& t25)
{
  /// Implementation:
  return ( t0 != dragon::NoData<T0>::value() && t1 != dragon::NoData<T1>::value() && t2 != dragon::NoData<T2>::value() && t3 != dragon::NoData<T3>::value() && t4 != dragon::NoData<T4>::value() && t5 != dragon::NoData<T5>::value() && t6 != dragon::NoData<T6>::value() && t7 != dragon::NoData<T7>::value() && t8 != dragon::NoData<T8>::value() && t9 != dragon::NoData<T9>::value() && t10 != dragon::NoData<T10>::value() && t11 != dragon::NoData<T11>::value() && t12 != dragon::NoData<T12>::value() && t13 != dragon::NoData<T13>::value() && t14 != dragon::NoData<T14>::value() && t15 != dragon::NoData<T15>::value() && t16 != dragon::NoData<T16>::value() && t17 != dragon::NoData<T17>::value() && t18 != dragon::NoData<T18>::value() && t19 != dragon::NoData<T19>::value() && t20 != dragon::NoData<T20>::value() && t21 != dragon::NoData<T21>::value() && t22 != dragon::NoData<T22>::value() && t23 != dragon::NoData<T23>::value() && t24 != dragon::NoData<T24>::value() && t25 != dragon::NoData<T25>::value() );
}

/// Reset 26 data points
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11, T12& t12, T13& t13, T14& t14, T15& t15, T16& t16, T17& t17, T18& t18, T19& t19, T20& t20, T21& t21, T22& t22, T23& t23, T24& t24, T25& t25)
{
  /// Implementation:
  t0 = dragon::NoData<T0>::value();
  t1 = dragon::NoData<T1>::value();
  t2 = dragon::NoData<T2>::value();
  t3 = dragon::NoData<T3>::value();
  t4 = dragon::NoData<T4>::value();
  t5 = dragon::NoData<T5>::value();
  t6 = dragon::NoData<T6>::value();
  t7 = dragon::NoData<T7>::value();
  t8 = dragon::NoData<T8>::value();
  t9 = dragon::NoData<T9>::value();
  t10 = dragon::NoData<T10>::value();
  t11 = dragon::NoData<T11>::value();
  t12 = dragon::NoData<T12>::value();
  t13 = dragon::NoData<T13>::value();
  t14 = dragon::NoData<T14>::value();
  t15 = dragon::NoData<T15>::value();
  t16 = dragon::NoData<T16>::value();
  t17 = dragon::NoData<T17>::value();
  t18 = dragon::NoData<T18>::value();
  t19 = dragon::NoData<T19>::value();
  t20 = dragon::NoData<T20>::value();
  t21 = dragon::NoData<T21>::value();
  t22 = dragon::NoData<T22>::value();
  t23 = dragon::NoData<T23>::value();
  t24 = dragon::NoData<T24>::value();
  t25 = dragon::NoData<T25>::value();
}

/// Check if 27 data points are valid
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11, const T12& t12, const T13& t13, const T14& t14, const T15& t15, const T16& t16, const T17& t17, const T18& t18, const T19& t19, const T20& t20, const T21& t21, const T22& t22, const T23& t23, const T24& t24, const T25& t25, const T26& t26)
{
  /// Implementation:
  return ( t0 != dragon::NoData<T0>::value() && t1 != dragon::NoData<T1>::value() && t2 != dragon::NoData<T2>::value() && t3 != dragon::NoData<T3>::value() && t4 != dragon::NoData<T4>::value() && t5 != dragon::NoData<T5>::value() && t6 != dragon::NoData<T6>::value() && t7 != dragon::NoData<T7>::value() && t8 != dragon::NoData<T8>::value() && t9 != dragon::NoData<T9>::value() && t10 != dragon::NoData<T10>::value() && t11 != dragon::NoData<T11>::value() && t12 != dragon::NoData<T12>::value() && t13 != dragon::NoData<T13>::value() && t14 != dragon::NoData<T14>::value() && t15 != dragon::NoData<T15>::value() && t16 != dragon::NoData<T16>::value() && t17 != dragon::NoData<T17>::value() && t18 != dragon::NoData<T18>::value() && t19 != dragon::NoData<T19>::value() && t20 != dragon::NoData<T20>::value() && t21 != dragon::NoData<T21>::value() && t22 != dragon::NoData<T22>::value() && t23 != dragon::NoData<T23>::value() && t24 != dragon::NoData<T24>::value() && t25 != dragon::NoData<T25>::value() && t26 != dragon::NoData<T26>::value() );
}

/// Reset 27 data points
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11, T12& t12, T13& t13, T14& t14, T15& t15, T16& t16, T17& t17, T18& t18, T19& t19, T20& t20, T21& t21, T22& t22, T23& t23, T24& t24, T25& t25, T26& t26)
{
  /// Implementation:
  t0 = dragon::NoData<T0>::value();
  t1 = dragon::NoData<T1>::value();
  t2 = dragon::NoData<T2>::value();
  t3 = dragon::NoData<T3>::value();
  t4 = dragon::NoData<T4>::value();
  t5 = dragon::NoData<T5>::value();
  t6 = dragon::NoData<T6>::value();
  t7 = dragon::NoData<T7>::value();
  t8 = dragon::NoData<T8>::value();
  t9 = dragon::NoData<T9>::value();
  t10 = dragon::NoData<T10>::value();
  t11 = dragon::NoData<T11>::value();
  t12 = dragon::NoData<T12>::value();
  t13 = dragon::NoData<T13>::value();
  t14 = dragon::NoData<T14>::value();
  t15 = dragon::NoData<T15>::value();
  t16 = dragon::NoData<T16>::value();
  t17 = dragon::NoData<T17>::value();
  t18 = dragon::NoData<T18>::value();
  t19 = dragon::NoData<T19>::value();
  t20 = dragon::NoData<T20>::value();
  t21 = dragon::NoData<T21>::value();
  t22 = dragon::NoData<T22>::value();
  t23 = dragon::NoData<T23>::value();
  t24 = dragon::NoData<T24>::value();
  t25 = dragon::NoData<T25>::value();
  t26 = dragon::NoData<T26>::value();
}

/// Check if 28 data points are valid
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11, const T12& t12, const T13& t13, const T14& t14, const T15& t15, const T16& t16, const T17& t17, const T18& t18, const T19& t19, const T20& t20, const T21& t21, const T22& t22, const T23& t23, const T24& t24, const T25& t25, const T26& t26, const T27& t27)
{
  /// Implementation:
  return ( t0 != dragon::NoData<T0>::value() && t1 != dragon::NoData<T1>::value() && t2 != dragon::NoData<T2>::value() && t3 != dragon::NoData<T3>::value() && t4 != dragon::NoData<T4>::value() && t5 != dragon::NoData<T5>::value() && t6 != dragon::NoData<T6>::value() && t7 != dragon::NoData<T7>::value() && t8 != dragon::NoData<T8>::value() && t9 != dragon::NoData<T9>::value() && t10 != dragon::NoData<T10>::value() && t11 != dragon::NoData<T11>::value() && t12 != dragon::NoData<T12>::value() && t13 != dragon::NoData<T13>::value() && t14 != dragon::NoData<T14>::value() && t15 != dragon::NoData<T15>::value() && t16 != dragon::NoData<T16>::value() && t17 != dragon::NoData<T17>::value() && t18 != dragon::NoData<T18>::value() && t19 != dragon::NoData<T19>::value() && t20 != dragon::NoData<T20>::value() && t21 != dragon::NoData<T21>::value() && t22 != dragon::NoData<T22>::value() && t23 != dragon::NoData<T23>::value() && t24 != dragon::NoData<T24>::value() && t25 != dragon::NoData<T25>::value() && t26 != dragon::NoData<T26>::value() && t27 != dragon::NoData<T27>::value() );
}

/// Reset 28 data points
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11, T12& t12, T13& t13, T14& t14, T15& t15, T16& t16, T17& t17, T18& t18, T19& t19, T20& t20, T21& t21, T22& t22, T23& t23, T24& t24, T25& t25, T26& t26, T27& t27)
{
  /// Implementation:
  t0 = dragon::NoData<T0>::value();
  t1 = dragon::NoData<T1>::value();
  t2 = dragon::NoData<T2>::value();
  t3 = dragon::NoData<T3>::value();
  t4 = dragon::NoData<T4>::value();
  t5 = dragon::NoData<T5>::value();
  t6 = dragon::NoData<T6>::value();
  t7 = dragon::NoData<T7>::value();
  t8 = dragon::NoData<T8>::value();
  t9 = dragon::NoData<T9>::value();
  t10 = dragon::NoData<T10>::value();
  t11 = dragon::NoData<T11>::value();
  t12 = dragon::NoData<T12>::value();
  t13 = dragon::NoData<T13>::value();
  t14 = dragon::NoData<T14>::value();
  t15 = dragon::NoData<T15>::value();
  t16 = dragon::NoData<T16>::value();
  t17 = dragon::NoData<T17>::value();
  t18 = dragon::NoData<T18>::value();
  t19 = dragon::NoData<T19>::value();
  t20 = dragon::NoData<T20>::value();
  t21 = dragon::NoData<T21>::value();
  t22 = dragon::NoData<T22>::value();
  t23 = dragon::NoData<T23>::value();
  t24 = dragon::NoData<T24>::value();
  t25 = dragon::NoData<T25>::value();
  t26 = dragon::NoData<T26>::value();
  t27 = dragon::NoData<T27>::value();
}

/// Check if 29 data points are valid
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27, class T28>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11, const T12& t12, const T13& t13, const T14& t14, const T15& t15, const T16& t16, const T17& t17, const T18& t18, const T19& t19, const T20& t20, const T21& t21, const T22& t22, const T23& t23, const T24& t24, const T25& t25, const T26& t26, const T27& t27, const T28& t28)
{
  /// Implementation:
  return ( t0 != dragon::NoData<T0>::value() && t1 != dragon::NoData<T1>::value() && t2 != dragon::NoData<T2>::value() && t3 != dragon::NoData<T3>::value() && t4 != dragon::NoData<T4>::value() && t5 != dragon::NoData<T5>::value() && t6 != dragon::NoData<T6>::value() && t7 != dragon::NoData<T7>::value() && t8 != dragon::NoData<T8>::value() && t9 != dragon::NoData<T9>::value() && t10 != dragon::NoData<T10>::value() && t11 != dragon::NoData<T11>::value() && t12 != dragon::NoData<T12>::value() && t13 != dragon::NoData<T13>::value() && t14 != dragon::NoData<T14>::value() && t15 != dragon::NoData<T15>::value() && t16 != dragon::NoData<T16>::value() && t17 != dragon::NoData<T17>::value() && t18 != dragon::NoData<T18>::value() && t19 != dragon::NoData<T19>::value() && t20 != dragon::NoData<T20>::value() && t21 != dragon::NoData<T21>::value() && t22 != dragon::NoData<T22>::value() && t23 != dragon::NoData<T23>::value() && t24 != dragon::NoData<T24>::value() && t25 != dragon::NoData<T25>::value() && t26 != dragon::NoData<T26>::value() && t27 != dragon::NoData<T27>::value() && t28 != dragon::NoData<T28>::value() );
}

/// Reset 29 data points
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27, class T28>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11, T12& t12, T13& t13, T14& t14, T15& t15, T16& t16, T17& t17, T18& t18, T19& t19, T20& t20, T21& t21, T22& t22, T23& t23, T24& t24, T25& t25, T26& t26, T27& t27, T28& t28)
{
  /// Implementation:
  t0 = dragon::NoData<T0>::value();
  t1 = dragon::NoData<T1>::value();
  t2 = dragon::NoData<T2>::value();
  t3 = dragon::NoData<T3>::value();
  t4 = dragon::NoData<T4>::value();
  t5 = dragon::NoData<T5>::value();
  t6 = dragon::NoData<T6>::value();
  t7 = dragon::NoData<T7>::value();
  t8 = dragon::NoData<T8>::value();
  t9 = dragon::NoData<T9>::value();
  t10 = dragon::NoData<T10>::value();
  t11 = dragon::NoData<T11>::value();
  t12 = dragon::NoData<T12>::value();
  t13 = dragon::NoData<T13>::value();
  t14 = dragon::NoData<T14>::value();
  t15 = dragon::NoData<T15>::value();
  t16 = dragon::NoData<T16>::value();
  t17 = dragon::NoData<T17>::value();
  t18 = dragon::NoData<T18>::value();
  t19 = dragon::NoData<T19>::value();
  t20 = dragon::NoData<T20>::value();
  t21 = dragon::NoData<T21>::value();
  t22 = dragon::NoData<T22>::value();
  t23 = dragon::NoData<T23>::value();
  t24 = dragon::NoData<T24>::value();
  t25 = dragon::NoData<T25>::value();
  t26 = dragon::NoData<T26>::value();
  t27 = dragon::NoData<T27>::value();
  t28 = dragon::NoData<T28>::value();
}

/// Check if 30 data points are valid
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27, class T28, class T29>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11, const T12& t12, const T13& t13, const T14& t14, const T15& t15, const T16& t16, const T17& t17, const T18& t18, const T19& t19, const T20& t20, const T21& t21, const T22& t22, const T23& t23, const T24& t24, const T25& t25, const T26& t26, const T27& t27, const T28& t28, const T29& t29)
{
  /// Implementation:
  return ( t0 != dragon::NoData<T0>::value() && t1 != dragon::NoData<T1>::value() && t2 != dragon::NoData<T2>::value() && t3 != dragon::NoData<T3>::value() && t4 != dragon::NoData<T4>::value() && t5 != dragon::NoData<T5>::value() && t6 != dragon::NoData<T6>::value() && t7 != dragon::NoData<T7>::value() && t8 != dragon::NoData<T8>::value() && t9 != dragon::NoData<T9>::value() && t10 != dragon::NoData<T10>::value() && t11 != dragon::NoData<T11>::value() && t12 != dragon::NoData<T12>::value() && t13 != dragon::NoData<T13>::value() && t14 != dragon::NoData<T14>::value() && t15 != dragon::NoData<T15>::value() && t16 != dragon::NoData<T16>::value() && t17 != dragon::NoData<T17>::value() && t18 != dragon::NoData<T18>::value() && t19 != dragon::NoData<T19>::value() && t20 != dragon::NoData<T20>::value() && t21 != dragon::NoData<T21>::value() && t22 != dragon::NoData<T22>::value() && t23 != dragon::NoData<T23>::value() && t24 != dragon::NoData<T24>::value() && t25 != dragon::NoData<T25>::value() && t26 != dragon::NoData<T26>::value() && t27 != dragon::NoData<T27>::value() && t28 != dragon::NoData<T28>::value() && t29 != dragon::NoData<T29>::value() );
}

/// Reset 30 data points
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27, class T28, class T29>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11, T12& t12, T13& t13, T14& t14, T15& t15, T16& t16, T17& t17, T18& t18, T19& t19, T20& t20, T21& t21, T22& t22, T23& t23, T24& t24, T25& t25, T26& t26, T27& t27, T28& t28, T29& t29)
{
  /// Implementation:
  t0 = dragon::NoData<T0>::value();
  t1 = dragon::NoData<T1>::value();
  t2 = dragon::NoData<T2>::value();
  t3 = dragon::NoData<T3>::value();
  t4 = dragon::NoData<T4>::value();
  t5 = dragon::NoData<T5>::value();
  t6 = dragon::NoData<T6>::value();
  t7 = dragon::NoData<T7>::value();
  t8 = dragon::NoData<T8>::value();
  t9 = dragon::NoData<T9>::value();
  t10 = dragon::NoData<T10>::value();
  t11 = dragon::NoData<T11>::value();
  t12 = dragon::NoData<T12>::value();
  t13 = dragon::NoData<T13>::value();
  t14 = dragon::NoData<T14>::value();
  t15 = dragon::NoData<T15>::value();
  t16 = dragon::NoData<T16>::value();
  t17 = dragon::NoData<T17>::value();
  t18 = dragon::NoData<T18>::value();
  t19 = dragon::NoData<T19>::value();
  t20 = dragon::NoData<T20>::value();
  t21 = dragon::NoData<T21>::value();
  t22 = dragon::NoData<T22>::value();
  t23 = dragon::NoData<T23>::value();
  t24 = dragon::NoData<T24>::value();
  t25 = dragon::NoData<T25>::value();
  t26 = dragon::NoData<T26>::value();
  t27 = dragon::NoData<T27>::value();
  t28 = dragon::NoData<T28>::value();
  t29 = dragon::NoData<T29>::value();
}

} } // namespace dragon namespace utils


#endif
