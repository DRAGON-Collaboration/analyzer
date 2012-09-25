//! \file IO32.cxx
//! \brief Implements IO32.hxx
#include <iostream>
#include "IO32.hxx"

uint64_t vme::calc_ts64(uint32_t tstamp32, uint32_t event_number)
{
	const  uint64_t max32 = 4294967295;
	static uint64_t last = 0;
	static uint32_t nrollover = 0;
	if(!event_number) {
		last = 0;
		nrollover = 0;
	}

	uint64_t ts64 = tstamp32 + nrollover*max32;
	if(ts64 < last) {
		++nrollover;
		ts64 += max32;
	}
	last = ts64;
	return ts64;
}

bool vme::unpack_io32(const dragon::MidasEvent& event, const char* bank, vme::IO32& module)
{
	void* p_bank = 0;
  int bank_len, bank_type;
  int found = event.FindBank(bank, &bank_len, &bank_type, &p_bank);
  if(!found) return false;

	uint32_t* pbank32 = reinterpret_cast<uint32_t*>(p_bank);
	module.header     = pbank32[0];
	module.trig_count = pbank32[1];
	module.tstamp     = pbank32[2];
	module.start      = pbank32[3];
	module.end        = pbank32[4];
	module.tstamp64   = calc_ts64(module.tstamp, module.trig_count);	

//	printf("count: %u, ts32: %u, ts64: %lu\n", module.trig_count , module.tstamp , module.tstamp64);

  return true;
}
