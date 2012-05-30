/// \file HeavyIon.cxx
/// \brief Implements HeavyIon.hxx
#include <string>
#include <iostream>
#include "utils/copy_array.h"
#include "dragon/heavy_ion/HeavyIon.hxx"
#include "vme/Vme.hxx"
#include "odb/Odb.hxx"
#include "odb/MidasXML.hxx"


// ====== struct dragon::hion::HeavyIon ====== //

dragon::hion::HeavyIon::HeavyIon() :
#ifndef DRAGON_OMIT_DSSSD
	dsssd(),
#endif
#ifndef DRAGON_OMIT_IC
	ic(),
#endif
	mcp(),
	sb()
#ifndef DRAGON_OMIT_NAI
	, nai()
#endif
#ifndef DRAGON_OMIT_GE
	, ge()
#endif
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
	to.sb  = from.sb;
#ifndef DRAGON_OMIT_NAI
	to.nai = from.nai;
#endif
#ifndef DRAGON_OMIT_GE
	to.ge = from.ge;
#endif
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
	sb.reset();
#ifndef DRAGON_OMIT_NAI
	nai.reset();
#endif
#ifndef DRAGON_OMIT_GE
	ge.reset();
#endif
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
	sb.read_data(modules);
#ifndef DRAGON_OMIT_NAI
	nai.read_data(modules);
#endif
#ifndef DRAGON_OMIT_GE
	ge.read_data(modules);
#endif
}

void dragon::hion::HeavyIon::calculate()
{
	mcp.calculate();
#ifndef DRAGON_OMIT_NAI
	nai.calculate();
#endif
#ifndef DRAGON_OMIT_GE
	ge.calculate();
#endif
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

void dragon::hion::HeavyIon::Variables::set(const char* odb)
{
	/// \todo Set actual ODB paths, TEST!!
	const std::string path = "Equipment/V1190/HeavyIon/TriggerCh";

	if(strcmp(odb, "online")) { // Read from offline XML file
		MidasXML mxml (odb);
		bool success = false;
		mxml.GetValue(path.c_str(), v1190_trigger_ch, &success);

		if(!success) {
			std::cerr << "Error (HeavyIon::Variables::set): Couldn't set one or more variable values properly.\n";
		}
	}
	else { // Read from online ODB.
#ifdef MIDASSYS
		v1190_trigger_ch = odb::ReadInt(path.c_str(), 0, 0);
#else
		std::cerr << "MIDASSYS not defined, can't read from online ODB, no changes made.\n";
#endif
	}
}

void dragon::hion::HeavyIon::set_variables(const char* odb)
{
#ifndef DRAGON_OMIT_DSSSD
	dsssd.variables.set(odb);
#endif
#ifndef DRAGON_OMIT_IC
	ic.variables.set(odb);
#endif
	mcp.variables.set(odb);
	sb.variables.set(odb);
	variables.set(odb);
#ifndef DRAGON_OMIT_NAI
	nai.variables.set(odb);
#endif
#ifndef DRAGON_OMIT_GE
	ge.variables.set(odb);
#endif
}
