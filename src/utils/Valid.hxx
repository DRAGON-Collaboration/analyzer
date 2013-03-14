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
#include <algorithm>

namespace dragon {

#ifdef DRAGON_NO_DATA

/// "Empty" code used when no data is present in a measurement channel
/*!
 * \note #defining DRAGON_NO_DATA as a value globally sets dragon::NO_DATA; by default it's -1
 */
const int32_t NO_DATA = VME_NO_DATA; 

#else

/// "Empty" code used when no data is present in a measurement channel
/*!
 * \note #defining DRAGON_NO_DATA as a value globally sets dragon::NO_DATA; by default it's -1
 */
const int32_t NO_DATA = -1;

#endif


namespace utils {

template <class T>
inline bool is_valid(T* const tArray, int len)
{
	for(int i=0; i< len; ++i) {
		if(tArray[i] == dragon::NO_DATA) {
			return false;
		}
	}
	return true;
}

template <class T>
inline void reset_array(int len, T* array)
{
	std::fill_n(array, len, dragon::NO_DATA);
}

template <class T0>
inline bool is_valid (const T0& t0)
{
  return ( t0 != dragon::NO_DATA );
}
template <class T0>
inline void reset_data (T0& t0)
{
  t0 = (T0)dragon::NO_DATA;
}
template <class T0, class T1>
inline bool is_valid (const T0& t0, const T1& t1)
{
  return ( t0 != dragon::NO_DATA && t1 != dragon::NO_DATA );
}
template <class T0, class T1>
inline void reset_data (T0& t0, T1& t1)
{
  t0 = (T0)dragon::NO_DATA;
  t1 = (T1)dragon::NO_DATA;
}
template <class T0, class T1, class T2>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2)
{
  return ( t0 != dragon::NO_DATA && t1 != dragon::NO_DATA && t2 != dragon::NO_DATA );
}
template <class T0, class T1, class T2>
inline void reset_data (T0& t0, T1& t1, T2& t2)
{
  t0 = (T0)dragon::NO_DATA;
  t1 = (T1)dragon::NO_DATA;
  t2 = (T2)dragon::NO_DATA;
}
template <class T0, class T1, class T2, class T3>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3)
{
  return ( t0 != dragon::NO_DATA && t1 != dragon::NO_DATA && t2 != dragon::NO_DATA && t3 != dragon::NO_DATA );
}
template <class T0, class T1, class T2, class T3>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3)
{
  t0 = (T0)dragon::NO_DATA;
  t1 = (T1)dragon::NO_DATA;
  t2 = (T2)dragon::NO_DATA;
  t3 = (T3)dragon::NO_DATA;
}
template <class T0, class T1, class T2, class T3, class T4>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4)
{
  return ( t0 != dragon::NO_DATA && t1 != dragon::NO_DATA && t2 != dragon::NO_DATA && t3 != dragon::NO_DATA && t4 != dragon::NO_DATA );
}
template <class T0, class T1, class T2, class T3, class T4>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4)
{
  t0 = (T0)dragon::NO_DATA;
  t1 = (T1)dragon::NO_DATA;
  t2 = (T2)dragon::NO_DATA;
  t3 = (T3)dragon::NO_DATA;
  t4 = (T4)dragon::NO_DATA;
}
template <class T0, class T1, class T2, class T3, class T4, class T5>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5)
{
  return ( t0 != dragon::NO_DATA && t1 != dragon::NO_DATA && t2 != dragon::NO_DATA && t3 != dragon::NO_DATA && t4 != dragon::NO_DATA && t5 != dragon::NO_DATA );
}
template <class T0, class T1, class T2, class T3, class T4, class T5>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5)
{
  t0 = (T0)dragon::NO_DATA;
  t1 = (T1)dragon::NO_DATA;
  t2 = (T2)dragon::NO_DATA;
  t3 = (T3)dragon::NO_DATA;
  t4 = (T4)dragon::NO_DATA;
  t5 = (T5)dragon::NO_DATA;
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6)
{
  return ( t0 != dragon::NO_DATA && t1 != dragon::NO_DATA && t2 != dragon::NO_DATA && t3 != dragon::NO_DATA && t4 != dragon::NO_DATA && t5 != dragon::NO_DATA && t6 != dragon::NO_DATA );
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6)
{
  t0 = (T0)dragon::NO_DATA;
  t1 = (T1)dragon::NO_DATA;
  t2 = (T2)dragon::NO_DATA;
  t3 = (T3)dragon::NO_DATA;
  t4 = (T4)dragon::NO_DATA;
  t5 = (T5)dragon::NO_DATA;
  t6 = (T6)dragon::NO_DATA;
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7)
{
  return ( t0 != dragon::NO_DATA && t1 != dragon::NO_DATA && t2 != dragon::NO_DATA && t3 != dragon::NO_DATA && t4 != dragon::NO_DATA && t5 != dragon::NO_DATA && t6 != dragon::NO_DATA && t7 != dragon::NO_DATA );
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7)
{
  t0 = (T0)dragon::NO_DATA;
  t1 = (T1)dragon::NO_DATA;
  t2 = (T2)dragon::NO_DATA;
  t3 = (T3)dragon::NO_DATA;
  t4 = (T4)dragon::NO_DATA;
  t5 = (T5)dragon::NO_DATA;
  t6 = (T6)dragon::NO_DATA;
  t7 = (T7)dragon::NO_DATA;
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8)
{
  return ( t0 != dragon::NO_DATA && t1 != dragon::NO_DATA && t2 != dragon::NO_DATA && t3 != dragon::NO_DATA && t4 != dragon::NO_DATA && t5 != dragon::NO_DATA && t6 != dragon::NO_DATA && t7 != dragon::NO_DATA && t8 != dragon::NO_DATA );
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8)
{
  t0 = (T0)dragon::NO_DATA;
  t1 = (T1)dragon::NO_DATA;
  t2 = (T2)dragon::NO_DATA;
  t3 = (T3)dragon::NO_DATA;
  t4 = (T4)dragon::NO_DATA;
  t5 = (T5)dragon::NO_DATA;
  t6 = (T6)dragon::NO_DATA;
  t7 = (T7)dragon::NO_DATA;
  t8 = (T8)dragon::NO_DATA;
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9)
{
  return ( t0 != dragon::NO_DATA && t1 != dragon::NO_DATA && t2 != dragon::NO_DATA && t3 != dragon::NO_DATA && t4 != dragon::NO_DATA && t5 != dragon::NO_DATA && t6 != dragon::NO_DATA && t7 != dragon::NO_DATA && t8 != dragon::NO_DATA && t9 != dragon::NO_DATA );
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9)
{
  t0 = (T0)dragon::NO_DATA;
  t1 = (T1)dragon::NO_DATA;
  t2 = (T2)dragon::NO_DATA;
  t3 = (T3)dragon::NO_DATA;
  t4 = (T4)dragon::NO_DATA;
  t5 = (T5)dragon::NO_DATA;
  t6 = (T6)dragon::NO_DATA;
  t7 = (T7)dragon::NO_DATA;
  t8 = (T8)dragon::NO_DATA;
  t9 = (T9)dragon::NO_DATA;
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10)
{
  return ( t0 != dragon::NO_DATA && t1 != dragon::NO_DATA && t2 != dragon::NO_DATA && t3 != dragon::NO_DATA && t4 != dragon::NO_DATA && t5 != dragon::NO_DATA && t6 != dragon::NO_DATA && t7 != dragon::NO_DATA && t8 != dragon::NO_DATA && t9 != dragon::NO_DATA && t10 != dragon::NO_DATA );
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10)
{
  t0 = (T0)dragon::NO_DATA;
  t1 = (T1)dragon::NO_DATA;
  t2 = (T2)dragon::NO_DATA;
  t3 = (T3)dragon::NO_DATA;
  t4 = (T4)dragon::NO_DATA;
  t5 = (T5)dragon::NO_DATA;
  t6 = (T6)dragon::NO_DATA;
  t7 = (T7)dragon::NO_DATA;
  t8 = (T8)dragon::NO_DATA;
  t9 = (T9)dragon::NO_DATA;
  t10 = (T10)dragon::NO_DATA;
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11)
{
  return ( t0 != dragon::NO_DATA && t1 != dragon::NO_DATA && t2 != dragon::NO_DATA && t3 != dragon::NO_DATA && t4 != dragon::NO_DATA && t5 != dragon::NO_DATA && t6 != dragon::NO_DATA && t7 != dragon::NO_DATA && t8 != dragon::NO_DATA && t9 != dragon::NO_DATA && t10 != dragon::NO_DATA && t11 != dragon::NO_DATA );
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11)
{
  t0 = (T0)dragon::NO_DATA;
  t1 = (T1)dragon::NO_DATA;
  t2 = (T2)dragon::NO_DATA;
  t3 = (T3)dragon::NO_DATA;
  t4 = (T4)dragon::NO_DATA;
  t5 = (T5)dragon::NO_DATA;
  t6 = (T6)dragon::NO_DATA;
  t7 = (T7)dragon::NO_DATA;
  t8 = (T8)dragon::NO_DATA;
  t9 = (T9)dragon::NO_DATA;
  t10 = (T10)dragon::NO_DATA;
  t11 = (T11)dragon::NO_DATA;
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11, const T12& t12)
{
  return ( t0 != dragon::NO_DATA && t1 != dragon::NO_DATA && t2 != dragon::NO_DATA && t3 != dragon::NO_DATA && t4 != dragon::NO_DATA && t5 != dragon::NO_DATA && t6 != dragon::NO_DATA && t7 != dragon::NO_DATA && t8 != dragon::NO_DATA && t9 != dragon::NO_DATA && t10 != dragon::NO_DATA && t11 != dragon::NO_DATA && t12 != dragon::NO_DATA );
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11, T12& t12)
{
  t0 = (T0)dragon::NO_DATA;
  t1 = (T1)dragon::NO_DATA;
  t2 = (T2)dragon::NO_DATA;
  t3 = (T3)dragon::NO_DATA;
  t4 = (T4)dragon::NO_DATA;
  t5 = (T5)dragon::NO_DATA;
  t6 = (T6)dragon::NO_DATA;
  t7 = (T7)dragon::NO_DATA;
  t8 = (T8)dragon::NO_DATA;
  t9 = (T9)dragon::NO_DATA;
  t10 = (T10)dragon::NO_DATA;
  t11 = (T11)dragon::NO_DATA;
  t12 = (T12)dragon::NO_DATA;
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11, const T12& t12, const T13& t13)
{
  return ( t0 != dragon::NO_DATA && t1 != dragon::NO_DATA && t2 != dragon::NO_DATA && t3 != dragon::NO_DATA && t4 != dragon::NO_DATA && t5 != dragon::NO_DATA && t6 != dragon::NO_DATA && t7 != dragon::NO_DATA && t8 != dragon::NO_DATA && t9 != dragon::NO_DATA && t10 != dragon::NO_DATA && t11 != dragon::NO_DATA && t12 != dragon::NO_DATA && t13 != dragon::NO_DATA );
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11, T12& t12, T13& t13)
{
  t0 = (T0)dragon::NO_DATA;
  t1 = (T1)dragon::NO_DATA;
  t2 = (T2)dragon::NO_DATA;
  t3 = (T3)dragon::NO_DATA;
  t4 = (T4)dragon::NO_DATA;
  t5 = (T5)dragon::NO_DATA;
  t6 = (T6)dragon::NO_DATA;
  t7 = (T7)dragon::NO_DATA;
  t8 = (T8)dragon::NO_DATA;
  t9 = (T9)dragon::NO_DATA;
  t10 = (T10)dragon::NO_DATA;
  t11 = (T11)dragon::NO_DATA;
  t12 = (T12)dragon::NO_DATA;
  t13 = (T13)dragon::NO_DATA;
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11, const T12& t12, const T13& t13, const T14& t14)
{
  return ( t0 != dragon::NO_DATA && t1 != dragon::NO_DATA && t2 != dragon::NO_DATA && t3 != dragon::NO_DATA && t4 != dragon::NO_DATA && t5 != dragon::NO_DATA && t6 != dragon::NO_DATA && t7 != dragon::NO_DATA && t8 != dragon::NO_DATA && t9 != dragon::NO_DATA && t10 != dragon::NO_DATA && t11 != dragon::NO_DATA && t12 != dragon::NO_DATA && t13 != dragon::NO_DATA && t14 != dragon::NO_DATA );
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11, T12& t12, T13& t13, T14& t14)
{
  t0 = (T0)dragon::NO_DATA;
  t1 = (T1)dragon::NO_DATA;
  t2 = (T2)dragon::NO_DATA;
  t3 = (T3)dragon::NO_DATA;
  t4 = (T4)dragon::NO_DATA;
  t5 = (T5)dragon::NO_DATA;
  t6 = (T6)dragon::NO_DATA;
  t7 = (T7)dragon::NO_DATA;
  t8 = (T8)dragon::NO_DATA;
  t9 = (T9)dragon::NO_DATA;
  t10 = (T10)dragon::NO_DATA;
  t11 = (T11)dragon::NO_DATA;
  t12 = (T12)dragon::NO_DATA;
  t13 = (T13)dragon::NO_DATA;
  t14 = (T14)dragon::NO_DATA;
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11, const T12& t12, const T13& t13, const T14& t14, const T15& t15)
{
  return ( t0 != dragon::NO_DATA && t1 != dragon::NO_DATA && t2 != dragon::NO_DATA && t3 != dragon::NO_DATA && t4 != dragon::NO_DATA && t5 != dragon::NO_DATA && t6 != dragon::NO_DATA && t7 != dragon::NO_DATA && t8 != dragon::NO_DATA && t9 != dragon::NO_DATA && t10 != dragon::NO_DATA && t11 != dragon::NO_DATA && t12 != dragon::NO_DATA && t13 != dragon::NO_DATA && t14 != dragon::NO_DATA && t15 != dragon::NO_DATA );
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11, T12& t12, T13& t13, T14& t14, T15& t15)
{
  t0 = (T0)dragon::NO_DATA;
  t1 = (T1)dragon::NO_DATA;
  t2 = (T2)dragon::NO_DATA;
  t3 = (T3)dragon::NO_DATA;
  t4 = (T4)dragon::NO_DATA;
  t5 = (T5)dragon::NO_DATA;
  t6 = (T6)dragon::NO_DATA;
  t7 = (T7)dragon::NO_DATA;
  t8 = (T8)dragon::NO_DATA;
  t9 = (T9)dragon::NO_DATA;
  t10 = (T10)dragon::NO_DATA;
  t11 = (T11)dragon::NO_DATA;
  t12 = (T12)dragon::NO_DATA;
  t13 = (T13)dragon::NO_DATA;
  t14 = (T14)dragon::NO_DATA;
  t15 = (T15)dragon::NO_DATA;
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11, const T12& t12, const T13& t13, const T14& t14, const T15& t15, const T16& t16)
{
  return ( t0 != dragon::NO_DATA && t1 != dragon::NO_DATA && t2 != dragon::NO_DATA && t3 != dragon::NO_DATA && t4 != dragon::NO_DATA && t5 != dragon::NO_DATA && t6 != dragon::NO_DATA && t7 != dragon::NO_DATA && t8 != dragon::NO_DATA && t9 != dragon::NO_DATA && t10 != dragon::NO_DATA && t11 != dragon::NO_DATA && t12 != dragon::NO_DATA && t13 != dragon::NO_DATA && t14 != dragon::NO_DATA && t15 != dragon::NO_DATA && t16 != dragon::NO_DATA );
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11, T12& t12, T13& t13, T14& t14, T15& t15, T16& t16)
{
  t0 = (T0)dragon::NO_DATA;
  t1 = (T1)dragon::NO_DATA;
  t2 = (T2)dragon::NO_DATA;
  t3 = (T3)dragon::NO_DATA;
  t4 = (T4)dragon::NO_DATA;
  t5 = (T5)dragon::NO_DATA;
  t6 = (T6)dragon::NO_DATA;
  t7 = (T7)dragon::NO_DATA;
  t8 = (T8)dragon::NO_DATA;
  t9 = (T9)dragon::NO_DATA;
  t10 = (T10)dragon::NO_DATA;
  t11 = (T11)dragon::NO_DATA;
  t12 = (T12)dragon::NO_DATA;
  t13 = (T13)dragon::NO_DATA;
  t14 = (T14)dragon::NO_DATA;
  t15 = (T15)dragon::NO_DATA;
  t16 = (T16)dragon::NO_DATA;
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11, const T12& t12, const T13& t13, const T14& t14, const T15& t15, const T16& t16, const T17& t17)
{
  return ( t0 != dragon::NO_DATA && t1 != dragon::NO_DATA && t2 != dragon::NO_DATA && t3 != dragon::NO_DATA && t4 != dragon::NO_DATA && t5 != dragon::NO_DATA && t6 != dragon::NO_DATA && t7 != dragon::NO_DATA && t8 != dragon::NO_DATA && t9 != dragon::NO_DATA && t10 != dragon::NO_DATA && t11 != dragon::NO_DATA && t12 != dragon::NO_DATA && t13 != dragon::NO_DATA && t14 != dragon::NO_DATA && t15 != dragon::NO_DATA && t16 != dragon::NO_DATA && t17 != dragon::NO_DATA );
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11, T12& t12, T13& t13, T14& t14, T15& t15, T16& t16, T17& t17)
{
  t0 = (T0)dragon::NO_DATA;
  t1 = (T1)dragon::NO_DATA;
  t2 = (T2)dragon::NO_DATA;
  t3 = (T3)dragon::NO_DATA;
  t4 = (T4)dragon::NO_DATA;
  t5 = (T5)dragon::NO_DATA;
  t6 = (T6)dragon::NO_DATA;
  t7 = (T7)dragon::NO_DATA;
  t8 = (T8)dragon::NO_DATA;
  t9 = (T9)dragon::NO_DATA;
  t10 = (T10)dragon::NO_DATA;
  t11 = (T11)dragon::NO_DATA;
  t12 = (T12)dragon::NO_DATA;
  t13 = (T13)dragon::NO_DATA;
  t14 = (T14)dragon::NO_DATA;
  t15 = (T15)dragon::NO_DATA;
  t16 = (T16)dragon::NO_DATA;
  t17 = (T17)dragon::NO_DATA;
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11, const T12& t12, const T13& t13, const T14& t14, const T15& t15, const T16& t16, const T17& t17, const T18& t18)
{
  return ( t0 != dragon::NO_DATA && t1 != dragon::NO_DATA && t2 != dragon::NO_DATA && t3 != dragon::NO_DATA && t4 != dragon::NO_DATA && t5 != dragon::NO_DATA && t6 != dragon::NO_DATA && t7 != dragon::NO_DATA && t8 != dragon::NO_DATA && t9 != dragon::NO_DATA && t10 != dragon::NO_DATA && t11 != dragon::NO_DATA && t12 != dragon::NO_DATA && t13 != dragon::NO_DATA && t14 != dragon::NO_DATA && t15 != dragon::NO_DATA && t16 != dragon::NO_DATA && t17 != dragon::NO_DATA && t18 != dragon::NO_DATA );
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11, T12& t12, T13& t13, T14& t14, T15& t15, T16& t16, T17& t17, T18& t18)
{
  t0 = (T0)dragon::NO_DATA;
  t1 = (T1)dragon::NO_DATA;
  t2 = (T2)dragon::NO_DATA;
  t3 = (T3)dragon::NO_DATA;
  t4 = (T4)dragon::NO_DATA;
  t5 = (T5)dragon::NO_DATA;
  t6 = (T6)dragon::NO_DATA;
  t7 = (T7)dragon::NO_DATA;
  t8 = (T8)dragon::NO_DATA;
  t9 = (T9)dragon::NO_DATA;
  t10 = (T10)dragon::NO_DATA;
  t11 = (T11)dragon::NO_DATA;
  t12 = (T12)dragon::NO_DATA;
  t13 = (T13)dragon::NO_DATA;
  t14 = (T14)dragon::NO_DATA;
  t15 = (T15)dragon::NO_DATA;
  t16 = (T16)dragon::NO_DATA;
  t17 = (T17)dragon::NO_DATA;
  t18 = (T18)dragon::NO_DATA;
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11, const T12& t12, const T13& t13, const T14& t14, const T15& t15, const T16& t16, const T17& t17, const T18& t18, const T19& t19)
{
  return ( t0 != dragon::NO_DATA && t1 != dragon::NO_DATA && t2 != dragon::NO_DATA && t3 != dragon::NO_DATA && t4 != dragon::NO_DATA && t5 != dragon::NO_DATA && t6 != dragon::NO_DATA && t7 != dragon::NO_DATA && t8 != dragon::NO_DATA && t9 != dragon::NO_DATA && t10 != dragon::NO_DATA && t11 != dragon::NO_DATA && t12 != dragon::NO_DATA && t13 != dragon::NO_DATA && t14 != dragon::NO_DATA && t15 != dragon::NO_DATA && t16 != dragon::NO_DATA && t17 != dragon::NO_DATA && t18 != dragon::NO_DATA && t19 != dragon::NO_DATA );
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11, T12& t12, T13& t13, T14& t14, T15& t15, T16& t16, T17& t17, T18& t18, T19& t19)
{
  t0 = (T0)dragon::NO_DATA;
  t1 = (T1)dragon::NO_DATA;
  t2 = (T2)dragon::NO_DATA;
  t3 = (T3)dragon::NO_DATA;
  t4 = (T4)dragon::NO_DATA;
  t5 = (T5)dragon::NO_DATA;
  t6 = (T6)dragon::NO_DATA;
  t7 = (T7)dragon::NO_DATA;
  t8 = (T8)dragon::NO_DATA;
  t9 = (T9)dragon::NO_DATA;
  t10 = (T10)dragon::NO_DATA;
  t11 = (T11)dragon::NO_DATA;
  t12 = (T12)dragon::NO_DATA;
  t13 = (T13)dragon::NO_DATA;
  t14 = (T14)dragon::NO_DATA;
  t15 = (T15)dragon::NO_DATA;
  t16 = (T16)dragon::NO_DATA;
  t17 = (T17)dragon::NO_DATA;
  t18 = (T18)dragon::NO_DATA;
  t19 = (T19)dragon::NO_DATA;
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11, const T12& t12, const T13& t13, const T14& t14, const T15& t15, const T16& t16, const T17& t17, const T18& t18, const T19& t19, const T20& t20)
{
  return ( t0 != dragon::NO_DATA && t1 != dragon::NO_DATA && t2 != dragon::NO_DATA && t3 != dragon::NO_DATA && t4 != dragon::NO_DATA && t5 != dragon::NO_DATA && t6 != dragon::NO_DATA && t7 != dragon::NO_DATA && t8 != dragon::NO_DATA && t9 != dragon::NO_DATA && t10 != dragon::NO_DATA && t11 != dragon::NO_DATA && t12 != dragon::NO_DATA && t13 != dragon::NO_DATA && t14 != dragon::NO_DATA && t15 != dragon::NO_DATA && t16 != dragon::NO_DATA && t17 != dragon::NO_DATA && t18 != dragon::NO_DATA && t19 != dragon::NO_DATA && t20 != dragon::NO_DATA );
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11, T12& t12, T13& t13, T14& t14, T15& t15, T16& t16, T17& t17, T18& t18, T19& t19, T20& t20)
{
  t0 = (T0)dragon::NO_DATA;
  t1 = (T1)dragon::NO_DATA;
  t2 = (T2)dragon::NO_DATA;
  t3 = (T3)dragon::NO_DATA;
  t4 = (T4)dragon::NO_DATA;
  t5 = (T5)dragon::NO_DATA;
  t6 = (T6)dragon::NO_DATA;
  t7 = (T7)dragon::NO_DATA;
  t8 = (T8)dragon::NO_DATA;
  t9 = (T9)dragon::NO_DATA;
  t10 = (T10)dragon::NO_DATA;
  t11 = (T11)dragon::NO_DATA;
  t12 = (T12)dragon::NO_DATA;
  t13 = (T13)dragon::NO_DATA;
  t14 = (T14)dragon::NO_DATA;
  t15 = (T15)dragon::NO_DATA;
  t16 = (T16)dragon::NO_DATA;
  t17 = (T17)dragon::NO_DATA;
  t18 = (T18)dragon::NO_DATA;
  t19 = (T19)dragon::NO_DATA;
  t20 = (T20)dragon::NO_DATA;
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11, const T12& t12, const T13& t13, const T14& t14, const T15& t15, const T16& t16, const T17& t17, const T18& t18, const T19& t19, const T20& t20, const T21& t21)
{
  return ( t0 != dragon::NO_DATA && t1 != dragon::NO_DATA && t2 != dragon::NO_DATA && t3 != dragon::NO_DATA && t4 != dragon::NO_DATA && t5 != dragon::NO_DATA && t6 != dragon::NO_DATA && t7 != dragon::NO_DATA && t8 != dragon::NO_DATA && t9 != dragon::NO_DATA && t10 != dragon::NO_DATA && t11 != dragon::NO_DATA && t12 != dragon::NO_DATA && t13 != dragon::NO_DATA && t14 != dragon::NO_DATA && t15 != dragon::NO_DATA && t16 != dragon::NO_DATA && t17 != dragon::NO_DATA && t18 != dragon::NO_DATA && t19 != dragon::NO_DATA && t20 != dragon::NO_DATA && t21 != dragon::NO_DATA );
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11, T12& t12, T13& t13, T14& t14, T15& t15, T16& t16, T17& t17, T18& t18, T19& t19, T20& t20, T21& t21)
{
  t0 = (T0)dragon::NO_DATA;
  t1 = (T1)dragon::NO_DATA;
  t2 = (T2)dragon::NO_DATA;
  t3 = (T3)dragon::NO_DATA;
  t4 = (T4)dragon::NO_DATA;
  t5 = (T5)dragon::NO_DATA;
  t6 = (T6)dragon::NO_DATA;
  t7 = (T7)dragon::NO_DATA;
  t8 = (T8)dragon::NO_DATA;
  t9 = (T9)dragon::NO_DATA;
  t10 = (T10)dragon::NO_DATA;
  t11 = (T11)dragon::NO_DATA;
  t12 = (T12)dragon::NO_DATA;
  t13 = (T13)dragon::NO_DATA;
  t14 = (T14)dragon::NO_DATA;
  t15 = (T15)dragon::NO_DATA;
  t16 = (T16)dragon::NO_DATA;
  t17 = (T17)dragon::NO_DATA;
  t18 = (T18)dragon::NO_DATA;
  t19 = (T19)dragon::NO_DATA;
  t20 = (T20)dragon::NO_DATA;
  t21 = (T21)dragon::NO_DATA;
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11, const T12& t12, const T13& t13, const T14& t14, const T15& t15, const T16& t16, const T17& t17, const T18& t18, const T19& t19, const T20& t20, const T21& t21, const T22& t22)
{
  return ( t0 != dragon::NO_DATA && t1 != dragon::NO_DATA && t2 != dragon::NO_DATA && t3 != dragon::NO_DATA && t4 != dragon::NO_DATA && t5 != dragon::NO_DATA && t6 != dragon::NO_DATA && t7 != dragon::NO_DATA && t8 != dragon::NO_DATA && t9 != dragon::NO_DATA && t10 != dragon::NO_DATA && t11 != dragon::NO_DATA && t12 != dragon::NO_DATA && t13 != dragon::NO_DATA && t14 != dragon::NO_DATA && t15 != dragon::NO_DATA && t16 != dragon::NO_DATA && t17 != dragon::NO_DATA && t18 != dragon::NO_DATA && t19 != dragon::NO_DATA && t20 != dragon::NO_DATA && t21 != dragon::NO_DATA && t22 != dragon::NO_DATA );
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11, T12& t12, T13& t13, T14& t14, T15& t15, T16& t16, T17& t17, T18& t18, T19& t19, T20& t20, T21& t21, T22& t22)
{
  t0 = (T0)dragon::NO_DATA;
  t1 = (T1)dragon::NO_DATA;
  t2 = (T2)dragon::NO_DATA;
  t3 = (T3)dragon::NO_DATA;
  t4 = (T4)dragon::NO_DATA;
  t5 = (T5)dragon::NO_DATA;
  t6 = (T6)dragon::NO_DATA;
  t7 = (T7)dragon::NO_DATA;
  t8 = (T8)dragon::NO_DATA;
  t9 = (T9)dragon::NO_DATA;
  t10 = (T10)dragon::NO_DATA;
  t11 = (T11)dragon::NO_DATA;
  t12 = (T12)dragon::NO_DATA;
  t13 = (T13)dragon::NO_DATA;
  t14 = (T14)dragon::NO_DATA;
  t15 = (T15)dragon::NO_DATA;
  t16 = (T16)dragon::NO_DATA;
  t17 = (T17)dragon::NO_DATA;
  t18 = (T18)dragon::NO_DATA;
  t19 = (T19)dragon::NO_DATA;
  t20 = (T20)dragon::NO_DATA;
  t21 = (T21)dragon::NO_DATA;
  t22 = (T22)dragon::NO_DATA;
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11, const T12& t12, const T13& t13, const T14& t14, const T15& t15, const T16& t16, const T17& t17, const T18& t18, const T19& t19, const T20& t20, const T21& t21, const T22& t22, const T23& t23)
{
  return ( t0 != dragon::NO_DATA && t1 != dragon::NO_DATA && t2 != dragon::NO_DATA && t3 != dragon::NO_DATA && t4 != dragon::NO_DATA && t5 != dragon::NO_DATA && t6 != dragon::NO_DATA && t7 != dragon::NO_DATA && t8 != dragon::NO_DATA && t9 != dragon::NO_DATA && t10 != dragon::NO_DATA && t11 != dragon::NO_DATA && t12 != dragon::NO_DATA && t13 != dragon::NO_DATA && t14 != dragon::NO_DATA && t15 != dragon::NO_DATA && t16 != dragon::NO_DATA && t17 != dragon::NO_DATA && t18 != dragon::NO_DATA && t19 != dragon::NO_DATA && t20 != dragon::NO_DATA && t21 != dragon::NO_DATA && t22 != dragon::NO_DATA && t23 != dragon::NO_DATA );
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11, T12& t12, T13& t13, T14& t14, T15& t15, T16& t16, T17& t17, T18& t18, T19& t19, T20& t20, T21& t21, T22& t22, T23& t23)
{
  t0 = (T0)dragon::NO_DATA;
  t1 = (T1)dragon::NO_DATA;
  t2 = (T2)dragon::NO_DATA;
  t3 = (T3)dragon::NO_DATA;
  t4 = (T4)dragon::NO_DATA;
  t5 = (T5)dragon::NO_DATA;
  t6 = (T6)dragon::NO_DATA;
  t7 = (T7)dragon::NO_DATA;
  t8 = (T8)dragon::NO_DATA;
  t9 = (T9)dragon::NO_DATA;
  t10 = (T10)dragon::NO_DATA;
  t11 = (T11)dragon::NO_DATA;
  t12 = (T12)dragon::NO_DATA;
  t13 = (T13)dragon::NO_DATA;
  t14 = (T14)dragon::NO_DATA;
  t15 = (T15)dragon::NO_DATA;
  t16 = (T16)dragon::NO_DATA;
  t17 = (T17)dragon::NO_DATA;
  t18 = (T18)dragon::NO_DATA;
  t19 = (T19)dragon::NO_DATA;
  t20 = (T20)dragon::NO_DATA;
  t21 = (T21)dragon::NO_DATA;
  t22 = (T22)dragon::NO_DATA;
  t23 = (T23)dragon::NO_DATA;
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11, const T12& t12, const T13& t13, const T14& t14, const T15& t15, const T16& t16, const T17& t17, const T18& t18, const T19& t19, const T20& t20, const T21& t21, const T22& t22, const T23& t23, const T24& t24)
{
  return ( t0 != dragon::NO_DATA && t1 != dragon::NO_DATA && t2 != dragon::NO_DATA && t3 != dragon::NO_DATA && t4 != dragon::NO_DATA && t5 != dragon::NO_DATA && t6 != dragon::NO_DATA && t7 != dragon::NO_DATA && t8 != dragon::NO_DATA && t9 != dragon::NO_DATA && t10 != dragon::NO_DATA && t11 != dragon::NO_DATA && t12 != dragon::NO_DATA && t13 != dragon::NO_DATA && t14 != dragon::NO_DATA && t15 != dragon::NO_DATA && t16 != dragon::NO_DATA && t17 != dragon::NO_DATA && t18 != dragon::NO_DATA && t19 != dragon::NO_DATA && t20 != dragon::NO_DATA && t21 != dragon::NO_DATA && t22 != dragon::NO_DATA && t23 != dragon::NO_DATA && t24 != dragon::NO_DATA );
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11, T12& t12, T13& t13, T14& t14, T15& t15, T16& t16, T17& t17, T18& t18, T19& t19, T20& t20, T21& t21, T22& t22, T23& t23, T24& t24)
{
  t0 = (T0)dragon::NO_DATA;
  t1 = (T1)dragon::NO_DATA;
  t2 = (T2)dragon::NO_DATA;
  t3 = (T3)dragon::NO_DATA;
  t4 = (T4)dragon::NO_DATA;
  t5 = (T5)dragon::NO_DATA;
  t6 = (T6)dragon::NO_DATA;
  t7 = (T7)dragon::NO_DATA;
  t8 = (T8)dragon::NO_DATA;
  t9 = (T9)dragon::NO_DATA;
  t10 = (T10)dragon::NO_DATA;
  t11 = (T11)dragon::NO_DATA;
  t12 = (T12)dragon::NO_DATA;
  t13 = (T13)dragon::NO_DATA;
  t14 = (T14)dragon::NO_DATA;
  t15 = (T15)dragon::NO_DATA;
  t16 = (T16)dragon::NO_DATA;
  t17 = (T17)dragon::NO_DATA;
  t18 = (T18)dragon::NO_DATA;
  t19 = (T19)dragon::NO_DATA;
  t20 = (T20)dragon::NO_DATA;
  t21 = (T21)dragon::NO_DATA;
  t22 = (T22)dragon::NO_DATA;
  t23 = (T23)dragon::NO_DATA;
  t24 = (T24)dragon::NO_DATA;
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11, const T12& t12, const T13& t13, const T14& t14, const T15& t15, const T16& t16, const T17& t17, const T18& t18, const T19& t19, const T20& t20, const T21& t21, const T22& t22, const T23& t23, const T24& t24, const T25& t25)
{
  return ( t0 != dragon::NO_DATA && t1 != dragon::NO_DATA && t2 != dragon::NO_DATA && t3 != dragon::NO_DATA && t4 != dragon::NO_DATA && t5 != dragon::NO_DATA && t6 != dragon::NO_DATA && t7 != dragon::NO_DATA && t8 != dragon::NO_DATA && t9 != dragon::NO_DATA && t10 != dragon::NO_DATA && t11 != dragon::NO_DATA && t12 != dragon::NO_DATA && t13 != dragon::NO_DATA && t14 != dragon::NO_DATA && t15 != dragon::NO_DATA && t16 != dragon::NO_DATA && t17 != dragon::NO_DATA && t18 != dragon::NO_DATA && t19 != dragon::NO_DATA && t20 != dragon::NO_DATA && t21 != dragon::NO_DATA && t22 != dragon::NO_DATA && t23 != dragon::NO_DATA && t24 != dragon::NO_DATA && t25 != dragon::NO_DATA );
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11, T12& t12, T13& t13, T14& t14, T15& t15, T16& t16, T17& t17, T18& t18, T19& t19, T20& t20, T21& t21, T22& t22, T23& t23, T24& t24, T25& t25)
{
  t0 = (T0)dragon::NO_DATA;
  t1 = (T1)dragon::NO_DATA;
  t2 = (T2)dragon::NO_DATA;
  t3 = (T3)dragon::NO_DATA;
  t4 = (T4)dragon::NO_DATA;
  t5 = (T5)dragon::NO_DATA;
  t6 = (T6)dragon::NO_DATA;
  t7 = (T7)dragon::NO_DATA;
  t8 = (T8)dragon::NO_DATA;
  t9 = (T9)dragon::NO_DATA;
  t10 = (T10)dragon::NO_DATA;
  t11 = (T11)dragon::NO_DATA;
  t12 = (T12)dragon::NO_DATA;
  t13 = (T13)dragon::NO_DATA;
  t14 = (T14)dragon::NO_DATA;
  t15 = (T15)dragon::NO_DATA;
  t16 = (T16)dragon::NO_DATA;
  t17 = (T17)dragon::NO_DATA;
  t18 = (T18)dragon::NO_DATA;
  t19 = (T19)dragon::NO_DATA;
  t20 = (T20)dragon::NO_DATA;
  t21 = (T21)dragon::NO_DATA;
  t22 = (T22)dragon::NO_DATA;
  t23 = (T23)dragon::NO_DATA;
  t24 = (T24)dragon::NO_DATA;
  t25 = (T25)dragon::NO_DATA;
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11, const T12& t12, const T13& t13, const T14& t14, const T15& t15, const T16& t16, const T17& t17, const T18& t18, const T19& t19, const T20& t20, const T21& t21, const T22& t22, const T23& t23, const T24& t24, const T25& t25, const T26& t26)
{
  return ( t0 != dragon::NO_DATA && t1 != dragon::NO_DATA && t2 != dragon::NO_DATA && t3 != dragon::NO_DATA && t4 != dragon::NO_DATA && t5 != dragon::NO_DATA && t6 != dragon::NO_DATA && t7 != dragon::NO_DATA && t8 != dragon::NO_DATA && t9 != dragon::NO_DATA && t10 != dragon::NO_DATA && t11 != dragon::NO_DATA && t12 != dragon::NO_DATA && t13 != dragon::NO_DATA && t14 != dragon::NO_DATA && t15 != dragon::NO_DATA && t16 != dragon::NO_DATA && t17 != dragon::NO_DATA && t18 != dragon::NO_DATA && t19 != dragon::NO_DATA && t20 != dragon::NO_DATA && t21 != dragon::NO_DATA && t22 != dragon::NO_DATA && t23 != dragon::NO_DATA && t24 != dragon::NO_DATA && t25 != dragon::NO_DATA && t26 != dragon::NO_DATA );
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11, T12& t12, T13& t13, T14& t14, T15& t15, T16& t16, T17& t17, T18& t18, T19& t19, T20& t20, T21& t21, T22& t22, T23& t23, T24& t24, T25& t25, T26& t26)
{
  t0 = (T0)dragon::NO_DATA;
  t1 = (T1)dragon::NO_DATA;
  t2 = (T2)dragon::NO_DATA;
  t3 = (T3)dragon::NO_DATA;
  t4 = (T4)dragon::NO_DATA;
  t5 = (T5)dragon::NO_DATA;
  t6 = (T6)dragon::NO_DATA;
  t7 = (T7)dragon::NO_DATA;
  t8 = (T8)dragon::NO_DATA;
  t9 = (T9)dragon::NO_DATA;
  t10 = (T10)dragon::NO_DATA;
  t11 = (T11)dragon::NO_DATA;
  t12 = (T12)dragon::NO_DATA;
  t13 = (T13)dragon::NO_DATA;
  t14 = (T14)dragon::NO_DATA;
  t15 = (T15)dragon::NO_DATA;
  t16 = (T16)dragon::NO_DATA;
  t17 = (T17)dragon::NO_DATA;
  t18 = (T18)dragon::NO_DATA;
  t19 = (T19)dragon::NO_DATA;
  t20 = (T20)dragon::NO_DATA;
  t21 = (T21)dragon::NO_DATA;
  t22 = (T22)dragon::NO_DATA;
  t23 = (T23)dragon::NO_DATA;
  t24 = (T24)dragon::NO_DATA;
  t25 = (T25)dragon::NO_DATA;
  t26 = (T26)dragon::NO_DATA;
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11, const T12& t12, const T13& t13, const T14& t14, const T15& t15, const T16& t16, const T17& t17, const T18& t18, const T19& t19, const T20& t20, const T21& t21, const T22& t22, const T23& t23, const T24& t24, const T25& t25, const T26& t26, const T27& t27)
{
  return ( t0 != dragon::NO_DATA && t1 != dragon::NO_DATA && t2 != dragon::NO_DATA && t3 != dragon::NO_DATA && t4 != dragon::NO_DATA && t5 != dragon::NO_DATA && t6 != dragon::NO_DATA && t7 != dragon::NO_DATA && t8 != dragon::NO_DATA && t9 != dragon::NO_DATA && t10 != dragon::NO_DATA && t11 != dragon::NO_DATA && t12 != dragon::NO_DATA && t13 != dragon::NO_DATA && t14 != dragon::NO_DATA && t15 != dragon::NO_DATA && t16 != dragon::NO_DATA && t17 != dragon::NO_DATA && t18 != dragon::NO_DATA && t19 != dragon::NO_DATA && t20 != dragon::NO_DATA && t21 != dragon::NO_DATA && t22 != dragon::NO_DATA && t23 != dragon::NO_DATA && t24 != dragon::NO_DATA && t25 != dragon::NO_DATA && t26 != dragon::NO_DATA && t27 != dragon::NO_DATA );
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11, T12& t12, T13& t13, T14& t14, T15& t15, T16& t16, T17& t17, T18& t18, T19& t19, T20& t20, T21& t21, T22& t22, T23& t23, T24& t24, T25& t25, T26& t26, T27& t27)
{
  t0 = (T0)dragon::NO_DATA;
  t1 = (T1)dragon::NO_DATA;
  t2 = (T2)dragon::NO_DATA;
  t3 = (T3)dragon::NO_DATA;
  t4 = (T4)dragon::NO_DATA;
  t5 = (T5)dragon::NO_DATA;
  t6 = (T6)dragon::NO_DATA;
  t7 = (T7)dragon::NO_DATA;
  t8 = (T8)dragon::NO_DATA;
  t9 = (T9)dragon::NO_DATA;
  t10 = (T10)dragon::NO_DATA;
  t11 = (T11)dragon::NO_DATA;
  t12 = (T12)dragon::NO_DATA;
  t13 = (T13)dragon::NO_DATA;
  t14 = (T14)dragon::NO_DATA;
  t15 = (T15)dragon::NO_DATA;
  t16 = (T16)dragon::NO_DATA;
  t17 = (T17)dragon::NO_DATA;
  t18 = (T18)dragon::NO_DATA;
  t19 = (T19)dragon::NO_DATA;
  t20 = (T20)dragon::NO_DATA;
  t21 = (T21)dragon::NO_DATA;
  t22 = (T22)dragon::NO_DATA;
  t23 = (T23)dragon::NO_DATA;
  t24 = (T24)dragon::NO_DATA;
  t25 = (T25)dragon::NO_DATA;
  t26 = (T26)dragon::NO_DATA;
  t27 = (T27)dragon::NO_DATA;
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27, class T28>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11, const T12& t12, const T13& t13, const T14& t14, const T15& t15, const T16& t16, const T17& t17, const T18& t18, const T19& t19, const T20& t20, const T21& t21, const T22& t22, const T23& t23, const T24& t24, const T25& t25, const T26& t26, const T27& t27, const T28& t28)
{
  return ( t0 != dragon::NO_DATA && t1 != dragon::NO_DATA && t2 != dragon::NO_DATA && t3 != dragon::NO_DATA && t4 != dragon::NO_DATA && t5 != dragon::NO_DATA && t6 != dragon::NO_DATA && t7 != dragon::NO_DATA && t8 != dragon::NO_DATA && t9 != dragon::NO_DATA && t10 != dragon::NO_DATA && t11 != dragon::NO_DATA && t12 != dragon::NO_DATA && t13 != dragon::NO_DATA && t14 != dragon::NO_DATA && t15 != dragon::NO_DATA && t16 != dragon::NO_DATA && t17 != dragon::NO_DATA && t18 != dragon::NO_DATA && t19 != dragon::NO_DATA && t20 != dragon::NO_DATA && t21 != dragon::NO_DATA && t22 != dragon::NO_DATA && t23 != dragon::NO_DATA && t24 != dragon::NO_DATA && t25 != dragon::NO_DATA && t26 != dragon::NO_DATA && t27 != dragon::NO_DATA && t28 != dragon::NO_DATA );
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27, class T28>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11, T12& t12, T13& t13, T14& t14, T15& t15, T16& t16, T17& t17, T18& t18, T19& t19, T20& t20, T21& t21, T22& t22, T23& t23, T24& t24, T25& t25, T26& t26, T27& t27, T28& t28)
{
  t0 = (T0)dragon::NO_DATA;
  t1 = (T1)dragon::NO_DATA;
  t2 = (T2)dragon::NO_DATA;
  t3 = (T3)dragon::NO_DATA;
  t4 = (T4)dragon::NO_DATA;
  t5 = (T5)dragon::NO_DATA;
  t6 = (T6)dragon::NO_DATA;
  t7 = (T7)dragon::NO_DATA;
  t8 = (T8)dragon::NO_DATA;
  t9 = (T9)dragon::NO_DATA;
  t10 = (T10)dragon::NO_DATA;
  t11 = (T11)dragon::NO_DATA;
  t12 = (T12)dragon::NO_DATA;
  t13 = (T13)dragon::NO_DATA;
  t14 = (T14)dragon::NO_DATA;
  t15 = (T15)dragon::NO_DATA;
  t16 = (T16)dragon::NO_DATA;
  t17 = (T17)dragon::NO_DATA;
  t18 = (T18)dragon::NO_DATA;
  t19 = (T19)dragon::NO_DATA;
  t20 = (T20)dragon::NO_DATA;
  t21 = (T21)dragon::NO_DATA;
  t22 = (T22)dragon::NO_DATA;
  t23 = (T23)dragon::NO_DATA;
  t24 = (T24)dragon::NO_DATA;
  t25 = (T25)dragon::NO_DATA;
  t26 = (T26)dragon::NO_DATA;
  t27 = (T27)dragon::NO_DATA;
  t28 = (T28)dragon::NO_DATA;
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27, class T28, class T29>
inline bool is_valid (const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10, const T11& t11, const T12& t12, const T13& t13, const T14& t14, const T15& t15, const T16& t16, const T17& t17, const T18& t18, const T19& t19, const T20& t20, const T21& t21, const T22& t22, const T23& t23, const T24& t24, const T25& t25, const T26& t26, const T27& t27, const T28& t28, const T29& t29)
{
  return ( t0 != dragon::NO_DATA && t1 != dragon::NO_DATA && t2 != dragon::NO_DATA && t3 != dragon::NO_DATA && t4 != dragon::NO_DATA && t5 != dragon::NO_DATA && t6 != dragon::NO_DATA && t7 != dragon::NO_DATA && t8 != dragon::NO_DATA && t9 != dragon::NO_DATA && t10 != dragon::NO_DATA && t11 != dragon::NO_DATA && t12 != dragon::NO_DATA && t13 != dragon::NO_DATA && t14 != dragon::NO_DATA && t15 != dragon::NO_DATA && t16 != dragon::NO_DATA && t17 != dragon::NO_DATA && t18 != dragon::NO_DATA && t19 != dragon::NO_DATA && t20 != dragon::NO_DATA && t21 != dragon::NO_DATA && t22 != dragon::NO_DATA && t23 != dragon::NO_DATA && t24 != dragon::NO_DATA && t25 != dragon::NO_DATA && t26 != dragon::NO_DATA && t27 != dragon::NO_DATA && t28 != dragon::NO_DATA && t29 != dragon::NO_DATA );
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27, class T28, class T29>
inline void reset_data (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10, T11& t11, T12& t12, T13& t13, T14& t14, T15& t15, T16& t16, T17& t17, T18& t18, T19& t19, T20& t20, T21& t21, T22& t22, T23& t23, T24& t24, T25& t25, T26& t26, T27& t27, T28& t28, T29& t29)
{
  t0 = (T0)dragon::NO_DATA;
  t1 = (T1)dragon::NO_DATA;
  t2 = (T2)dragon::NO_DATA;
  t3 = (T3)dragon::NO_DATA;
  t4 = (T4)dragon::NO_DATA;
  t5 = (T5)dragon::NO_DATA;
  t6 = (T6)dragon::NO_DATA;
  t7 = (T7)dragon::NO_DATA;
  t8 = (T8)dragon::NO_DATA;
  t9 = (T9)dragon::NO_DATA;
  t10 = (T10)dragon::NO_DATA;
  t11 = (T11)dragon::NO_DATA;
  t12 = (T12)dragon::NO_DATA;
  t13 = (T13)dragon::NO_DATA;
  t14 = (T14)dragon::NO_DATA;
  t15 = (T15)dragon::NO_DATA;
  t16 = (T16)dragon::NO_DATA;
  t17 = (T17)dragon::NO_DATA;
  t18 = (T18)dragon::NO_DATA;
  t19 = (T19)dragon::NO_DATA;
  t20 = (T20)dragon::NO_DATA;
  t21 = (T21)dragon::NO_DATA;
  t22 = (T22)dragon::NO_DATA;
  t23 = (T23)dragon::NO_DATA;
  t24 = (T24)dragon::NO_DATA;
  t25 = (T25)dragon::NO_DATA;
  t26 = (T26)dragon::NO_DATA;
  t27 = (T27)dragon::NO_DATA;
  t28 = (T28)dragon::NO_DATA;
  t29 = (T29)dragon::NO_DATA;
}

} } // namespace dragon namespace utils


#endif
