//
// Access ODB via mhttpd http requests
//
// Name: HttpOdb.h
// Author: K.Olchanski, 12-Nov-2008
//
// $Id$
//

#ifndef INCLUDE_HttpOdb_H
#define INCLUDE_HttpOdb_H

#include "VirtualOdb.h"

///
/// To enable ODB access in mhttpd, create an ODB string entry "/Custom/secret.html!" with blank value.
///
/// Then create an HttpOdb class with URL "http://hostname:port/CS/secret.html" and use the odb access functions as required.
///
/// Note that HTTP access is not optimized for speed. It can be slow, can result in high CPU utilization by mhttpd and is subject to lengthy delays if mhttpd is busy with something else (i.e. starting a run or generating history plots).
///
/// In this usage, the string "secret.html" functions as an access password.
///

/// Access to ODB through the MIDAS HTTP server mhttpd

class HttpOdb : public VirtualOdb
{
 protected:
  const char* fUrl;
  int fDebug;

 public:
  /// Contructor from a base URL
  HttpOdb(const char* url);

  /// Destructor
  virtual ~HttpOdb();

  int      odbReadAny(   const char*name, int index, int tid,void* buf, int bufsize=0);
  int      odbReadInt(   const char*name, int index, int      defaultValue);
  uint32_t odbReadUint32(const char*name, int index, uint32_t defaultValue);
  bool     odbReadBool(  const char*name, int index, bool     defaultValue);
  double   odbReadDouble(const char*name, int index, double   defaultValue);
  float    odbReadFloat(const char*name, int index, float    defaultValue);
  const char* odbReadString(const char*name, int index, const char* defaultValue);
  int      odbReadArraySize(const char*name);

 protected:
  const char* jkey(const char* path);
  const char* jget(const char* path, int index);

 protected:
  char fRequestBuf[1024];
  char fReplyBuf[1024];
};

#endif
//end
