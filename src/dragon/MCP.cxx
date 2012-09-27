/// \file MCP.cxx
/// \author G. Christian
/// \brief Implements MCP.hxx
#include <string>
#include <iostream>
#include "utils/copy_array.h"
#include "vme/Vme.hxx"
#include "midas/Odb.hxx"
#include "midas/Xml.hxx"
#include "MCP.hxx"


// ====== class dragon::MCP ====== //

dragon::MCP::MCP()
{
	reset();
}

void dragon::MCP::reset()
{
	for(int i=0; i< MAX_CHANNELS; ++i) {
		anode[i] = vme::NONE;
	}
	tac = vme::NONE;
	x = (double)vme::NONE;
	y = (double)vme::NONE;
}

void dragon::MCP::read_data(const dragon::hion::Modules& modules)
{
	for(int i=0; i< MAX_CHANNELS; ++i) {
		anode[i] = modules.v785_data(variables.anode_module[i], variables.anode_ch[i]);
	}
	tac = modules.v785_data(variables.tac_module, variables.tac_ch);
}

void dragon::MCP::calculate()
{
	if(!is_valid<int16_t>(anode, MAX_CHANNELS)) return;
	const double Lhalf = 25.;  // half the length of a single side of the MCP (50/2 [mm])
	int32_t sum = 0;
	for(int i=0; i< MAX_CHANNELS; ++i) sum += anode[i];
	if(sum) {
		x = Lhalf * ( (anode[1] + anode[2]) - (anode[0] + anode[3]) ) / (double)sum;
		y = Lhalf * ( (anode[0] + anode[1]) - (anode[2] + anode[3]) ) / (double)sum;
	}
}

// ====== class dragon::MCP::Variables ====== //

dragon::MCP::Variables::Variables()
{
	for(int i=0; i< MAX_CHANNELS; ++i) {
		anode_module[i] = 1;
		anode_ch[i] = i;
	}
	tac_module = 1;
	tac_ch = MAX_CHANNELS;
}

void dragon::MCP::Variables::set(const char* odb)
{
	/// \todo Set actual ODB paths, TEST!!
	const std::string pathAnodeModule = "Equipment/MCP/Variables/AnodeModule";
	const std::string pathAnodeCh     = "Equipment/MCP/Variables/AnodeChannel";
	const std::string pathTacCh       = "Equipment/MCP/Variables/TACChannel";
	const std::string pathTacModule   = "Equipment/MCP/Variables/TACModule";

	if(strcmp(odb, "online")) { // Read from offline XML file
		midas::Xml mxml (odb);
		bool success = false;
		mxml.GetArray(pathAnodeModule.c_str(), MAX_CHANNELS, anode_module, &success);
		mxml.GetArray(pathAnodeCh.c_str(), MAX_CHANNELS, anode_ch, &success);
		mxml.GetValue(pathTacCh.c_str(), tac_ch, &success);
		mxml.GetValue(pathTacModule.c_str(), tac_module, &success);

		if(!success) {
			std::cerr << "Error (MCP::Variables::set): Couldn't set one or more variable values properly.\n";
		}
	}
	else { // Read from online ODB.
#ifdef MIDASSYS
		for(int i=0; i< MAX_CHANNELS; ++i) {
			anode_ch[i] = midas::Odb::ReadInt(pathAnodeCh.c_str(), i, 0);
			anode_module[i] = midas::Odb::ReadInt(pathAnodeModule.c_str(), i, 0);
		}
		tac_ch = midas::Odb::ReadInt(pathTacCh.c_str(), 0, 0);
		tac_module = midas::Odb::ReadInt(pathTacModule.c_str(), 0, 0);
#else
		std::cerr << "MIDASSYS not defined, can't read from online ODB, no changes made.\n";
#endif
	}
}

