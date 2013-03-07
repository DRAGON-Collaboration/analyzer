#ifndef TMidasOnline_hxx_seen
#define TMidasOnline_hxx_seen
/********************************************************************\

  Name:         TMidasOnline.h
  Created by:   Konstantin Olchanski - TRIUMF

  Contents:     C++ MIDAS analyzer

  $Id$

\********************************************************************/


#include <stdio.h>
#include <string>

#include "VirtualOdb.h"

/// Request events from online shared memory data buffer

struct EventRequest
{
public:
  EventRequest* fNext;          ///< (internal use) list of all requests
  std::string   fBufferName;    ///< name of the midas data buffer, e.g. "SYSTEM"
  int           fBufferHandle;  ///< buffer handle from bm_open_buffer()
  int           fEventId;       ///< request event ID
  int           fTriggerMask;   ///< request trigger mask
  int           fSamplingType;  ///< sampling type
  int           fRequestId;     ///< request ID assigned by midas
};

/// MIDAS online connection, including access to online ODB

class TMidasOnline : public VirtualOdb
{
public:

  /// User handler for run state transition events
  typedef void (*TransitionHandler)(int transition,int run_number,int trans_time);

  /// User handler for data events
  typedef void (*EventHandler)(const void*header,const void*data,int length);

public:

  std::string fHostname; ///< hostname where the mserver is running, blank if using shared memory
  std::string fExptname; ///< experiment name, blank if only one experiment defined in exptab

  int  fDB; ///< ODB handle

  TransitionHandler fStartHandler;
  TransitionHandler fStopHandler;
  TransitionHandler fPauseHandler;
  TransitionHandler fResumeHandler;

  EventRequest*     fEventRequests;
  EventHandler      fEventHandler;

private:
  /// TMidasOnline is a singleton class: only one
  /// instance is allowed at any time
  static TMidasOnline* gfMidas;

  TMidasOnline(); ///< default constructor is private for singleton classes
  virtual ~TMidasOnline(); ///< destructor is private for singleton classes

public:

  /// TMidasOnline is a singleton class. Call instance() to get a reference
  /// to the one instance of this class.
  static TMidasOnline* instance();

  /// Connect to MIDAS experiment
  int connect(const char*hostname,const char*exptname,const char*progname);

  /// Disconnect from MIDAS
  int disconnect();

  /// Check for MIDAS events (run transitions, data requests)
  bool poll(int mdelay);

  // run transitions functions

  /// Ask MIDAS to tell us about run transitions
  void registerTransitions();

  /// Specify user handlers for run transitions
  void setTransitionHandlers(TransitionHandler start,TransitionHandler stop,TransitionHandler pause,TransitionHandler resume);

  /// Check for pending transitions, call user handlers. Returns "true" if there were transitions.
  bool checkTransitions();

  // event request functions

  /// Specify user handler for data events
  void setEventHandler(EventHandler handler);

  /// Request data for delivery via callback (setEventHandler) or by polling (via receiveEvent)
  int eventRequest(const char* bufferName,int eventId,int triggerMask,int samplingType,bool poll=false);

  /// Delete data request
  void deleteEventRequest(int requestId);

  /// Receive event by polling
  int receiveEvent(int requestId, void* pevent, int size, bool async);

  // ODB functions required by VirtualOdb

  int      odbReadArraySize(const char*name);
  int      odbReadAny(   const char*name, int index, int tid,void* buf, int bufsize = 0);
  int      odbReadInt(   const char*name, int index = 0, int      defaultValue = 0);
  uint32_t odbReadUint32(const char*name, int index = 0, uint32_t defaultValue = 0);
  double   odbReadDouble(const char*name, int index = 0, double   defaultValue = 0);
  float    odbReadFloat(const char*name, int index = 0, float   defaultValue = 0);
  bool     odbReadBool(  const char*name, int index = 0, bool     defaultValue = false);
  const char* odbReadString(const char*name, int index = 0,const char* defaultValue = NULL);
};

//end
#endif
