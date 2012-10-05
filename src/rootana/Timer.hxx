///\file Periodic.hxx
///\author K. Olchanski
///\author G. Christian
///\brief Defines a TTimer class for use in rootana
#ifndef ROOTANA_TIMER_HXX
#define ROOTANA_TIMER_HXX
#include <sys/time.h>
#include <cassert>
#include <TTimer.h>

namespace rootana {

/// TTimer class for rootana event loop
class Timer: public TTimer {

public:
  typedef void (*TimerHandler)(void); ///< Callback type

private:
  int          fPeriod_msec; ///< Period in milliseconds
  TimerHandler fHandler;     ///< Periodic callback routine
  double       fLastTime;    ///< Time of last callback

public:
	/// Sets period and callback function
	Timer(int period_msec, TimerHandler handler);
	/// Calls callback function
  Bool_t Notify();
	/// Calls TTimer::TurnOff()
	~Timer();

private:
	/// Get time of day in seconds
	double GetTimeSec(); 
};

}

inline rootana::Timer::Timer(int period_msec, TimerHandler handler):
	fPeriod_msec(period_msec), fHandler(handler)
{
	assert(handler != NULL);
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
		if (fHandler)
			(*fHandler)();
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


#endif
