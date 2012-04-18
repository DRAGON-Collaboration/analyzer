/// \file copy_array.h
/// \brief Defines a template function to copy arrays using memcpy
#ifndef DRAGON_COPY_ARRAY_H
#define DRAGON_COPY_ARRAY_H
#include <string.h>

namespace {
template<class T> inline void copy_array(const T* from, T* to, int length)
{
	memcpy((void*)to, (void*)from, length * sizeof(T) / sizeof(char));
} }


#endif
