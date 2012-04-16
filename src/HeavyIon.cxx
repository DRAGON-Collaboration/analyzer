/// \file HeavyIon.cxx
/// \brief Implements HeavyIon.hxx
#include "utils/copy_array.h"
#include "HeavyIon.hxx"
#include "vme/Vme.hxx"


// ====== struct dragon::hion::DSSSD ====== //

dragon::hion::DSSSD::DSSSD() :
	variables()
{
	reset();
}

namespace {
inline void copy_dsssd(const dragon::hion::DSSSD& from, dragon::hion::DSSSD& to)
{
	to.variables = from.variables;
	copy_array(from.qraw, to.qraw, dragon::hion::DSSSD::nch);
	to.tof = from.tof;
} }

dragon::hion::DSSSD::DSSSD(const dragon::hion::DSSSD& other)
{
	copy_dsssd(other, *this);
}

dragon::hion::DSSSD& dragon::hion::DSSSD::operator= (const dragon::hion::DSSSD& other)
{
	copy_dsssd(other, *this);
	return *this;
}

void dragon::hion::DSSSD::reset()
{
	for(int i=0; i< DSSSD::nch; ++i) {
		qraw[i] = vme::NONE;
	}
	tof = vme::NONE;
}

void dragon::hion::DSSSD::read_data(const dragon::hion::Modules& modules, int v1190_trigger_ch)
{
	for(int i=0; i< DSSSD::nch; ++i) {
		qraw[i] = modules.v785_data(variables.qdc_module[i], variables.qdc_ch[i]);
	}
	tof = modules.v1190b_data(variables.tof_ch) - modules.v1190b_data(v1190_trigger_ch);
}


// ====== struct dragon::hion::DSSSD::Variables ====== //

dragon::hion::DSSSD::Variables::Variables()
{
	for(int i=0; i< DSSSD::nch; ++i) {
		qdc_module[i] = 0;
		qdc_ch[i] = i;
	}
	tof_ch = 1; /// \todo Update once plugged in
}

namespace {
inline void copy_dsssd_variables(const dragon::hion::DSSSD::Variables& from, dragon::hion::DSSSD::Variables& to)
{
	copy_array(from.qdc_module, to.qdc_module, dragon::hion::DSSSD::nch);
	copy_array(from.qdc_ch, to.qdc_ch, dragon::hion::DSSSD::nch);
	to.tof_ch = from.tof_ch;
} }

dragon::hion::DSSSD::Variables::Variables(const Variables& other)
{
	copy_dsssd_variables(other, *this);
}

dragon::hion::DSSSD::Variables& dragon::hion::DSSSD::Variables::operator= (const Variables& other)
{
	copy_dsssd_variables(other, *this);
	return *this;
}

void dragon::hion::DSSSD::Variables::set(const char* odb_file)
{
	/// \todo Implement
}


// ====== struct dragon::hion::IonChamber ====== //

dragon::hion::IonChamber::IonChamber()
{
	reset();
}

namespace {
inline void copy_ic(const dragon::hion::IonChamber& from, dragon::hion::IonChamber& to)
{
	to.variables = from.variables;
	copy_array(from.anode, to.anode, dragon::hion::IonChamber::nch);
	to.tof = from.tof;
} }

dragon::hion::IonChamber::IonChamber(const dragon::hion::IonChamber& other)
{
	copy_ic(other, *this);
}

dragon::hion::IonChamber& dragon::hion::IonChamber::operator= (const dragon::hion::IonChamber& other)
{
	copy_ic(other, *this);
	return *this;
}

void dragon::hion::IonChamber::reset()
{
	for(int i=0; i< IonChamber::nch; ++i) {
		anode[i] = vme::NONE;
	}
	tof = vme::NONE;
}

void dragon::hion::IonChamber::read_data(const dragon::hion::Modules& modules, int v1190_trigger_ch)
{
	for(int i=0; i< IonChamber::nch; ++i) {
		anode[i] = modules.v785_data(variables.anode_module[i], variables.anode_ch[i]);
	}
	tof = modules.v1190b_data(variables.tof_ch) - modules.v1190b_data(v1190_trigger_ch);
}


// ====== struct dragon::hion::IonChamber::Variables ====== //

dragon::hion::IonChamber::Variables::Variables()
{
	for(int i=0; i< IonChamber::nch; ++i) {
		anode_module[i] = 0;
		anode_ch[i] = i;
	}
	tof_ch = 1; /// \todo Update once plugged in
}

namespace {
inline void copy_ic_variables(const dragon::hion::IonChamber::Variables& from, dragon::hion::IonChamber::Variables& to)
{
	copy_array(from.anode_module, to.anode_module, dragon::hion::IonChamber::nch);
	copy_array(from.anode_ch, to.anode_ch, dragon::hion::IonChamber::nch);
	to.tof_ch = from.tof_ch;
} }

dragon::hion::IonChamber::Variables::Variables(const dragon::hion::IonChamber::Variables& other)
{
	copy_ic_variables(other, *this);
}

dragon::hion::IonChamber::Variables& dragon::hion::IonChamber::Variables::operator= (const dragon::hion::IonChamber::Variables& other)
{
	copy_ic_variables(other, *this);
	return *this;
}

void dragon::hion::IonChamber::Variables::set(const char* odb_file)
{
	/// \todo Implement
}


// ====== struct dragon::hion::MCP ====== //

dragon::hion::MCP::MCP()
{
	reset();
}

namespace {
inline void copy_mcp(const dragon::hion::MCP& from, dragon::hion::MCP& to)
{
	to.variables = from.variables;
	copy_array(from.anode, to.anode, dragon::hion::MCP::nch);
	to.tac = from.tac;
	to.x = from.x;
	to.y = from.y;
} }

dragon::hion::MCP::MCP(const dragon::hion::MCP& other)
{
	copy_mcp(other, *this);
}

dragon::hion::MCP& dragon::hion::MCP::operator= (const dragon::hion::MCP& other)
{
	copy_mcp(other, *this);
	return *this;
}

void dragon::hion::MCP::reset()
{
	for(int i=0; i< MCP::nch; ++i) {
		anode[i] = vme::NONE;
	}
	tac = vme::NONE;
	x = (double)vme::NONE;
	y = (double)vme::NONE;
}

void dragon::hion::MCP::read_data(const dragon::hion::Modules& modules)
{
	for(int i=0; i< MCP::nch; ++i) {
		anode[i] = modules.v785_data(variables.anode_module[i], variables.anode_ch[i]);
	}
	tac = modules.v785_data(variables.tac_module, variables.tac_ch);

	x = -1.; /// \todo Calculate x and y
	y = -1.;
}


// ====== struct dragon::hion::MCP::Variables ====== //

dragon::hion::MCP::Variables::Variables()
{
	for(int i=0; i< MCP::nch; ++i) {
		anode_module[i] = 1;
		anode_ch[i] = i;
	}
	tac_module = 1;
	tac_ch = MCP::nch;
}

namespace {
inline void copy_mcp_variables(const dragon::hion::MCP::Variables& from, dragon::hion::MCP::Variables& to)
{
	copy_array(from.anode_module, to.anode_module, dragon::hion::MCP::nch);
	copy_array(from.anode_ch, to.anode_ch, dragon::hion::MCP::nch);
	to.tac_module = from.tac_module;
	to.tac_ch = from.tac_ch;
} }

dragon::hion::MCP::Variables::Variables(const dragon::hion::MCP::Variables& other)
{
	copy_mcp_variables(other, *this);
}

dragon::hion::MCP::Variables& dragon::hion::MCP::Variables::operator= (const dragon::hion::MCP::Variables& other)
{
	copy_mcp_variables(other, *this);
	return *this;
}

void dragon::hion::MCP::Variables::set(const char* odb_file)
{
	/// \todo Implement
}


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
