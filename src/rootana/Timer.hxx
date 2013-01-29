///\file Timer.hxx
///\author K. Olchanski
///\author G. Christian
///\brief Defines a TTimer class for use in rootana
#ifndef ROOTANA_TIMER_HXX
#define ROOTANA_TIMER_HXX
#include <sys/time.h>
#include <cassert>
#include <TSystem.h>
#include <TTimer.h>

#ifdef MIDASSYS
#ifndef __MAKECINT__
#include "midas.h"
#endif
#include "IncludeMidasOnline.h"
#endif

namespace rootana {

/// TTimer class for rootana event loop
class Timer: public TTimer {
private:
  int          fPeriod_msec; ///< Period in milliseconds
  double       fLastTime;    ///< Time of last callback

public:
	/// Sets period and callback function
	Timer(int period_msec);
	/// Calls callback function
  Bool_t Notify();
	/// Function to execute perioducally from within Notify()
	virtual void PeriodicAction();
	/// Calls TTimer::TurnOff()
	~Timer();

private:
	/// Get time of day in seconds
	double GetTimeSec(); 
};

}

inline rootana::Timer::Timer(int period_msec):
	fPeriod_msec(period_msec)
{
	fLastTime = GetTimeSec();
	Start(period_msec,kTRUE);
}

inline rootana::Timer::~Timer()
{
	TurnOff();
}

inline Bool_t rootana::Timer::Notify()
{
	double t = GetTimeSec();
	
	if (t - fLastTime >= 0.9*fPeriod_msec*0.001) {
		PeriodicAction();
		fLastTime = t;
	}
	Reset();
	return true;
}

inline double	rootana::Timer::GetTimeSec()
{
  struct timeval tv;
  gettimeofday(&tv,NULL);
  return tv.tv_sec + 0.000001*tv.tv_usec;
}


#ifdef MIDASSYS

inline void rootana::Timer::PeriodicAction()
{
	/*!
	 * Call TMidasOnline::poll() to request events; also check for
	 * manual shutdown via '!' on the command line.
	 */
	char c = ss_getchar(0);
  if (!(TMidasOnline::instance()->poll(10)) || c == '!') {
		ss_getchar(1);
    gSystem->ExitLoop();
	}
}

#else

inline void rootana::Timer::PeriodicAction() { }

#endif


#endif
