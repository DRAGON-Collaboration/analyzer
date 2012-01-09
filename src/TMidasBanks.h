//
// TMidasBanks.h
//

#ifndef INCLUDE_TMidasBanksH
#define INCLUDE_TMidasBanksH

#include <stdint.h>

// This file defines the data structures written
// into MIDAS .mid files. They define the on-disk
// data format, they cannot be arbitrarily changed.

/// Event header

struct EventHeader_t {
  uint16_t fEventId;      ///< event id
  uint16_t fTriggerMask;  ///< event trigger mask
  uint32_t fSerialNumber; ///< event serial number
  uint32_t fTimeStamp;    ///< event timestamp in seconds
  uint32_t fDataSize;     ///< event size in bytes
};

/// Bank header

struct BankHeader_t {
  uint32_t fDataSize; 
  uint32_t fFlags; 
};

/// 16-bit data bank

struct Bank_t {
  char fName[4];      ///< bank name
  uint16_t fType;     ///< type of data (see midas.h TID_xxx)
  uint16_t fDataSize;
};

/// 32-bit data bank

struct Bank32_t {
  char fName[4];      ///< bank name
  uint32_t fType;     ///< type of data (see midas.h TID_xxx)
  uint32_t fDataSize;
};

#endif
//end
