/// \file MCP.cxx
/// \brief Implements MCP.hxx
#include <string>
#include <iostream>
#include "utils/copy_array.h"
#include "dragon/heavy_ion/MCP.hxx"
#include "vme/Vme.hxx"
#include "odb/Odb.hxx"
#include "odb/MidasXML.hxx"


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
}

void dragon::hion::MCP::calculate()
{
	if(!is_valid<int16_t>(anode, MCP::nch)) return;
	const double Lhalf = 25.;  // half the length of a single side of the MCP (50/2 [mm])
	int32_t sum = 0;
	for(int i=0; i< MCP::nch; ++i) sum += anode[i];
	if(sum) {
		x = Lhalf * ( (anode[1] + anode[2]) - (anode[0] + anode[3]) ) / (double)sum;
		y = Lhalf * ( (anode[0] + anode[1]) - (anode[2] + anode[3]) ) / (double)sum;
	}
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

void dragon::hion::MCP::Variables::set(const char* odb)
{
	/// \todo Set actual ODB paths, TEST!!
	const std::string pathAnodeModule = "Equipment/MCP/Variables/AnodeModule";
	const std::string pathAnodeCh     = "Equipment/MCP/Variables/AnodeChannel";
	const std::string pathTacCh       = "Equipment/MCP/Variables/TACChannel";
	const std::string pathTacModule   = "Equipment/MCP/Variables/TACModule";

	if(strcmp(odb, "online")) { // Read from offline XML file
		MidasXML mxml (odb);
		bool success = false;
		mxml.GetArray(pathAnodeModule.c_str(), MCP::nch, anode_module, &success);
		mxml.GetArray(pathAnodeCh.c_str(), MCP::nch, anode_ch, &success);
		mxml.GetValue(pathTacCh.c_str(), tac_ch, &success);
		mxml.GetValue(pathTacModule.c_str(), tac_module, &success);

		if(!success) {
			std::cerr << "Error (MCP::Variables::set): Couldn't set one or more variable values properly.\n";
		}
	}
	else { // Read from online ODB.
#ifdef MIDASSYS
		for(int i=0; i< dragon::hion::MCP::nch; ++i) {
			anode_ch[i] = odb::ReadInt(pathAnodeCh.c_str(), i, 0);
			anode_module[i] = odb::ReadInt(pathAnodeModule.c_str(), i, 0);
		}
		tac_ch = odb::ReadInt(pathTacCh.c_str(), 0, 0);
		tac_module = odb::ReadInt(pathTacModule.c_str(), 0, 0);
#else
		std::cerr << "MIDASSYS not defined, can't read from online ODB, no changes made.\n";
#endif
	}
}

