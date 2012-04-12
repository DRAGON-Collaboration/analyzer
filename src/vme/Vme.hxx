//! \file Vme.hxx
//! \brief Defines classes relevant to unpacking data from VME modules.
#ifndef  DRAGON_VME_HXX
#define  DRAGON_VME_HXX
#include <stdint.h>
#include <cstring>

/// Encloses all VME related classes
namespace vme {

/// "Empty" code used when no data is present in a channel.
#ifdef VME_NO_DATA
const int16_t NONE = VME_NO_DATA;
#else
const int16_t NONE = -1;
#endif

#ifndef __MAKECINT__
/// Reset a VME module data to default values (vme::NONE).
/// \tparam T The module type being reset. Must have an array called "data" with N entries
/// \tparam N The template argument of \e T
/// \param [out] module The module you want to reset
template <template <int N_CH> class T, int N>
void reset(T<N>& module)
{
	for(int i=0; i< N; ++i)
		 module.data[i] = vme::NONE;
}

/// Reset a VME module data to default values (vme::NONE).
/// \tparam T The module type being reset. Must have an array called "data" with N entries
/// \tparam N The template argument of \e T
/// \param [out] module The module you want to reset
template <class T, int N>
void reset(T& module)
{
	for(int i=0; i< N; ++i)
		 module.data[i] = vme::NONE;
}

/// Copy VME module data to an external array
/// \tparam T The module type being reset. Must have an array called "data" with N entries
/// \tparam N The template argument of \e T
/// \param [in] module Module from which you are copying data
/// \param [out] destination Array to which you are copying the data
/// \warning No bounds checking done; \e destination must point to an array of length >= \e N
template <template <int N_CH> class T, int N>
void copy_data(const T<N>& module, int16_t* destination)
{
  int nbytes = N * sizeof(int16_t);
  memcpy(reinterpret_cast<void*>(destination), reinterpret_cast<const void*>(module.data), nbytes);
}
#endif // #ifndef __MAKECINT__

}

#endif
