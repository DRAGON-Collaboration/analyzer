//! \file Vme.hxx
//! \brief Defines classes relevant to unpacking data from VME modules.
#ifndef  DRAGON_VME_HXX
#define  DRAGON_VME_HXX
#include <stdint.h>
#include <algorithm>
#include <cstring>

#include "Constants.hxx"

/// Encloses all VME related classes, functions, and constants
namespace vme {
}

namespace {

template <class T>
inline bool is_valid(const T& t)
{
	return t != vme::NONE;
} 

template <class T1, class T2>
inline bool is_valid(const T1& t1, const T2& t2)
{
	return (t1 != vme::NONE && t2 != vme::NONE);
} 

template <class T1, class T2, class T3>
inline bool is_valid(const T1& t1, const T2& t2, const T3& t3)
{
	return (t1 != vme::NONE && t2 != vme::NONE && t3 != vme::NONE);
}

template <class T1, class T2, class T3, class T4>
inline bool is_valid(const T1& t1, const T2& t2, const T3& t3, const T4& t4)
{
	return (t1 != vme::NONE && t2 != vme::NONE && t3 != vme::NONE && t4 != vme::NONE);
}

template <class T1, class T2, class T3, class T4, class T5>
inline bool is_valid(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5)
{
	return (t1 != vme::NONE && t2 != vme::NONE && t3 != vme::NONE && t4 != vme::NONE && t5 != vme::NONE);
}

template <class T1, class T2, class T3, class T4, class T5,
					class T6>
inline bool is_valid(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5,
	                   const T6& t6)
{
	return (t1 != vme::NONE && t2 != vme::NONE && t3 != vme::NONE && t4 != vme::NONE && t5 != vme::NONE &&
		      t6 != vme::NONE);
}

template <class T1, class T2, class T3, class T4, class T5,
					class T6, class T7>
inline bool is_valid(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5,
	                   const T6& t6, const T7& t7)
{
	return (t1 != vme::NONE && t2 != vme::NONE && t3 != vme::NONE && t4 != vme::NONE && t5 != vme::NONE &&
		      t6 != vme::NONE && t7 != vme::NONE);
}

template <class T1, class T2, class T3, class T4, class T5,
					class T6, class T7, class T8>
inline bool is_valid(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5,
	                   const T6& t6, const T7& t7, const T8& t8)
{
	return (t1 != vme::NONE && t2 != vme::NONE && t3 != vme::NONE && t4 != vme::NONE && t5 != vme::NONE &&
		      t6 != vme::NONE && t7 != vme::NONE && t8 != vme::NONE);
}

template <class T1, class T2, class T3, class T4, class T5,
					class T6, class T7, class T8, class T9>
inline bool is_valid(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5,
	                   const T6& t6, const T7& t7, const T8& t8, const T9& t9)
{
	return (t1 != vme::NONE && t2 != vme::NONE && t3 != vme::NONE && t4 != vme::NONE && t5 != vme::NONE &&
		      t6 != vme::NONE && t7 != vme::NONE && t8 != vme::NONE && t9 != vme::NONE);
}

template <class T1, class T2, class T3, class T4, class T5,
					class T6, class T7, class T8, class T9, class T10>
inline bool is_valid(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5,
	                   const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10)
{
	return (t1 != vme::NONE && t2 != vme::NONE && t3 != vme::NONE && t4 != vme::NONE && t5 != vme::NONE &&
		      t6 != vme::NONE && t7 != vme::NONE && t8 != vme::NONE && t9 != vme::NONE && t10 != vme::NONE);
}

template <class T>
inline bool is_valid(T* const tArray, int len)
{
	for(int i=0; i< len; ++i) {
		if(tArray[i] == vme::NONE) {
			return false;
		}
	}
	return true;
}

template <class T>
inline void reset_data(T* t1,     T* t2 = 0, T* t3 = 0, T* t4 = 0, T* t5  = 0,
											 T* t6 = 0, T* t7 = 0, T* t8 = 0, T* t9 = 0, T* t10 = 0)
{
	if(t1)  *t1  = (T)vme::NONE;
	if(t2)  *t2  = (T)vme::NONE;
	if(t3)  *t3  = (T)vme::NONE;
	if(t4)  *t4  = (T)vme::NONE;
	if(t5)  *t5  = (T)vme::NONE;
	if(t6)  *t6  = (T)vme::NONE;
	if(t7)  *t7  = (T)vme::NONE;
	if(t8)  *t8  = (T)vme::NONE;
	if(t9)  *t9  = (T)vme::NONE;
	if(t10) *t10 = (T)vme::NONE;
}

template <class T>
inline void reset_array(int len, T* array)
{
	std::fill_n(array, len, vme::NONE);
}

template <class T>
inline void reset_arrays(int len,
												 T* t1,      T* t2  = 0,
												 T* t3 = 0,  T* t4  = 0,
												 T* t5 = 0,  T* t6  = 0,
												 T* t7 = 0,  T* t8  = 0,
												 T* t9 = 0,  T* t10 = 0)
{
	if(t1)  std::fill_n(t1,  len, vme::NONE);
	if(t2)  std::fill_n(t2,  len, vme::NONE);
	if(t3)  std::fill_n(t3,  len, vme::NONE);
	if(t4)  std::fill_n(t4,  len, vme::NONE);
	if(t5)  std::fill_n(t5,  len, vme::NONE);
	if(t6)  std::fill_n(t6,  len, vme::NONE);
	if(t7)  std::fill_n(t7,  len, vme::NONE);
	if(t8)  std::fill_n(t8,  len, vme::NONE);
	if(t9)  std::fill_n(t9,  len, vme::NONE);
	if(t10) std::fill_n(t10, len, vme::NONE);
}

} // namespace vme


#endif
