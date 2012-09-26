/// \file DSSSD.cxx
/// \brief Implements DSSSD.hxx
#include <string>
#include <iostream>
#include "utils/copy_array.h"
#include "dragon/heavy_ion/DSSSD.hxx"
#include "vme/Vme.hxx"
#include "midas/Odb.hxx"
#include "midas/MidasXML.hxx"


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
	// tof = modules.v1190b_data(variables.tof_ch) - modules.v1190b_data(v1190_trigger_ch);
}


// ====== struct dragon::hion::DSSSD::Variables ====== //

dragon::hion::DSSSD::Variables::Variables()
{
	for(int i=0; i< DSSSD::nch; ++i) {
		qdc_module[i] = 1;
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

void dragon::hion::DSSSD::Variables::set(const char* odb)
{
	/// \todo Set actual ODB paths, TEST!!
	const std::string pathAdcModule = "Equipment/DSSSD/Variables/ADCModule";
	const std::string pathAdcCh     = "Equipment/DSSSD/Variables/ADCChannel";
	const std::string pathTdcCh     = "Equipment/DSSSD/Variables/TDCChannel";

	if(strcmp(odb, "online")) { // Read from offline XML file
		MidasXML mxml (odb);
		bool success = false;
		mxml.GetArray(pathAdcModule.c_str(), DSSSD::nch, qdc_module, &success);
		mxml.GetArray(pathAdcCh.c_str(), DSSSD::nch, qdc_ch, &success);
		mxml.GetValue(pathTdcCh.c_str(), tof_ch, &success);

		if(!success) {
			std::cerr << "Error (DSSSD::Variables::set): Couldn't set one or more variable values properly.\n";
		}
	}
	else { // Read from online ODB.
#ifdef MIDASSYS
		for(int i=0; i< dragon::hion::DSSSD::nch; ++i) {
			qdc_ch[i] = midas::Odb::ReadInt(pathAdcCh.c_str(), i, 0);
			qdc_module[i] = midas::Odb::ReadInt(pathAdcModule.c_str(), i, 0);
		}
		tof_ch = midas::Odb::ReadInt(pathTdcCh.c_str(), 0, 0);
#else
		std::cerr << "MIDASSYS not defined, can't read from online ODB, no changes made.\n";
#endif
	}
}
