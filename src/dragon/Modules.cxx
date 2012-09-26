/// \file Modules.cxx
/// \author G. Christian
/// \brief Implements Modules.hxx
#include <cassert>
#include <sstream>
#include "utils/Error.hxx"
#include "Modules.hxx"


// ====== Class dragon::gamma::Modules ======= //

dragon::gamma::Modules::Modules():
	v792(), v1190(), io32()
{
	sprintf(banks.v792,   "ADC0");
	sprintf(banks.v1190,  "TDC0");
	sprintf(banks.io32,   "VTR0");
	reset();
};

dragon::gamma::Modules::Modules(const Modules& other)
{
	v792   = other.v792;
	v1190  = other.v1190;
	io32   = other.io32;
	banks  = other.banks;
}

dragon::gamma::Modules& dragon::gamma::Modules::operator= (const dragon::gamma::Modules& other)
{
	v792   = other.v792;
	v1190  = other.v1190;
	io32   = other.io32;
	banks  = other.banks;
	return *this;
}

void dragon::gamma::Modules::reset()
{
	v792.reset();
	v1190.reset();
}

void dragon::gamma::Modules::unpack(const dragon::MidasEvent& event)
{
	v792.unpack(event, banks.v792);
	v1190.unpack(event, banks.v1190);
	vme::unpack_io32(event, banks.io32, io32);
}

int16_t dragon::gamma::Modules::v792_data(unsigned ch) const
{
#ifdef DEBUG
	assert(ch < 32);
#endif
	return v792.data[ch];
}

int16_t dragon::gamma::Modules::v1190_data(unsigned ch) const
{
#ifdef DEBUG
	assert(ch < 64);
#endif
	err::Info("v1190_data") << "TODO: update!" << ERR_FILE_LINE;
	return -1; //v1190.data[ch];
}

int32_t dragon::gamma::Modules::tstamp() const
{
	return io32.tstamp;
}


// ====== Class dragon::hion::Modules ======= //

dragon::hion::Modules::Modules():
	v1190(), io32()
{
	for(int i=0; i< 2; ++i) {
		sprintf(banks.v785[i], "TLQ%d", i);
	}
	sprintf(banks.v1190, "TLT0");
	sprintf(banks.io32,  "TLIO");
	reset();
};

dragon::hion::Modules::Modules(const Modules& other)
{
	for(int i=0; i< 2; ++i) {
		v785[i] = other.v785[i];
	}
	v1190  = other.v1190;
	io32   = other.io32;
	banks  = other.banks;
}

void dragon::hion::Modules::reset()
{
	for(int i=0; i< 2; ++i) {
		v785[i].reset();
	}
	v1190.reset();
}

dragon::hion::Modules& dragon::hion::Modules::operator= (const Modules& other)
{
	for(int i=0; i< 2; ++i) {
		v785[i] = other.v785[i];
	}
	v1190  = other.v1190;
	io32   = other.io32;
	banks  = other.banks;
	return* this;
}

void dragon::hion::Modules::unpack(const dragon::MidasEvent& event)
{

	for(int i=0; i< 2; ++i) {
		v785[i].unpack(event, banks.v785[i]);
	}

	v1190.unpack(event, banks.v1190);
	vme::unpack_io32(event, banks.io32, io32);
}

int16_t dragon::hion::Modules::v785_data(unsigned which, unsigned ch) const
{
#ifdef DEBUG
	assert(which < 2);
	assert(ch < 32);
#endif
	return v785[which].data[ch];
}

int16_t dragon::hion::Modules::v1190_data(unsigned ch) const
{
#ifdef DEBUG
	assert(ch < 64);
#endif
	err::Info("v1190_data") << "TODO: update!" << ERR_FILE_LINE;
	return -1; //v1190.data[ch];
}

int32_t dragon::hion::Modules::tstamp() const
{
	return io32.tstamp;
}
