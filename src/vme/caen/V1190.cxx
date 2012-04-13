//! \file V1190.cxx
//! \brief Implements V1190.hxx
#include <map>
#include <string>
#include "utils/Bits.hxx"
#include "vme/caen/V1190.hxx"
#include "utils/Incrvoid.hxx"

namespace {
const int GLOBAL_HEADER         = 0x8;  // 00010
const int GLOBAL_TRAILER        = 0x10; // 00001
const int EXTENDED_TRIGGER_TIME = 0x11; // 10001
const int TDC_HEADER            = 0x1;  // 10000
const int TDC_MEASUREMENT       = 0x0;  // 00000
const int TDC_ERROR             = 0x4;  // 00100
const int TDC_TRAILER           = 0x3;  // 11000

typedef void (* V1190Unpacker) (uint32_t, vme::caen::V1190b&);
typedef std::map<int, V1190Unpacker> V1190UnpackerMap_t;

inline bool run_v1190_unpacker(int which, uint32_t data, vme::caen::V1190b& module)
{
	static V1190UnpackerMap_t unpackers;
	if(!unpackers.size()) {
		unpackers.insert(std::make_pair(GLOBAL_HEADER, &vme::caen::unpack_v1190_global_header));
		unpackers.insert(std::make_pair(GLOBAL_TRAILER, &vme::caen::unpack_v1190_global_footer));
		unpackers.insert(std::make_pair(EXTENDED_TRIGGER_TIME, &vme::caen::handle_v1190_extended_trigger));
		unpackers.insert(std::make_pair(TDC_HEADER, &vme::caen::unpack_v1190_header));
		unpackers.insert(std::make_pair(TDC_MEASUREMENT, &vme::caen::unpack_v1190_data));
		unpackers.insert(std::make_pair(TDC_ERROR, &vme::caen::handle_v1190_error));
		unpackers.insert(std::make_pair(TDC_TRAILER, &vme::caen::unpack_v1190_footer));
	}
	if(!unpackers.count(which)) return false;
	unpackers.find(which)->second (data, module);
	return true;
}

} // namespace



void vme::caen::unpack_v1190_data(uint32_t data, V1190b& module) {
	module.type = (data >> 26) & READ1;
	int ch = (data >> 19) & READ7;
	module.data[ch] = (data >> 0) & READ19;

}

void vme::caen::unpack_v1190_header(uint32_t data, V1190b& module) {
	module.bunch_id = (data >> 0)  & READ12;
	module.event_id = (data >> 12) & READ12;
}

void vme::caen::unpack_v1190_global_header(uint32_t data, V1190b& module) {
	module.count = (data >> 5) & READ22;
}

void vme::caen::unpack_v1190_footer(uint32_t data, V1190b& module) {
	module.word_count = (data >> 0) & READ12;
	int16_t evtId = (data >> 12) & READ12;
	if(evtId != module.event_id) {
		std::cerr << ERR_FILE_LINE;
		err::Throw() << "Trailer event id (" << evtId << ") != header event Id ("
								 << module.event_id << ")\n";
	}
}

void vme::caen::unpack_v1190_global_footer(uint32_t data, V1190b& module) {
	module.status = (data >> 24) & READ3;
	module.trailer_word_count = (data >> 5) & READ16;
}

void vme::caen::handle_v1190_extended_trigger(uint32_t data, V1190b& module) {
	module.extended_trigger = (data >> 0) & READ27;
}

void vme::caen::handle_v1190_error(uint32_t data, V1190b& module) {
	int flag;
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
	err::Throw error;
	error << "TDC Error buffer: error flags:\n";
		
	for(int i=0; i< 14; ++i) {
		flag = (data >> i) & READ1;
		if(flag) {
			 error << "[" << i << "]: " << errors[i] << "\n";
		}
	}
}

bool vme::caen::unpack_v1190_buffer(void* address, const char* bank, V1190b& module) {
	bool success;
	try {
		uint32_t* data32 = reinterpret_cast<uint32_t*>(address);
		uint32_t type = (*data32 >> 27) & READ5;
		success = run_v1190_unpacker(type, *data32, module);
		if(!success) {
			std::cerr << ERR_FILE_LINE;
			err::Throw("unpack_v1190_buffer")
				 << "Unknown CAEN V1190b buffer type (bits 27, 28, 29, 30, 31 = "
				 << ((*data32 >> 27) & READ1) << ", " << ((*data32 >> 28) & READ1) << ", "
				 << ((*data32 >> 29) & READ1) << ", " << ((*data32 >> 30) & READ1) << ", "
				 << ((*data32 >> 31) & READ1) << ").\n";
		}
	}
	catch (std::exception& e) {
		std::cerr << e.what() << "    MIDAS Bank name: " << bank << "\n\n";
		success = false;
	}
	return success;
}

bool vme::caen::unpack_v1190(const TMidasEvent& event, const char* bank, V1190b& module) {
	void* p_bank = NULL;
  int bank_len, bank_type;
  int found = event.FindBank(bank, &bank_len, &bank_type, &p_bank);
  if(!found) return false;

  // Loop over all buffers int the bank
	bool ret = true;
  for(int i=0; i< bank_len; ++i) {
    bool success = unpack_v1190_buffer(p_bank, bank, module);
		if(!success) ret = false;
    increment_void(p_bank, bank_type);
  }
  return ret;
}
