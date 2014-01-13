///
/// \file IntTypes.h
/// \brief File to make stdint.h play nice with both CINT and the new OS/X 10.9
///
#ifndef HAVE_INTTYPES_HXX
#define HAVE_INTTYPES_HXX

#ifndef __MAKECINT__ // Not in CINT, use system stdint.h

#include <stdint.h>


#else // In CINT, use stdint types from ROOT's types instead

#include <Rtypes.h>

typedef Char_t   int8_t;
typedef Short_t  int16_t;
typedef Int_t    int32_t;
typedef Long64_t int64_t;

typedef UChar_t   uint8_t;
typedef UShort_t  uint16_t;
typedef UInt_t    uint32_t;
typedef ULong64_t uint64_t;
 

#endif // #ifndef __MAKECINT__



#endif // #include guard
