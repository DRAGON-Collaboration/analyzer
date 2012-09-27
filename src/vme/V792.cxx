//! \file V792.cxx
//! \author G. Chritian
//! \brief Implements Adc.hxx
#include "utils/Bits.hxx"
#include "midas/Event.hxx"
#include "V792.hxx"



void vme::V792::reset()
{
	n_ch  = 0;
	count = 0;
	overflow = false;
	underflow = false;
	reset_array(MAX_CHANNELS, data);
}

bool vme::V792::unpack_data_buffer(const uint32_t* const pbuffer)
{
	/*!
	 * \param [in] pbuffer Pointer to the data buffer word.
	 *
	 * A data buffer encodes the conversion value (i.e. integrated charge or peak pulse height)
	 * for a single ADC channel. See below for bitpacking instructions.
	 */
	overflow     = (*pbuffer >> 12) & READ1; /// Bit 12 is an overflow tag
	underflow    = (*pbuffer >> 13) & READ1; /// Bit 13 is an underflow tag
	uint16_t ch  = (*pbuffer >> 16) & READ5; /// Bits 16-20 tell the channel number of the conversion
	if (ch >= MAX_CHANNELS) {
		err::Error("vme::V792::unpack_data_buffer")
			<< ERR_FILE_LINE << "Read a channel number (" << ch
			<< ") which is >= the maximum (" << MAX_CHANNELS << "). Skipping...\n";
		return false;
	}
	data[ch]  = (*pbuffer >> 0) & READ12; /// Bits 0 - 11 encode the converted value
	return true;
}

bool vme::V792::unpack_buffer(const uint32_t* const pbuffer, const char* bankName)
{
  /*!
	 * \param [in] pbuffer Pointer to the buffer word
	 * \param [in] bank Name of the midas bank being unpacked
	 * \returns True if the buffer was successfully unpacked, false otherwise
	 *
	 * V792 buffers are 32 bit words. Bits 24-26 specify the type of data contained
	 * in the buffer. In this function, we read the buffer type and then handle appropriately.
	 */
	bool success = true;
	uint32_t type = (*pbuffer >> 24) & READ3;

	switch (type) {
	case DATA_BITS:    /// case DATA_BITS : See unpack_data_buffer() 
		success = unpack_data_buffer(pbuffer);
		break;
	case HEADER_BITS:  /// case HEADER_BITS: read number of channels (n_ch) in the event from bits 6 - 13
		n_ch  = (*pbuffer >> 6) & READ8;
		break;
	case FOOTER_BITS:  /// case FOOTER_BITS: read event counter (count) from bits 0 - 23
		count = (*pbuffer >> 0) & READ24;
		break;
	case INVALID_BITS: /// case INVALID_BITS: bail out
		err::Error("vme::V792::unpack_buffer")
			<< ERR_FILE_LINE << "Bank name: \"" << bankName
			<< "\": Read INVALID_BITS code from a CAEN ADC output buffer. Skipping...\n";
		success = false;
		break;
	default: /// Bail out if we read an unknown buffer code
		err::Error("vme::V792::unpack_buffer")
			<< ERR_FILE_LINE << "Bank name: \"" << bankName
			<< "\": Unknown ADC buffer code: 0x" << std::hex << type << ". Skipping...\n";
		success = false;
		break;
	}
	return success;
}

bool vme::V792::unpack(const midas::Event& event, const char* bankName, bool reportMissing)
{
	/*!
	 * Searches for a bank tagged by \e bankName and then proceeds to loop over the data contained
	 * in the bank and extract into the appropriate class data fields.
	 *
	 * \param [in] event The midas event to unpack
   * \param [in] bankName Name of the bank to unpack
   * \param [in] reportMissing False specifies to silently return if \e bankName isn't found in
	 *             the event. True specifies to print a warning message if this is the case.
   * \returns True if the event was successfully unpacked, false otherwise
	 */
  int bank_len;
	uint32_t* pbank32 =
		event.GetBankPointer<uint32_t>(bankName, &bank_len, reportMissing, true);

  // Loop over all data words in the bank
	bool ret = true;
  for (int i=0; i< bank_len; ++i) {
    bool success = unpack_buffer(pbank32++, bankName);
		if(!success) ret = false;
  }
  return ret;
}
