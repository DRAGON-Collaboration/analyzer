/// \file V1190.hxx
/// \author G. Christian
/// \brief Defines caen V1190b tdc class
#ifndef DRAGON_VME_V1190_HXX
#define DRAGON_VME_V1190_HXX
#include <stdint.h>
#include "utils/Error.hxx"
#include "utils/Valid.hxx"


namespace midas { class Event; }

namespace vme {

/// \brief CAEN V1190 TDC module
class V1190 {
public:
	static const uint16_t TDC_HEADER            = 0x1;  ///< Header buffer code
	static const uint16_t TDC_MEASUREMENT       = 0x0;  ///< Measurement (data) buffer code
	static const uint16_t TDC_ERROR             = 0x4;  ///< Error buffer code
	static const uint16_t TDC_TRAILER           = 0x3;  ///< Trailer buffer code
	static const uint16_t GLOBAL_HEADER         = 0x8;  ///< Global header buffer code
	static const uint16_t GLOBAL_TRAILER        = 0x10; ///< Global trailer buffer code
	static const uint16_t EXTENDED_TRIGGER_TIME = 0x11; ///< Extended trigger time buffer code
	static const uint16_t MAX_CHANNELS = 64;            ///< Number of data channels available in the TDC
	static const uint16_t MAX_HITS     = 5;             ///< Maximum number of hits (head + tail) in a channel

	int16_t n_ch;               ///< Number of channels present in an event

	int32_t count;              ///< Event counter
	
	int16_t word_count;         ///< Word count
	
	int16_t trailer_word_count; ///< Global trailer word count
	
	int16_t event_id;           ///< Event id
	
	int16_t bunch_id;           ///< Bunch id
	
	int16_t status;             ///< Module status
	
	/// \details 0 = leading, 1 = trailing
	int16_t type;               ///< Measurement type

	/// \todo Explore other options for handling multiple hits
	int32_t leading_edge[MAX_HITS][MAX_CHANNELS];  ///< Leading edge event data
	
	int32_t trailing_edge[MAX_HITS][MAX_CHANNELS]; ///< Trailing edge event data

	int16_t nleading[MAX_CHANNELS];   ///< Number of leading-edge measurements

	int16_t ntrailing[MAX_CHANNELS];  ///< Number of trailing-edge measurements

	int32_t extended_trigger;   ///< Extended trigger time

public:
	/// Unpack TDC data from a MIDAS event
	bool unpack(const midas::Event& event, const char* bankName, bool reportMissing = false);
	
	/// Reset data fields to default values
	void reset();

	/// Get a data value, with bounds checking
	int16_t get_data(int16_t ch) const
		{
			/*!
			 * \param ch Channel number to get data from
			 * \returns the leading edge time value of the first hit on
			 * channel \e ch. If \e ch is out of bounds, prints a warning
			 * message and returns dragon::NO_DATA.
			 */
			if (ch >= 0 && ch < MAX_CHANNELS) {
				return leading_edge[0][ch];
			}
			else {
				dragon::err::Warning("V1190::get_data")
					<< "Channel number " << ch << " out of bounds (valid range: [0, "
					<< MAX_CHANNELS -1 << "]\n";
				return dragon::NO_DATA;
			}
		}

	/// Calls reset()
	V1190() { reset(); }

	/// Empty
	~V1190() { }

private:
  /// Unpack a generic V1190 buffer
	bool unpack_buffer(const uint32_t* const pbuffer, const char* bankName);

  /// Unpack footer buffer
	void unpack_footer_buffer(const uint32_t* const pbuffer, const char* bankName);

  /// Deals with V1190 error codes
	void handle_error_buffer(const uint32_t* const pbuffer, const char* bankName);

	/// Unpack event data buffer
	bool unpack_data_buffer(const uint32_t* const pbuffer);
};

} // namespace vme



#endif
