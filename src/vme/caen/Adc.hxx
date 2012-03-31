//! \file Adc.hxx
//! \brief Defines classes relevant to unpacking data from CAEN ADC modules.
#ifndef  DRAGON_VME_ADC_HXX
#define  DRAGON_VME_ADC_HXX
#include "midas/TMidasEvent.h"

namespace vme {

namespace caen {

/// \brief Generic ADC module (covers caen v792 or caen v785)
/// \tparam N Number of channels in the module
template <int N>
struct Adc {
   /// Number of channels present in an event
	 int16_t n_present;
	 /// Array of data
	 int16_t data[N];
	 /// Event counter
	 int32_t count;
	 /// Is any channel under threshold?
	 bool underflow;
	 /// Is any channel an overflow?
	 bool overflow;
};
/// 32 channel integrating ADC
typedef vme::caen::Adc<32> V792;

/// 32 channel peak-sensing ADC
typedef vme::caen::Adc<32> V785;

/// Unpack event data from a caen 32 channel adc
/// \param [in] data longword
/// \param [out] module struct you're unpacking into
/// \throws std::invalid_argument Throws if an invalid channel number is read
void unpack_adc_data(uint32_t data, Adc<32>& module);

/// Unpack header data from a caen 32 channel adc
/// \param [in] data longword
/// \param [out] module struct you're unpacking into
/// \note Header contains the geographic address (GEO), crate number, and
///  number of channels with data. GEO and crate are unused with Midas, so
///  we'll ignore them and just set the number of channels.
void unpack_adc_header(uint32_t data, Adc<32>& module);

/// Unpack footer data from a caen 32 channel adc
/// \param [in] data longword
/// \param [out] module struct you're unpacking into
/// \note Footer contains the GEO and event counter, read the event counter only.
void unpack_adc_footer(uint32_t data, Adc<32>& module);

/// Handle invalid data bits from an adc
/// \param [in] data longword
/// \param [out] module struct you're unpacking into
void handle_adc_invalid(uint32_t data, Adc<32>& module);

/// Unpack a Midas data buffer from a CAEN ADC
/// \param [in] address Memory address of the buffer
/// \param [in] bank Name of the midas bank being unpacked
/// \param [out] module Module into which the data is unpacked
/// \returns True if the buffer was successfully unpacked, false otherwise
bool unpack_adc_buffer(void* address, const char* bank, vme::caen::Adc<32>& module);

/// Unpack a midas event from a CAEN ADC
/// \param [in] event The midas event to unpack
/// \param [in] bank Name of the bank to unpack
/// \param [out] module The module into which you are unpacking
/// \returns True if the event was successfully unpacked, false otherwise
bool unpack_adc(const TMidasEvent& event, const char* bank, vme::caen::Adc<32>& module);


} // namespace caen

} // namespace vme




#endif
