/// \file IO32.hxx
/// \brief Defines structures and routines for IO32 FPGA data
#ifndef DRAGON_VME_IO32_HXX
#define DRAGON_VME_IO32_HXX
#include <stdint.h>

namespace vme {

/// \brief IO32 FPGA
struct IO32 {
	 /// Timestamp value
	 int32_t tstamp;
};
                 
} // namespace vme




#endif
