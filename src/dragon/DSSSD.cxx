/// \file DSSSD.cxx
/// \brief Implements DSSSD.hxx
#include <string>
#include <iostream>
#include "utils/copy_array.h"
#include "vme/Vme.hxx"
#include "midas/Odb.hxx"
#include "midas/Xml.hxx"
#include "DSSSD.hxx"


// ====== class dragon::DSSSD ====== //

dragon::DSSSD::DSSSD() :
	variables()
{
	reset();
}

void dragon::DSSSD::reset()
{
	for(int i=0; i< MAX_CHANNELS; ++i) {
		qraw[i] = vme::NONE;
	}
	tof = vme::NONE;
}

void dragon::DSSSD::read_data(const dragon::hion::Modules& modules, int v1190_trigger_ch)
{
	for(int i=0; i< MAX_CHANNELS; ++i) {
		qraw[i] = modules.v785_data(variables.qdc_module[i], variables.qdc_ch[i]);
	}
	// tof = modules.v1190b_data(variables.tof_ch) - modules.v1190b_data(v1190_trigger_ch);
}


// ====== class dragon::DSSSD::Variables ====== //

dragon::DSSSD::Variables::Variables()
{
	for(int i=0; i< MAX_CHANNELS; ++i) {
		qdc_module[i] = 1;
		qdc_ch[i] = i;
	}
	tof_ch = 1; /// \todo Update once plugged in
}

void dragon::DSSSD::Variables::set(const char* odb)
{
	/// \todo Set actual ODB paths, TEST!!
	const std::string pathAdcModule = "Equipment/DSSSD/Variables/ADCModule";
	const std::string pathAdcCh     = "Equipment/DSSSD/Variables/ADCChannel";
	const std::string pathTdcCh     = "Equipment/DSSSD/Variables/TDCChannel";

	if(strcmp(odb, "online")) { // Read from offline XML file
		midas::Xml mxml (odb);
		bool success = false;
		mxml.GetArray(pathAdcModule.c_str(), MAX_CHANNELS, qdc_module, &success);
		mxml.GetArray(pathAdcCh.c_str(), MAX_CHANNELS, qdc_ch, &success);
		mxml.GetValue(pathTdcCh.c_str(), tof_ch, &success);

		if(!success) {
			std::cerr << "Error (DSSSD::Variables::set): Couldn't set one or more variable values properly.\n";
		}
	}
	else { // Read from online ODB.
#ifdef MIDASSYS
		for(int i=0; i< MAX_CHANNELS; ++i) {
			qdc_ch[i] = midas::Odb::ReadInt(pathAdcCh.c_str(), i, 0);
			qdc_module[i] = midas::Odb::ReadInt(pathAdcModule.c_str(), i, 0);
		}
		tof_ch = midas::Odb::ReadInt(pathTdcCh.c_str(), 0, 0);
#else
		std::cerr << "MIDASSYS not defined, can't read from online ODB, no changes made.\n";
#endif
	}
}
