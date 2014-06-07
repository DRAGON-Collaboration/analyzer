/********************************************************************\

   Name:         strlcpy.h
   Created by:   Stefan Ritt

   Contents:     Header file for strlcpy.c

\********************************************************************/

#ifndef _STRLCPY_H_
#define _STRLCPY_H_

// some version of gcc have a built-in strlcpy
#ifdef strlcpy
#define STRLCPY_DEFINED
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EXPRT
#if defined(EXPORT_DLL)
#define EXPRT __declspec(dllexport)
#else
#define EXPRT
#endif
#endif

#ifndef STRLCPY_DEFINED
size_t EXPRT strlcpy(char *dst, const char *src, size_t size);
size_t EXPRT strlcat(char *dst, const char *src, size_t size);
#endif

#ifdef __cplusplus
}
#endif

#endif /*_STRLCPY_H_ */
