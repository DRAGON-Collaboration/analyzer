/// \file v1190.hxx
/// \brief Defines v1190b tdc module structures and routines
#ifndef DRAGON_VME_V1190_HXX
#define DRAGON_VME_V1190_HXX

namespace vme {

namespace caen {

/// \brief CAEN V1190b TDC module
struct V1190b {
   /// Number of channels present in an event
	 int16_t n_present;
	 /// Array of data
	 int16_t data[64];
	 /// Event counter
	 int32_t count;
	 /// Is any channel under threshold?
	 bool underflow;
	 /// Is any channel an overflow?
	 bool overflow;
};

} // namespace caen
                 
} // namespace vme



#endif
