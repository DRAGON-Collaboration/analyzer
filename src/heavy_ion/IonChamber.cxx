/// \file IonChamber.cxx
/// \brief Implements IonChamber.hxx
#include "heavy_ion/IonChamber.hxx"
#include "utils/copy_array.h"

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
