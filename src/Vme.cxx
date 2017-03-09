///
/// \file Vme.cxx
/// \author G. Christian
/// \brief Implements Vme.hxx
///
#include "utils/ErrorDragon.hxx"
#include "utils/Valid.hxx"
#include "utils/Bits.hxx"
#include "midas/Event.hxx"
#include "Vme.hxx"

namespace dutils = dragon::utils;



// ================ Class vme::Io32 ================ //

vme::Io32::Io32()
{
	///
	reset();
}

void vme::Io32::reset()
{
	///
	dutils::reset_data(header, trig_count, tstamp, start, end, latency, read_time,
										 busy_time, trigger_latch, which_trigger, tsc4.trig_time);
	for(int i=0; i< 4; ++i) {
		tsc4.n_fifo[i] = 0;
		tsc4.fifo[i].clear();
	}
}

bool vme::Io32::unpack(const midas::Event& event, const char* bankName, bool reportMissing)
{
	/*! Here is the portion of the MIDAS frontent where values are written to the "main" bank:
	 * \code
	 * *pdata32++ = 0xaaaa0020;           // 0 - header and version
	 * *pdata32++ = trig_count-1;         // 1 - event number, counting from 0
	 * *pdata32++ = trig_time;            // 2 - trigger timestamp
	 * *pdata32++ = start_time;           // 3 - readout start time
	 * *pdata32++ = end_time;             // 4 - readout end time
	 * *pdata32++ = start_time-trig_time; // 5 - trigger latency (trigger time - start of readout time)
	 * *pdata32++ = end_time-start_time;  // 6 - readout elapsed time
	 * *pdata32++ = end_time-trig_time;   // 7 - busy elapsed time
	 * *pdata32++ = trigger_latch         // 8 - dragon trigger latch
	 * \endcode
	 *
	 * The TSC4 bank is already unpacked in a midas::Event, so we can just copy the data over
	 * \param [in] event The midas event to unpack
	 * \param [in] bankName Name of the "main" IO32 bank
	 * \returns True if the event was successfully unpacked, false otherwise
	 */
	int bank_len;
	const int expected_bank_len = 9;
	uint32_t* pdata32 =
		event.GetBankPointer<uint32_t>(bankName, &bank_len, reportMissing, true);

	if (!pdata32) return false;

	if (bank_len != expected_bank_len) {
		dutils::Error("vme::Io32::unpack", __FILE__, __LINE__) <<
			"Bank length: " << bank_len << " != 8, skipping..." << DRAGON_ERR_FILE_LINE;
		return false;
	}

	uint32_t* data_fields[expected_bank_len]
		= { &header, &trig_count, &tstamp, &start, &end,
				&latency, &read_time, &busy_time, &trigger_latch };

	for (int i=0; i< bank_len; ++i) {
		*(data_fields[i]) = *pdata32++;
	}

	// Unpck TSC4 from midas::Event storage
	tsc4.trig_time = event.TriggerTime();
	event.CopyFifo(tsc4.fifo);
	for(int j=0; j< 4; ++j)
		tsc4.n_fifo[j] = tsc4.fifo[j].size();

	return true;
}


// ================ Class vme::V1190 ================ //

vme::V1190::V1190():
	fMessagePeriod(0)
{
	///
	reset();
}

namespace { inline void reset_channel(vme::V1190::Channel* channel)
{
	///
	channel->nleading  = 0;
	channel->ntrailing = 0;
	channel->fLeading.clear();
	channel->fTrailing.clear();
}

template <class T> class hitmatch {
public:
	hitmatch(const T& t, int16_t n):
		fT(t), fN(n) { }
	bool operator()(const T& t)
		{ return t == fT && fN-- == 0; }
private:
	T fT;
	int16_t fN;
};

template <class ch_t, class hit_t, class vch_t, class vmeas_t>
int32_t get_hit_edge(ch_t ch, hit_t hit, const vch_t& vch, const vmeas_t& vmeas) {
	typename vch_t::const_iterator ich =
		std::find_if (vch.begin(), vch.end(), hitmatch<ch_t> (ch, hit));
	return ich != vch.end() ? *(vmeas.begin() + (ich - vch.begin())) : -1;
} }

int32_t vme::V1190::get_leading(int16_t ch, int16_t hit) const
{
	return ::get_hit_edge(ch, hit, fifo0.channel, fifo0.measurement);
}

int32_t vme::V1190::get_trailing(int16_t ch, int16_t hit) const
{
	return ::get_hit_edge(ch, hit, fifo1.channel, fifo1.measurement);
}

void vme::V1190::Fifo::push_back(int32_t measurement_, int16_t channel_, int16_t number_)
{
	///
	measurement.push_back(measurement_);
	channel.push_back(channel_);
	number.push_back(number_);
}

void vme::V1190::Fifo::clear()
{
	///
	measurement.clear();
	channel.clear();
	number.clear();
}

void vme::V1190::reset()
{
	///
	for (int ch = 0; ch < MAX_CHANNELS; ++ch)
		::reset_channel( &(channel[ch]) );

	fifo0.clear();
	fifo1.clear();

	dutils::reset_data
		(type, extended_trigger, n_ch, count, word_count, trailer_word_count,
		 event_id, bunch_id, status, error);
}

int32_t vme::V1190::get_data(int16_t ch) const
{
	/*!
	 * \param ch Channel number to get data from
	 * \returns the leading edge time value of the first hit on
	 * channel \e ch. If \e ch is out of bounds, prints a warning
	 * message and returns dragon::DR_NO_DATA.
	 */

	if (ch >= 0 && ch < MAX_CHANNELS) {
		if(!channel[ch].fLeading.empty())
			return channel[ch].fLeading[0];
		else
			return dragon::NoData<int32_t>::value();
	}
	else {
		dutils::Warning("V1190::get_data")
			<< "Channel number " << ch << " out of bounds (valid range: [0, "
			<< MAX_CHANNELS -1 << "]\n";
		return dragon::NoData<int32_t>::value();
	}
}

bool vme::V1190::unpack_data_buffer(const uint32_t* const pbuffer)
{
	/*!
	 * \param [in] pbuffer Pointer to the data buffer word.
	 *
	 * A data buffer encodes the measurement value (i.e. the pulse time)
	 * for a single TDC measurement. The TDC is multi-hit, so more than one measurement
	 * per channel can be read in a single event.
	 *
	 * See below for bitpacking instructions.
	 */

	type     = (*pbuffer >> 26) & READ1; /// - Bit 26 tells the measurement type (leading or trailing)
	int ch   = (*pbuffer >> 19) & READ7; /// - Bits 19-25 tell the channel number
	if (ch >= MAX_CHANNELS) {
		dutils::Error("vme::V1190::unpack_data_buffer", __FILE__, __LINE__)
			<< DRAGON_ERR_FILE_LINE << "Read a channel number (" << ch
			<< ") which is >= the maximum (" << MAX_CHANNELS << "). Skipping...\n";
		return false;
	}
	if ( !(type==1 || type == 0) ) {
		dutils::Error("vme::V1190::unpack_data_buffer", __FILE__, __LINE__)
			<< "\"type\" bitfield == " << type << ": Should be impossible, skipping event..."
			<< DRAGON_ERR_FILE_LINE;
		return false;
	}

	int32_t measurement = (*pbuffer >> 0) & READ19; /// - Bits 0 - 18 encode the measurement value

	if (type == 0) // leading edge
	{
		channel[ch].fLeading.push_back(measurement);
	}
	else // trailing edge
		channel[ch].fTrailing.push_back(measurement);

	channel[ch].nleading  = channel[ch].fLeading.size();
	channel[ch].ntrailing = channel[ch].fTrailing.size();

	if(type == 0) fifo0.push_back(measurement, ch, channel[ch].nleading);
	else          fifo1.push_back(measurement, ch, channel[ch].ntrailing);

	return true;
}

void vme::V1190::unpack_footer_buffer(const uint32_t* const pbuffer, const char* bankName)
{
	/*!
	 * \param [in] pbuffer Pointer to the footer buffer
	 * \param [in] bankName Name of the midas bank being unpacked
	 * \note Footer also contains the GEO information, which we ignore.
	 *
	 * See below for the bitpacking instructions and what we read.
	 */
	word_count = (*pbuffer >> 0) & READ12; /// Bits 0 - 11 are the event counter (word_count)
	int16_t evtId = (*pbuffer >> 12) & READ12;
	if(evtId != event_id) { /// Bits 12 - 23 are the event id (event_id), check for consistency w/ header
		std::cerr << DRAGON_ERR_FILE_LINE;
		dutils::Warning("vme::V1190::unpack_footer_buffer")
			<< DRAGON_ERR_FILE_LINE << "Bank name: \"" << bankName << "\": "
			<< "Trailer event id (" << evtId << ") != header event Id (" << event_id << ")\n";
	}
}

void vme::V1190::handle_error_buffer(const uint32_t* const pbuffer, const char* bankName)
{
	/*!
	 * Error encoding is handled with a bitmask, bits 0 - 13. Here we print the
	 * appropriate messages as given in the V1190 manual and set the `error` flag
	 * to the corresponding code.
	 * \param [in] pbuffer Pointer to the error buffer longword
	 * \param [in] bankName Name of the MIDAS bank containing the data in question
	 */
	std::string errors[15] = {
		"Hit lost in group 0 from read-out FIFO overflow.",
		"Hit lost in group 0 from L1 buffer overflow",
		"Hit error have been detected in group 0.",
		"Hit lost in group 1 from read-out FIFO overflow.",
		"Hit lost in group 1 from L1 buffer overflow",
		"Hit error have been detected in group 1.",
		"Hit data lost in group 2 from read-out FIFO overflow.",
		"Hit lost in group 2 from L1 buffer overflow",
		"Hit error have been detected in group 2.",
		"Hit lost in group 3 from read-out FIFO overflow.",
		"Hit lost in group 3 from L1 buffer overflow",
		"Hit error have been detected in group 3.",
		"Hits rejected because of programmed event size limit",
		"Event lost (trigger FIFO overflow).",
		"Internal fatal chip error has been detected."
	};

	for(int i=0; i< 14; ++i) {
		if((*pbuffer >> i) & READ1) {
			error = i; // set error code

			dutils::ADelayedMessagePrinter* msg = dutils::gDelayedMessageFactory.Get(this, i);
			if(!msg) {
				std::stringstream temp;
				temp << "TDC error (bank \"" << bankName << "\", addr " << this << "): " << errors[i];
				msg = dutils::gDelayedMessageFactory.Register<dutils::Error>
					(this, i, "vme::V1190::handle_error_buffer", fMessagePeriod, __FILE__, __LINE__, temp.str().c_str());
			}

			if(msg) msg->Incr();
		}
	}
}

bool vme::V1190::unpack_buffer(const uint32_t* const pbuffer, const char* bankName)
{
	/*!
	 * \param [in] pbuffer Pointer to the buffer data
	 * \param [in] bankName Name of the midas bank being unpacked
	 * \returns True if the buffer was successfully unpacked, false otherwise
	 *
	 * V1190 buffers are 32 bit words. Bits 27-31 specify the type of data contained
	 * in the buffer. In this function, we read the buffer type and then handle appropriately.
	 */
	bool success = true;
	uint32_t type = (*pbuffer >> 27) & READ5;

	switch (type) {
	case GLOBAL_HEADER:  /// case GLOBAL_HEADER: read event counter from bits 5 - 26
		count = (*pbuffer >> 5) & READ22;
		break;
	case GLOBAL_TRAILER: /// case GLOBAL_TRAILER: read status word from bits 24-26 and word count from bits 5-21
		status = (*pbuffer >> 24) & READ3;
		trailer_word_count = (*pbuffer >> 5) & READ16;
		break;
	case EXTENDED_TRIGGER_TIME: /// case EXTENDED_TRIGGER_TIME: read extended trigger from bits 0 - 26
		extended_trigger = (*pbuffer >> 0) & READ27;
		break;
	case TDC_HEADER: /// case TDC_HEADER: read bunch id from bits 0 - 12, event id from bits 12 - 23
		bunch_id = (*pbuffer >> 0)  & READ12;
		event_id = (*pbuffer >> 12) & READ12;
		break;
	case TDC_MEASUREMENT: /// case TDC_MEASUREMENT: See unpack_data_buffer()
		success = unpack_data_buffer(pbuffer);
		break;
	case TDC_ERROR:   /// case TDC_ERROR: See handle_error_buffer()
		handle_error_buffer(pbuffer, bankName);
		success = false;
		break;
	case TDC_TRAILER: /// case TDC_TRAILER: See unpack_footer_buffer()
		unpack_footer_buffer(pbuffer, bankName);
		break;
	default: /// Bail out if we read an unknown buffer code
		dutils::Error("vme::V1190::unpack_buffer", __FILE__, __LINE__)
			<< DRAGON_ERR_FILE_LINE << "Bank name: \"" << bankName
			<< "\": Unknown TDC buffer code: 0x" << std::hex << type << ". Skipping...\n";
		success = false;
		break;
	}
	return success;
}

bool vme::V1190::unpack(const midas::Event& event, const char* bankName, bool reportMissing)
{
	/*!
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


// ================ Class vme::V792 ================ //

vme::V792::V792()
{
	///
	reset();
}

void vme::V792::reset()
{
	///
	n_ch  = 0;
	count = 0;
	overflow = false;
	underflow = false;
	dutils::reset_array(MAX_CHANNELS, data);
}

int32_t vme::V792::get_data(int16_t ch) const
{
	/*!
	 * \param ch Channel number to get data from
	 * Returns the data value stored at \e ch. If \e ch is out of bounds
	 * of the internal array, prints a warning message and returns dragon::DR_NO_DATA.
	 */
	if (ch >= 0 && ch < MAX_CHANNELS) return data [ch];
	else {
		dutils::Warning("V792::get_data")
			<< "Channel number " << ch << " out of bounds (valid range: [0, "
			<< MAX_CHANNELS -1 << "]\n";
		return dragon::NoData<int32_t>::value();
	}
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
		dutils::Error("vme::V792::unpack_data_buffer", __FILE__, __LINE__)
			<< DRAGON_ERR_FILE_LINE << "Read a channel number (" << ch
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
		dutils::Error("vme::V792::unpack_buffer", __FILE__, __LINE__)
			<< DRAGON_ERR_FILE_LINE << "Bank name: \"" << bankName
			<< "\": Read INVALID_BITS code from a CAEN ADC output buffer. Skipping...\n";
		success = false;
		break;
	default: /// Bail out if we read an unknown buffer code
		dutils::Error("vme::V792::unpack_buffer", __FILE__, __LINE__)
			<< DRAGON_ERR_FILE_LINE << "Bank name: \"" << bankName
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
