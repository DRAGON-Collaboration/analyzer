/// \file Gamma.cxx
/// \author G. Christian
/// \brief Implements Gamma.hxx
#include "Gamma.hxx"


// ========== Class dragon::gamma::Gamma ========== //
dragon::gamma::Gamma::Gamma() :
	evt_count(0), bgo()
{
	reset();
}

namespace {
inline void copy_gamma(const dragon::gamma::Gamma& from, dragon::gamma::Gamma& to)
{
	to.evt_count = from.evt_count;
	to.modules = from.modules;
	to.header  = from.header;
	to.bgo = from.bgo;
} }

dragon::gamma::Gamma::Gamma(const dragon::gamma::Gamma& other)
{
	copy_gamma(other, *this);
}

dragon::gamma::Gamma& dragon::gamma::Gamma::operator= (const dragon::gamma::Gamma& other)
{
	copy_gamma(other, *this);
	return *this;
}

void dragon::gamma::Gamma::reset()
{
	modules.reset();
	bgo.reset();
}

void dragon::gamma::Gamma::set_variables(const char* odb)
{
	bgo.variables.set(odb);
}

void dragon::gamma::Gamma::unpack(const midas::Event& event)
{
	reset();
	modules.unpack(event);
	event.CopyHeader(header);
}

void dragon::gamma::Gamma::read_data()
{
	++evt_count;
	bgo.read_data(modules);
}

void dragon::gamma::Gamma::calculate()
{
	bgo.calculate();
}
