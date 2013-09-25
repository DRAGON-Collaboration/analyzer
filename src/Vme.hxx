///
/// \file Vme.hxx
/// \brief Defines structures and routines for vme modules
/// used in the DRAGON experiment.
///
#ifndef HAVE_DRAGON_VME_HXX
#define HAVE_DRAGON_VME_HXX
#include <stdint.h>
#include <vector>
#include "utils/Valid.hxx"
#include "utils/ErrorDragon.hxx"


/// Sets the maximum number of recorded TDC hits in a single event
#define DRAGON_TDC_MAX_HITS 5

namespace midas { class Event; }


/// Encloses all VME module classes
namespace vme {

///
/// IO32 FPGA
///
class Io32 {
public: // Methods
	/// Calls reset()
	Io32();
	/// Unpack all data from the io32 main bank
	bool unpack(const midas::Event& event, const char* bankName, bool reportMissing = false);
	/// Set all data fields to default values (== 0).
	void reset();

public: // Data
	/// Header and version
	uint32_t header;
	/// Event number, counting from 0
	uint32_t trig_count;
	/// Trigger timestamp
	uint32_t tstamp;
	/// Readout start time
	uint32_t start;
	/// Readout end time
	uint32_t end;
	/// Trigger latency
	uint32_t latency;
	/// Readout elapsed time
	uint32_t read_time;
	/// Busy elapsed time
	uint32_t busy_time;
	/// Dragon trigger latch code
	/*! Bitmask defining which signal generated the trigger */
	uint32_t trigger_latch;
	/// Which ECL input made the trigger?
	/*! Integer version of trigger_latch */
	uint32_t which_trigger;

public: // Subclasses
	/// Encloses TSC4 data
	struct Tsc4 {
		/// Number of events in each FIFO channel
		int n_fifo[4];
		/// TSC FIFO data
		std::vector<uint64_t> fifo[4]; //!
		/// Trigger time in usec
		double trig_time;
	};

public: // Subclass instances
	/// TSC4 data
	Tsc4 tsc4;
};


///
/// CAEN V1190 TDC module
///
class V1190 {
public: // Constants
	/// Header buffer code
	static const uint16_t TDC_HEADER            = 0x1;
	/// Measurement (data) buffer code
	static const uint16_t TDC_MEASUREMENT       = 0x0;
	/// Error buffer code
	static const uint16_t TDC_ERROR             = 0x4;
	/// Trailer buffer code
	static const uint16_t TDC_TRAILER           = 0x3;
	/// Global header buffer code
	static const uint16_t GLOBAL_HEADER         = 0x8;
	/// Global trailer buffer code
	static const uint16_t GLOBAL_TRAILER        = 0x10;
  /// Extended trigger time buffer code
	static const uint16_t EXTENDED_TRIGGER_TIME = 0x11;
	/// Number of data channels available in the TDC
	static const uint16_t MAX_CHANNELS          = 64;

public: // Methods
	/// Calls reset()
	V1190();
	/// Unpack TDC data from a MIDAS event
	bool unpack(const midas::Event& event, const char* bankName, bool reportMissing = false);
	/// Reset data fields to default values
	void reset();
	/// Get a data value, with bounds checking
	int32_t get_data(int16_t ch) const;
	/// Find leading edge hit
	int32_t get_leading(int16_t ch, int16_t hit) const;
	/// Find trailing edge hit
	int32_t get_trailing(int16_t ch, int16_t hit) const;

public: // Data
  /// Number of channels present in an event
	int16_t n_ch;
  /// Event counter
	int32_t count;
	/// Word count
	int16_t word_count;
	/// Global trailer word count
	int16_t trailer_word_count;
  /// Event id
	int16_t event_id;
	/// Bunch id
	int16_t bunch_id;
	/// Module status
	int16_t status;
	/// \brief Measurement type
	/// \details 0 = leading, 1 = trailing
	int16_t type;
	/// Extended trigger time
	int32_t extended_trigger;

private: // Internal routines
  /// Unpack a generic V1190 buffer
	bool unpack_buffer(const uint32_t* const pbuffer, const char* bankName);
  /// Unpack footer buffer
	void unpack_footer_buffer(const uint32_t* const pbuffer, const char* bankName);
  /// Deals with V1190 error codes
	void handle_error_buffer(const uint32_t* const pbuffer, const char* bankName);
	/// Unpack event data buffer
	bool unpack_data_buffer(const uint32_t* const pbuffer);

public: // Subclasses
/// Encloses measurement data for a single v1190 TDC channel
	struct Channel {
    /// Number of leading-edge hits in the current event
		int32_t nleading;
    /// Number of trailing-edge hits in the current event
		int32_t ntrailing;
		/// Temporary storage of leading edge hits
		std::vector<int32_t> fLeading;  //!
		/// Temporary storage of trailing edge hits
		std::vector<int32_t> fTrailing; //!
	};


	/// Holds measurement information in a first-in-first-out structure
	/*!
	 * \todo Explain reason for using this as the transient way of storing data
	 *  (TTree::Draw doesn't handle arrays of vectors, apparently).
	 */
	class Fifo {
 public:
		/// Append another measurement
		void push_back(int32_t measurement_, int16_t channel_, int16_t number_);
		/// Clear all vectors
		void clear();
 public:
		/// FIFO measurement values
		std::vector<uint32_t> measurement;
		/// FIFO channel numbers
		std::vector<uint16_t> channel;
		/// FIFO numner of mesurement per `channel`
		std::vector<uint16_t> number;
	};
	
public: // Subclass instances
	/// Array of all measurement channels (not transient)
	Channel channel[MAX_CHANNELS]; //!
	/// Leading edge measurements
	Fifo fifo0;
	/// Trailing edge measurements
	Fifo fifo1;
};

///
/// CAEN v792 ADC
///
class V792 {
public: // Constants
	/// Code to specify a data buffer
	static const uint16_t DATA_BITS    = 0x0;
	/// Code to specify a header buffer
	static const uint16_t HEADER_BITS  = 0x2;
	/// Code to specify a footer buffer
	static const uint16_t FOOTER_BITS  = 0x4;
  /// Code to specify an invalid buffer
	static const uint16_t INVALID_BITS = 0x6;
 	/// Number of data channels availible in the ADC
	static const uint16_t MAX_CHANNELS = 32;

public: // Methods
	/// Calls reset(),
	V792();
	/// Unpack ADC data from a midas event
	bool unpack(const midas::Event& event, const char* bankName, bool reportMissing = false);
	/// Reset data fields to default values
	void reset();
	/// Get a data value, with bounds checking
	int32_t get_data(int16_t ch) const;

public: // Class data
  /// Number of channels present in an event
	int16_t n_ch;
  /// Event counter
	int32_t count;
  /// Is any channel an overflow?
	bool overflow;
	/// Is any channel under threshold?
	bool underflow;
	/// Array of event data
	int16_t data[MAX_CHANNELS];

private:
  /// Unpack event data from a caen 32 channel adc
	bool unpack_data_buffer(const uint32_t* const pbuffer);
  /// Unpack a Midas data buffer from a CAEN ADC
	bool unpack_buffer(const uint32_t* const pbuffer, const char* bankName);
};


/// Alias V785 to V792
/*! The readout structure of CAEN v792 and v785 ADCs is identical; thus the
 *  same unpacker code can be used for each. Provide a typedef of V792->V785
 *  for naming consistency between hardware and software. */
typedef V792 V785;

} // namespace vme


#endif // #ifndef HAVE_DRAGON_VME_HXX
