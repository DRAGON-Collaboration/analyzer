/// \file HeavyIon.cxx
/// \brief Implements HeavyIon.hxx
#include "heavy_ion/HeavyIon.hxx"
#include "utils/copy_array.h"
#include "vme/Vme.hxx"


// ====== struct dragon::hion::HeavyIon ====== //

dragon::hion::HeavyIon::HeavyIon() :
#ifndef DRAGON_OMIT_DSSSD
	dsssd(),
#endif
#ifndef DRAGON_OMIT_IC
	ic(),
#endif
	mcp()
{
	reset();
}

namespace {
void copy_hi(const dragon::hion::HeavyIon& from, dragon::hion::HeavyIon& to)
{
	to.variables = from.variables;
	to.evt_count = from.evt_count;
	to.modules = from.modules;
#ifndef DRAGON_OMIT_DSSSD
	to.dsssd = from.dsssd;
#endif
#ifndef DRAGON_OMIT_IC
	to.ic = from.ic;
#endif
	to.mcp = from.mcp;
} }

dragon::hion::HeavyIon::HeavyIon(const dragon::hion::HeavyIon& other)
{
	copy_hi(other, *this);
}

dragon::hion::HeavyIon& dragon::hion::HeavyIon::operator= (const dragon::hion::HeavyIon& other)
{
	copy_hi(other, *this);
	return *this;
}

void dragon::hion::HeavyIon::reset()
{
	modules.reset();
#ifndef DRAGON_OMIT_DSSSD
	dsssd.reset();
#endif
#ifndef DRAGON_OMIT_IC
	ic.reset();
#endif
	mcp.reset();
}

void dragon::hion::HeavyIon::unpack(TMidasEvent& event)
{
	reset();
	modules.unpack(event);
}

void dragon::hion::HeavyIon::read_data()
{
	++evt_count;
#ifndef DRAGON_OMIT_DSSSD
	dsssd.read_data(modules, variables.v1190_trigger_ch);
#endif
#ifndef DRAGON_OMIT_IC
	ic.read_data(modules, variables.v1190_trigger_ch);
#endif
	mcp.read_data(modules);
}


// ====== struct dragon::hion::HeavyIon::Variables ====== //

dragon::hion::HeavyIon::Variables::Variables() :
	v1190_trigger_ch(0)
{
	// nothing else to do
}

namespace {
inline void copy_hion_variables(const dragon::hion::HeavyIon::Variables& from, dragon::hion::HeavyIon::Variables& to)
{
	to.v1190_trigger_ch = from.v1190_trigger_ch;
} }

dragon::hion::HeavyIon::Variables::Variables(const dragon::hion::HeavyIon::Variables& other)
{
	copy_hion_variables(other, *this);
}

dragon::hion::HeavyIon::Variables& dragon::hion::HeavyIon::Variables::operator= (const dragon::hion::HeavyIon::Variables& other)
{
	copy_hion_variables(other, *this);
	return *this;
}

void dragon::hion::HeavyIon::Variables::set(const char* odb_file)
{
	// todo
}
