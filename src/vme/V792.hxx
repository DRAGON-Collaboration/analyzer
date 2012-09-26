//! \file V792.hxx
//! \author G. Christian
//! \brief Defines classes relevant to unpacking data from CAEN ADC modules V792 and V785.
#ifndef  DRAGON_VME_ADC_HXX
#define  DRAGON_VME_ADC_HXX
#include <stdint.h>


namespace dragon { class MidasEvent; }


namespace vme {

/// Encloses CAEN vme module classes
namespace caen {

/// CAEN v792 ADC
class V792 {
public:
	static const uint16_t DATA_BITS    = 0x0; ///< Code to specify a data buffer
	static const uint16_t HEADER_BITS  = 0x2; ///< Code to specify a header buffer
	static const uint16_t FOOTER_BITS  = 0x4; ///< Code to specify a footer buffer
	static const uint16_t INVALID_BITS = 0x6; ///< Code to specify an invalid buffer
	static const uint32_t MAX_CHANNELS = 32; 	///< Number of data channels availible in the ADC

	int16_t n_ch;	  ///< Number of channels present in an event

	int32_t count;  ///< Event counter

	bool overflow; 	///< Is any channel an overflow?

	bool underflow; ///< Is any channel under threshold?

	int16_t data[MAX_CHANNELS]; ///< Array of event data

public:
	/// Unpack ADC data from a midas event
	bool unpack(const dragon::MidasEvent& event, const char* bankName, bool reportMissing = false);

	/// Reset data fields to default values
	void reset();

	/// Calls reset(),
	V792() { reset(); }

	/// Empty
	~V792() { }

private:
  /// Unpack event data from a caen 32 channel adc
	bool unpack_data_buffer(const uint32_t* const pbuffer);

  /// Unpack a Midas data buffer from a CAEN ADC
	bool unpack_buffer(const uint32_t* const pbuffer, const char* bankName);

}; // struct V792


/// Readout structure of CAEN V785 ADC (peak sensing) is identical to V792 QDC (charge sensing).
typedef V792 V785;

} // namespace caen

} // namespace vme




#endif
