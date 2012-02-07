//! \file vme.cxx
//! \brief Implements vme.hxx
#include <stdio.h>
#include <bitset>
#include "vme.hxx"
#include "deb_err.h"



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
  if(it == data_sizes.end())  ERR("Unknown midas data type: %d\n", midas_data_type);

  begin = reinterpret_cast<char*>(begin) + it->second;
}


typedef unsigned long ulong_t;
// int16_t vme::Module::NONE = -1;
// vme::Module::Map vme::Module::all;



vme::Module::Module(const char* bank_name, uint16_t max_channels) :
  bank(bank_name), max_ch(max_channels), count(0), self_allocated(true)
{
  data = new int16_t[max_ch];
  this->reset();
  all().insert(std::make_pair<std::string, Module*>(this->bank, this));
}

vme::Module::Module(const char* bank_name, uint16_t max_channels, int16_t* data_) :
  bank(bank_name), max_ch(max_channels), count(0), self_allocated(false)
{
  data = data_;
  this->reset();
  all().insert(std::make_pair<std::string, Module*>(this->bank, this));
}

vme::Module::~Module()
{
  vme::Module::Map::iterator it = all().find(this->bank);
  if(it != all().end()) all().erase(it);
  if(self_allocated)
    delete[] data;
}

void vme::Module::reset()
{
  underflow = false;
  overflow = false;
  n_ch = 0;
  for(int16_t i=0; i< max_ch; ++i)
    data[i] = NONE;
}

int16_t vme::Module::get_data(int ch)
{
  int16_t ret = NONE;
  if(this->max_ch < ch)
    WAR("Invalid channel number: %d (maximum %d). Bank name: %s\n",
	ch, max_ch-1, bank.c_str());
  else ret = data[ch];
  return ret;
}

void vme::Module::copy_data(int16_t* destination)
{
  int nbytes = max_ch*sizeof(int16_t);
  memcpy(reinterpret_cast<void*>(destination), reinterpret_cast<void*>(this->data), nbytes);
}

vme::Module* vme::Module::find(const char* bank_name)
{
  vme::Module::Map::iterator it = all().find(bank_name);
  return it != all().end() ? it->second : NULL;
}


int vme::Module::unpack_all(const TMidasEvent& event)
{
  Map::iterator it = all().begin();
  while(it != all().end()) {
    void* p_bank = NULL;
    Module* m = (*it++).second;

    int bank_len, bank_type;
    int found = event.FindBank(m->bank.c_str(), &bank_len, &bank_type, &p_bank);
    if(!found) continue;

    // Loop over all buffers int the bank    
    for(int i=0; i< bank_len; ++i) {
      m->unpack_buffer(p_bank);
      increment_void(p_bank, bank_type);
    }
  }  return 0;
}

void vme::Module::reset_all()
{
  Map::iterator it = all().begin();
  while(it != all().end()) {
    Module* m = (*it++).second;
    m->reset();
  }
}

template <size_t N_IN, size_t N_OUT>
void copy_bits(const std::bitset<N_IN>& from, std::bitset<N_OUT>& to, const uint32_t first_bit)
{
  for(uint32_t i=0; i< N_OUT; ++i)
    to[i] = from[i+first_bit];
}

template <size_t N_IN, size_t N_OUT>
ulong_t read_bits(const std::bitset<N_IN>& from, const uint32_t first_bit)
{
  std::bitset<N_OUT> subset(0);
  copy_bits<N_IN, N_OUT> (from, subset, first_bit);
  return subset.to_ulong();
}

vme::caen::Adc::Adc(const char* bank_name, uint16_t num_channels) :
  Module(bank_name, num_channels) { };

vme::caen::Adc::Adc(const char* bank_name, uint16_t num_channels, int16_t* data_) :
  Module(bank_name, num_channels, data_) { };

vme::caen::Adc::~Adc() { };

int vme::caen::Adc::unpack_buffer(void* addr)
{
  int ret = 0;
  uint32_t* data32 = reinterpret_cast<uint32_t*>(addr);
  std::bitset<32> all_bits (*data32);
  
  ulong_t type = read_bits<32, 3> (all_bits, 24);
  static const ulong_t DATA_BITS    = 0x0; // 0 0 0
  static const ulong_t HEADER_BITS  = 0x2; // 0 1 0
  static const ulong_t FOOTER_BITS  = 0x4; // 0 0 1
  static const ulong_t INVALID_BITS = 0x6; // 0 1 1

  switch(type) {
  case DATA_BITS:
    {
      uint16_t ch = read_bits<32, 5> (all_bits, 16);
      if(max_ch < ch) {
	WAR("Read a channel number (%d) which is >= the maximum (%d), aborting. "
	    "Bank name: %s\n", ch, max_ch, bank.c_str());
	ret = 1;
      }
      this->underflow = all_bits[13];
      this->overflow  = all_bits[12];
      this->data[ch] = read_bits<32, 12> (all_bits,  0);
      break;
    }
  case HEADER_BITS:
    {
      //! \note Header contains the geographic address (GEO), crate number, and
      //!  number of channels with data. GEO and crate are unused with Midas, so
      //!  we'll ignore them and just set the number of channels.
      n_ch = (int16_t)read_bits<32, 6> (all_bits, 8);
      break;
    }
  case FOOTER_BITS:
    {
      //! \note Footer contains the GEO and event counter, read the event counter only.
      count = read_bits<32, 24> (all_bits, 0);
      break;
    }
  case INVALID_BITS:
    {
      WAR("Invalid CAEN ADC output buffer. Bank: %s\n",
	  bank.c_str());
      ret = 1;
      break;
    }
  default:
    {
      std::bitset<3> type_bits(type);
      WAR("Unknown CAEN ADC buffer type (bits 24, 25, 26 = %i, %i, %i). Bank: %s\n",
	  (int)type_bits[0], (int)type_bits[1], (int)type_bits[2], bank.c_str());
      ret = 1;
      break;
    }
  }
  return ret;
}
