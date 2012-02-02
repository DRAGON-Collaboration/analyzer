//! \file vme.cxx
//! \brief Implements vme.hxx
#include <bitset>
#include "vme.hxx"
#include "deb_err.h"

typedef unsigned long ulong_t;
int16_t vme::Module::NONE = -1;
vme::Module::Map vme::Module::all;

bool vme::Module::Id::Compare::operator() (const Id& lhs, const Id& rhs)
{
  if (lhs.bank != rhs.bank)
    return (lhs.bank < rhs.bank);
  else
    return (lhs.number < rhs.number);
}    

vme::Module::Module(uint16_t max_channels, const std::string& bank_, int16_t number_) :
  id(bank_, number_), max_ch(max_channels)
{
  data = new int16_t[max_ch];
  this->reset();
  all.insert(std::make_pair<Id, Module*>(this->id, this));
}

vme::Module::~Module()
{
  vme::Module::Map::iterator it = all.find(this->id);
  if(it != all.end()) all.erase(it);
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
    WAR("Invalid channel number: %d (maximum %d). Bank name: %s, module number: %d\n",
	ch, max_ch-1, id.bank.c_str(), id.number);
  else ret = data[ch];
  return ret;
}

vme::Module* vme::Module::find(const std::string& bank_, int16_t module_)
{
  vme::Module::Map::iterator it = all.find(Id(bank_, module_));
  return it != all.end() ? it->second : NULL;
}


void* increment_void(void* begin, int midas_data_type)
{
  void* out = NULL;
  switch(midas_data_type) {
    // n.b. see midas.h for all available types
  case 4 : // 16 bit unsigned
    {
      uint16_t* word16 = reinterpret_cast<uint16_t*> (begin);
      ++word16;
      out = reinterpret_cast<void*> (word16);
      break;
    }
  case 6 : // 32 bit word
    {
      uint32_t* word32 = reinterpret_cast<uint32_t*> (begin);
      ++word32;
      out = reinterpret_cast<void*> (word32);
      break;
    }
  default :
    ERR("Can't increment event pointer: unknown midas_data_type %d\n", midas_data_type);
    break;
  }
  return out;
}

int vme::Module::unpack_all(const TMidasEvent& event)
{
  Map::iterator it = all.begin();
  while(it != all.end()) {
    void* p_bank = NULL;
    Module* m = (*it++).second;

    int bank_len, bank_type;
    int found = event.FindBank(m->id.bank.c_str(), &bank_len, &bank_type, &p_bank);
    if(!found) continue;

    // Loop over all buffers int the bank    
    for(int i=0; i< bank_len; ++i) {
      m->unpack_buffer(p_bank);
      p_bank = increment_void(p_bank, bank_type);
    }
  }
  return 0;
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

vme::caen::Adc::Adc(uint16_t num_channels, const std::string& bank_, int16_t number_) :
  Module(num_channels, bank_, number_) { };

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
  case DATA_BITS: {
    uint16_t ch = read_bits<32, 5> (all_bits, 16);
    if(max_ch < ch) {
      WAR("Read a channel number (%d) which is >= the maximum (%d), aborting. "
	  "Bank name: %s, module number: %d\n", ch, max_ch, id.bank.c_str(), id.number);
      ret = 1;
    }

    this->underflow = all_bits[13];
    this->overflow  = all_bits[12];
    this->data[ch] = read_bits<32, 12> (all_bits,  0);
    break;
    }
  case HEADER_BITS:
    //! \note Header contains the geographic address (GEO), crate number, and
    //!  number of channels with data. GEO and crate are unused with Midas, so
    //!  we'll ignore them and just set the number of channels.
    n_ch = (int16_t)read_bits<32, 6> (all_bits, 8);
    break;
  case FOOTER_BITS:
    //! \note Footer contains the GEO and event counter. I see no need to use the
    //! event counter, so we can just ignore the footer for now.
    break;
  case INVALID_BITS:
    WAR("Invalid CAEN ADC output buffer. Bank: %s, number: %d\n",
	id.bank.c_str(), id.number);
    ret = 1;
    break;
  default: {
    std::bitset<3> type_bits(type);
    WAR("Unknown CAEN ADC buffer type (bits 24, 25, 26 = %i, %i, %i). Bank: %s, number: %d\n",
	(int)type_bits[0], (int)type_bits[1], (int)type_bits[2], id.bank.c_str(), id.number);
    ret = 1;
    break;
    }
  }
  return ret;
}
