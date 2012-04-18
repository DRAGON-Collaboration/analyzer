/// \file MCP.cxx
/// \brief Implements MCP.hxx
#include "heavy_ion/MCP.hxx"
#include "vme/Vme.hxx"
#include "utils/copy_array.h"


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

