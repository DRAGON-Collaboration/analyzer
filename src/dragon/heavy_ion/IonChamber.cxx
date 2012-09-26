/// \file IonChamber.cxx
/// \brief Implements IonChamber.hxx
#include <string>
#include <iostream>
#include "utils/copy_array.h"
#include "dragon/heavy_ion/IonChamber.hxx"
#include "midas/Odb.hxx"
#include "midas/MidasXML.hxx"


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
	to.sum = from.sum;
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
	sum = vme::NONE;
}

void dragon::hion::IonChamber::read_data(const dragon::hion::Modules& modules, int v1190_trigger_ch)
{
	for(int i=0; i< IonChamber::nch; ++i) {
		anode[i] = modules.v785_data(variables.anode_module[i], variables.anode_ch[i]);
	}
	// tof = modules.v1190b_data(variables.tof_ch) - modules.v1190b_data(v1190_trigger_ch);
}

void dragon::hion::IonChamber::calculate()
{
	if(!is_valid<int16_t>(anode, IonChamber::nch)) return;
	sum = 0;
	for(int i=0; i< IonChamber::nch; ++i) {
		sum += (double)anode[i];
	}
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

void dragon::hion::IonChamber::Variables::set(const char* odb)
{
	/// \todo Set actual ODB paths, TEST!!
	const std::string pathAnodeModule = "Equipment/IonChamber/Variables/AnodeModule";
	const std::string pathAnodeCh     = "Equipment/IonChamber/Variables/AnodeChannel";
	const std::string pathTdcCh       = "Equipment/IonChamber/Variables/TDCChannel";

	if(strcmp(odb, "online")) { // Read from offline XML file
		MidasXML mxml (odb);
		bool success = false;
		mxml.GetArray(pathAnodeModule.c_str(), IonChamber::nch, anode_ch, &success);
		mxml.GetArray(pathAnodeCh.c_str(), IonChamber::nch, anode_module, &success);
		mxml.GetValue(pathTdcCh.c_str(), tof_ch, &success);

		if(!success) {
			std::cerr << "Error (IonChamber::Variables::set): Couldn't set one or more variable values properly.\n";
		}
	}
	else { // Read from online ODB.
#ifdef MIDASSYS
		for(int i=0; i< dragon::hion::IonChamber::nch; ++i) {
			anode_ch[i] = midas::Odb::ReadInt(pathAnodeCh.c_str(), i, 0);
			anode_module[i] = midas::Odb::ReadInt(pathAnodeModule.c_str(), i, 0);
		}
		tof_ch = midas::Odb::ReadInt(pathTdcCh.c_str(), 0, 0);
#else
		std::cerr << "MIDASSYS not defined, can't read from online ODB, no changes made.\n";
#endif
	}
}
