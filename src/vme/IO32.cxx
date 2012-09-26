//! \file IO32.cxx
//! \author G. Christian
//! \brief Implements IO32.hxx
#include "utils/Error.hxx"
#include "midas/Event.hxx"
#include "IO32.hxx"


bool vme::IO32::unpack(const midas::Event& event, const char* bankName, bool reportMissing)
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
	* \endcode
	*
	* The TSC4 bank is already unpacked in a midas::Event, so we can just copy the data over
	* \param [in] event The midas event to unpack
	* \param [in] bankName Name of the "main" IO32 bank
	* \returns True if the event was successfully unpacked, false otherwise
	*/
	int bank_len;
	uint32_t* pdata32 =
		event.GetBankPointer<uint32_t>(bankName, &bank_len, reportMissing, true);

	assert (bank_len == 8);

	uint32_t* data_fields[8] = { &header, &trig_count, &tstamp, &start,
															 &end, &latency, &read_time, &busy_time };

	for (int i=0; i< bank_len; ++i) {
		*(data_fields[i]) = *pdata32++;
	}

  return true;
}


void vme::IO32::reset()
{
	uint32_t* data_fields[8] =
		{ &header, &trig_count, &tstamp, &start, &end, &latency, &read_time, &busy_time };
	for (int i=0; i< 8; ++i) {
		*(data_fields[i]) = 0;
	}
}
