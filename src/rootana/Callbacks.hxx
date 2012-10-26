/*!
 * \file Callbacks.hxx
 * \author G. Christian
 * \brief Defines callback functions for rootana analyzer
 */
#ifndef ROOTANA_CALLBACKS_HXX
#define ROOTANA_CALLBACKS_HXX
#include "utils/definitions.h"
#include "utils/Error.hxx"
#include "midas/Event.hxx"
#include "Application.hxx"

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


// Inlined implementations

inline void rootana_run_start(int transition, int run, int time)
{
	/*!
	 *  Calls rootana::App::run_start()
	 */
	rootana::App::instance()->run_start(run);
}

inline void rootana_run_stop(int transition, int run, int time)
{
	/*!
	 *  Calls rootana::App::run_stop()
	 */
	rootana::App::instance()->run_stop(run);
}

inline void rootana_run_pause(int transition, int run, int time)
{
	/*!
	 * Prints an information message.
	 */
	utils::err::Info("rootana") << "Pausing run " << run;
}

inline void rootana_run_resume(int transition, int run, int time)
{
	/*!
	 * Prints an information message.
	 */
	utils::err::Info("rootana") << "Resuming run " << run;
}

inline void rootana_handle_event(const void* pheader, const void* pdata, int size)
{
	/*!
	 * Figure out the TSC bank name from event id, then pass on the work to
	 * rootana::App::handle_event().
	 */
	const midas::Event::Header* head = reinterpret_cast<const midas::Event::Header*>(pheader);
	char tscBk[5];
	if (head->fEventId == DRAGON_TAIL_EVENT)
		strcpy(tscBk, "TSCT");
	else 
		strcpy(tscBk, "TSCH");

	midas::Event e(tscBk, pheader, pdata, head->fDataSize);
	rootana::App::instance()->handle_event(e);
}


#endif
