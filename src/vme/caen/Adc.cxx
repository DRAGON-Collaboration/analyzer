//! \file Adc.cxx
//! \brief Implements Adc.hxx
#include <map>
#include "Adc.hxx"
#include "vme/Vme.hxx"
#include "utils/bits.hxx"
#include "utils/Error.hxx"


namespace {
const int DATA_BITS    = 0x0; // 0 0 0
const int HEADER_BITS  = 0x2; // 0 1 0
const int FOOTER_BITS  = 0x4; // 0 0 1
const int INVALID_BITS = 0x6; // 0 1 1
typedef void (* AdcUnpacker) (uint32_t, vme::caen::Adc<32>&);
typedef std::map<int, AdcUnpacker> AdcUnpackerMap_t;

inline bool run_adc_unpacker(int which, uint32_t data, vme::caen::Adc<32>& module)
{
	static AdcUnpackerMap_t unpackers;
	if(!unpackers.size()) {
		unpackers.insert(std::make_pair(DATA_BITS, &vme::caen::unpack_adc_data));      
		unpackers.insert(std::make_pair(HEADER_BITS, &vme::caen::unpack_adc_header));  
		unpackers.insert(std::make_pair(FOOTER_BITS, &vme::caen::unpack_adc_footer));  
		unpackers.insert(std::make_pair(INVALID_BITS, &vme::caen::handle_adc_invalid));
	}
	if(!unpackers.count(which)) return false;
	unpackers.find(which)->second (data, module);
	return true;
}

void increment_void(void*& begin, int midas_data_type)
{
  static std::map<int, int> data_sizes;
  if(!data_sizes.size()) { // From midas.h
    data_sizes[1]  = 1;    // TID_BYTE      1   /**< unsigned byte         0       255    */
    data_sizes[2]  = 1;	   // TID_SBYTE     2   /**< signed byte         -128      127    */
    data_sizes[3]  = 1;	   // TID_CHAR      3   /**< single character      0       255    */
    data_sizes[4]  = 2;	   // TID_WORD      4   /**< two bytes             0      65535   */
    data_sizes[5]  = 2;	   // TID_SHORT     5   /**< signed word        -32768    32767   */
    data_sizes[6]  = 4;	   // TID_DWORD     6   /**< four bytes            0      2^32-1  */
    data_sizes[7]  = 4;	   // TID_INT       7   /**< signed dword        -2^31    2^31-1  */
    data_sizes[8]  = 4;	   // TID_BOOL      8   /**< four bytes bool       0        1     */
    data_sizes[9]  = 4;	   // TID_FLOAT     9   /**< 4 Byte float format                  */
    data_sizes[10] = 8;	   // TID_DOUBLE   10   /**< 8 Byte float format                  */
  }			 
  std::map<int, int>::iterator it = data_sizes.find(midas_data_type);
  if(it == data_sizes.end())
		 err::Throw("increment_void") << "Unknown midas data type: " << midas_data_type << "\n";

  begin = reinterpret_cast<char*>(begin) + it->second;
}

} // namespace

void vme::caen::unpack_adc_data(uint32_t data, Adc<32>& module)
{
	uint16_t ch = (data >> 16) & READ5;
	if(32 < ch) {
		std::cerr << ERR_FILE_LINE;
		err::Throw("unpack_adc_data")
			 << "Read a channel number (" << ch << ") which is >= the maximum (32), aborting.\n";
	}
	module.underflow = (data >> 13) & READ1;
	module.overflow  = (data >> 12) & READ1;
	module.data[ch]  = (data >>  0) & READ12;
}

void vme::caen::unpack_adc_header(uint32_t data, Adc<32>& module)
{
	module.n_present = (data >> 6) & READ8;
}

void vme::caen::unpack_adc_footer(uint32_t data, Adc<32>& module)
{
	module.count = (data >> 0) & READ24;
}

void vme::caen::handle_adc_invalid(uint32_t data, Adc<32>& module)
{
	std::cerr << ERR_FILE_LINE;
	err::Throw("handle_adc_invalid")
		 << "Read INVALID_BITS code from a CAEN ADC output buffer.\n";
}

bool vme::caen::unpack_adc_buffer(void* address, const char* bank, vme::caen::Adc<32>& module)
{
	bool success;
	try {
		uint32_t* data32 = reinterpret_cast<uint32_t*>(address);;
		uint64_t type = (*data32 >> 24) & READ3;
		success = run_adc_unpacker(type, *data32, module);
		if(!success) {
			std::cerr << ERR_FILE_LINE;
			err::Throw("unpack_adc_buffer")
				 << "Unknown CAEN ADC buffer type (bits 24, 25, 26 = "
				 << ((*data32 >> 24) & READ1) << ", " << ((*data32 >> 25) & READ1) << ", "
				 << ((*data32 >> 26) & READ1) << ").\n";
		}
	}
	catch (std::exception& e) {
		std::cerr << e.what() << "    MIDAS Bank name: " << bank << "\n\n";
		success = false;
	}
	return success;
}

bool vme::caen::unpack_adc(const TMidasEvent& event, const char* bank, vme::caen::Adc<32>& module)
{
	void* p_bank = NULL;
  int bank_len, bank_type;
  int found = event.FindBank(bank, &bank_len, &bank_type, &p_bank);
  if(!found) return false;

  // Loop over all buffers int the bank
	bool ret = true;
  for(int i=0; i< bank_len; ++i) {
    bool success = unpack_adc_buffer(p_bank, bank, module);
		if(!success) ret = false;
    increment_void(p_bank, bank_type);
  }
  return ret;
}

// for testing //
#ifdef ADC_TEST
int main() {

	vme::caen::V792 qdc;
	vme::reset (qdc);
	std::cout << qdc.data[25] << "\n";
	qdc.data[10] = 123;
	short data[32];
	vme::copy_data(qdc, data);
	std::cout << data[10] << "\n";

	try {
		vme::caen::unpack_adc_data(12, qdc);
	}
	catch (std::exception& e) {
		std::cerr << e.what() << "Bank name: " << "VADC" << "\n\n";
	}
}
#endif
