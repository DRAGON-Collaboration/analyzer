/*!
 * \file RootanaDragon.hxx
 * \author G. Christian
 * \brief Defines callbacks, helper functions, etc. for rootana
 */
#ifndef ROOTANA_DRAGON_HXX
#define ROOTANA_DRAGON_HXX

// Declare callback functions as extern "C"
#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif

/// Run start transition handler.
EXTERN_C void rootana_run_start(int transition, int run, int time);

/// Run stop transition handler
EXTERN_C void rootana_run_stop(int transition, int run, int time);

/// Run pause transition handler
EXTERN_C void rootana_run_pause(int transition, int run, int time);

/// Run resume transition handler
EXTERN_C void rootana_run_resume(int transition, int run, int time);

/// Event handler callback function
EXTERN_C void rootana_handle_event(const void* pheader, const void* pdata, int size);


namespace rootana {



}



#endif
