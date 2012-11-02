/// \file V1190.hxx
/// \author G. Christian
/// \brief Defines caen V1190b tdc class
#ifndef DRAGON_VME_V1190_HXX
#define DRAGON_VME_V1190_HXX
#include <stdint.h>
#include "utils/Error.hxx"
#include "utils/Valid.hxx"


#define DRAGON_TDC_MAX_HITS 5

namespace midas { class Event; }

namespace vme {

// template <typename DataType, int MAX_HITS> struct TdcMeasurement {
// 	DataType hit[MAX_HITS];
// };

// template <typename DataType, uint16_t MAX_HITS> struct TdcChannel {
// 	DataType leading_edge[MAX_HITS];
// 	DataType trailing_edge[MAX_HITS];
// 	int16_t nleading;
// 	int16_t ntrailing;
// };

/// CAEN V1190 TDC module
class V1190 {
public:
	static const uint16_t TDC_HEADER            = 0x1;    ///< Header buffer code
	static const uint16_t TDC_MEASUREMENT       = 0x0;    ///< Measurement (data) buffer code
	static const uint16_t TDC_ERROR             = 0x4;    ///< Error buffer code
	static const uint16_t TDC_TRAILER           = 0x3;    ///< Trailer buffer code
	static const uint16_t GLOBAL_HEADER         = 0x8;    ///< Global header buffer code
	static const uint16_t GLOBAL_TRAILER        = 0x10;   ///< Global trailer buffer code
	static const uint16_t EXTENDED_TRIGGER_TIME = 0x11;   ///< Extended trigger time buffer code
	static const uint16_t MAX_CHANNELS = 64;              ///< Number of data channels available in the TDC
	static const uint16_t MAX_HITS = DRAGON_TDC_MAX_HITS; ///< Maximum number of hits (head + tail) in a channel

public:
	struct Channel {
		int32_t leading_edge[DRAGON_TDC_MAX_HITS];
		int32_t trailing_edge[DRAGON_TDC_MAX_HITS];
		int16_t nleading;
		int16_t ntrailing;
	};

public:
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
	// vme::TdcMeasurement<int32_t, MAX_HITS> leading_edge[MAX_CHANNELS];

	// vme::TdcMeasurement<int32_t, MAX_HITS> trailing_edge[MAX_CHANNELS];

	// int32_t leading_edge[MAX_HITS][MAX_CHANNELS];  ///< Leading edge event data
	
	// int32_t trailing_edge[MAX_HITS][MAX_CHANNELS]; ///< Trailing edge event data

	// int16_t nleading[MAX_CHANNELS];   ///< Number of valid leading-edge measurements

	// int16_t ntrailing[MAX_CHANNELS];  ///< Number of valid trailing-edge measurements

//	TdcChannel<int32_t, MAX_HITS> channel[MAX_CHANNELS];
	Channel channel[MAX_CHANNELS];

	int32_t extended_trigger;   ///< Extended trigger time
	
//	typedef TdcChannel<int32_t, MAX_HITS> Channel_t;

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
				// return leading_edge[0][ch];
				// return leading_edge[ch].hit[0];
				return channel[ch].leading_edge[0];
			}
			else {
				utils::err::Warning("V1190::get_data")
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


// #ifdef __MAKECINT__
// // #pragma link C++ class vme::TdcMeasurement<int32_t, vme::V1190::MAX_CHANNELS>+;
// #pragma link C++ class vme::TdcChannel<int32_t, vme::V1190::MAX_HITS>+;
// #endif

#endif
