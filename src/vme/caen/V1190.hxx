/// \file V1190.hxx
/// \brief Defines v1190b tdc module structures and routines
#include <stdint.h>
#include "midas/TMidasEvent.h"
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
	 /// Extended trigger time
	 int32_t extended_trigger;
	 /// Event counter
	 int32_t count;
	 /// Word count
	 int16_t word_count;
	 /// Global trailer Word count
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
};

/// Unpack event data from a caen v1190b TDC
/// \param [in] data longword
/// \param [out] module struct you're unpacking into
/// \throws std::invalid_argument Throws if an invalid channel number is read
void unpack_v1190_data(uint32_t data, V1190b& module);

/// Unpack header data from a caen v1190b TDC
/// \param [in] data longword
/// \param [out] module struct you're unpacking into
/// \note Header contains the geographic address (GEO), crate number, and
///  number of channels with data. GEO and crate are unused with Midas, so
///  we'll ignore them and just set the number of channels.
void unpack_v1190_header(uint32_t data, V1190b& module);

/// Unpack global header data from a caen v1190b TDC
/// \param [in] data longword
/// \param [out] module struct you're unpacking into
/// \note Header contains the geographic address (GEO), crate number, and
///  number of channels with data. GEO and crate are unused with Midas, so
///  we'll ignore them and just set the number of channels.
void unpack_v1190_global_header(uint32_t data, V1190b& module);

/// Unpack footer data from a caen v1190b TDC
/// \param [in] data longword
/// \param [out] module struct you're unpacking into
/// \note Footer contains the GEO and event counter, read the event counter only.
void unpack_v1190_footer(uint32_t data, V1190b& module);

/// Unpack global footer data from a caen v1190b TDC
/// \param [in] data longword
/// \param [out] module struct you're unpacking into
/// \note Footer contains the GEO and event counter, read the event counter only.
void unpack_v1190_global_footer(uint32_t data, V1190b& module);

/// Handle an Extended trigger time packed from a caen v190b TDC
/// \param [in] data longword
/// \param [out] module struct you're unpacking into
void handle_v1190_extended_trigger(uint32_t data, V1190b& module);

/// Handle invalid data bits from a caen v1190b TDC
/// \param [in] data longword
/// \param [out] module struct you're unpacking into
void handle_v1190_error(uint32_t data, V1190b& module);

/// Unpack a Midas data buffer from a CAEN v1190b TDC
/// \param [in] address Memory address of the buffer
/// \param [in] bank Name of the midas bank being unpacked
/// \param [out] module Module into which the data is unpacked
/// \returns True if the buffer was successfully unpacked, false otherwise
bool unpack_v1190_buffer(void* address, const char* bank, V1190b& module);

/// Unpack a midas event from a CAEN ADC
/// \param [in] event The midas event to unpack
/// \param [in] bank Name of the bank to unpack
/// \param [out] module The module into which you are unpacking
/// \returns True if the event was successfully unpacked, false otherwise
bool unpack_v1190(const TMidasEvent& event, const char* bank, V1190b& module);


} // namespace caen
                 
} // namespace vme



#endif
