/// \file Auxillary.cxx
/// \author G. Christian
/// \brief Implements Auxillary.hxx
#include <string>
#include <iostream>
#include "utils/copy_array.h"
#include "midas/Odb.hxx"
#include "midas/Xml.hxx"
#include "Auxillary.hxx"


// ====== class dragon::NaI ====== //

dragon::NaI::NaI()
{
	reset();
}

void dragon::NaI::reset()
{
	for(int i=0; i< MAX_CHANNELS; ++i) {
		qraw[i] = vme::NONE;
		qcal[i] = (double)vme::NONE;
	}
}

void dragon::NaI::read_data(const dragon::hion::Modules& modules)
{
	for(int i=0; i< MAX_CHANNELS; ++i) {
		qraw[i] = modules.v785_data(variables.module[i], variables.ch[i]);
	}
}

void dragon::NaI::calculate()
{
	for(int i=0; i< MAX_CHANNELS; ++i) {
		if(!is_valid(qraw[i])) continue;
		qcal[i] = variables.slope[i] * qraw[i] + variables.offset[i];
	}		
}


// ====== class dragon::NaI::Variables ====== //

dragon::NaI::Variables::Variables()
{
	for(int i=0; i< MAX_CHANNELS; ++i) {
		module[i] = 1;
		ch[i] = i;
		slope[i]  = 1.;
		offset[i] = 0.;
	}
}

void dragon::NaI::Variables::set(const char* odb)
{
	/// \todo Set actual ODB paths, TEST!!
	const std::string pathModule = "Equipment/NaI/Variables/Module";
	const std::string pathCh     = "Equipment/NaI/Variables/Channel";
	const std::string pathSlope  = "Equipment/NaI/Variables/Slope";
	const std::string pathOffset = "Equipment/NaI/Variables/Offset";
	if(strcmp(odb, "online")) { // Read from offline XML file
		midas::Xml mxml (odb);
		bool success = false;

		std::vector<int> chOdb, moduleOdb;
		mxml.GetArray(pathModule.c_str(), moduleOdb, &success);
		mxml.GetArray(pathCh.c_str(), chOdb, &success);

		std::vector<double> slopeOdb, offsetOdb;
		mxml.GetArray(pathSlope.c_str(), slopeOdb, &success);
		mxml.GetArray(pathOffset.c_str(), offsetOdb, &success);

		if(!success) {
			std::cerr << "Failure reading variable values from the odb file, no changes made.\n";
			return;
		}

		copy_array(&chOdb[0], ch, MAX_CHANNELS);
		copy_array(&moduleOdb[0], module, MAX_CHANNELS);

		copy_array(&slopeOdb[0], slope, MAX_CHANNELS);
		copy_array(&offsetOdb[0], offset, MAX_CHANNELS);

	}
	else { // Read from online ODB.
#ifdef MIDASSYS
		for(int i=0; i< MAX_CHANNELS; ++i) {
			ch[i]     = midas::Odb::ReadInt(pathCh.c_str(), i, 0);
			module[i] = midas::Odb::ReadInt(pathModule.c_str(), i, 0);
			slope[i]  = midas::Odb::ReadDouble(pathSlope.c_str(), i, 0);
			offset[i] = midas::Odb::ReadDouble(pathOffset.c_str(), i, 0);
		}
#else
		std::cerr << "MIDASSYS not defined, can't read from online ODB, no changes made.\n";
#endif
	}
}


// ====== class dragon::NaI ====== //

dragon::Ge::Ge()
{
	reset();
}

void dragon::Ge::reset()
{
	for(int i=0; i< MAX_CHANNELS; ++i) {
		qraw[i] = vme::NONE;
		qcal[i] = (double)vme::NONE;
	}
}

void dragon::Ge::read_data(const dragon::hion::Modules& modules)
{
	for(int i=0; i< MAX_CHANNELS; ++i) {
		qraw[i] = modules.v785_data(variables.module[i], variables.ch[i]);
	}
}

void dragon::Ge::calculate()
{
	for(int i=0; i< MAX_CHANNELS; ++i) {
		if(!is_valid(qraw[i])) continue;
		qcal[i] = variables.slope[i] * qraw[i] + variables.offset[i];
	}		
}


// ====== class dragon::Ge::Variables ====== //

dragon::Ge::Variables::Variables()
{
	for(int i=0; i< MAX_CHANNELS; ++i) {
		module[i] = 1;
		ch[i] = i;
		slope[i]  = 1.;
		offset[i] = 0.;
	}
}

void dragon::Ge::Variables::set(const char* odb)
{
	/// \todo Set actual ODB paths, TEST!!
	const std::string pathModule = "Equipment/Ge/Variables/Module";
	const std::string pathCh     = "Equipment/Ge/Variables/Channel";
	const std::string pathSlope  = "Equipment/Ge/Variables/Slope";
	const std::string pathOffset = "Equipment/Ge/Variables/Offset";
	if(strcmp(odb, "online")) { // Read from offline XML file
		midas::Xml mxml (odb);
		bool success = false;

		std::vector<int> chOdb, moduleOdb;
		mxml.GetArray(pathModule.c_str(), moduleOdb, &success);
		mxml.GetArray(pathCh.c_str(), chOdb, &success);

		std::vector<double> slopeOdb, offsetOdb;
		mxml.GetArray(pathSlope.c_str(), slopeOdb, &success);
		mxml.GetArray(pathOffset.c_str(), offsetOdb, &success);

		if(!success) {
			std::cerr << "Failure reading variable values from the odb file, no changes made.\n";
			return;
		}

		copy_array(&chOdb[0], ch, MAX_CHANNELS);
		copy_array(&moduleOdb[0], module, MAX_CHANNELS);

		copy_array(&slopeOdb[0], slope, MAX_CHANNELS);
		copy_array(&offsetOdb[0], offset, MAX_CHANNELS);

	}
	else { // Read from online ODB.
#ifdef MIDASSYS
		for(int i=0; i< MAX_CHANNELS; ++i) {
			ch[i]     = midas::Odb::ReadInt(pathCh.c_str(), i, 0);
			module[i] = midas::Odb::ReadInt(pathModule.c_str(), i, 0);
			slope[i]  = midas::Odb::ReadDouble(pathSlope.c_str(), i, 0);
			offset[i] = midas::Odb::ReadDouble(pathOffset.c_str(), i, 0);
		}
#else
		std::cerr << "MIDASSYS not defined, can't read from online ODB, no changes made.\n";
#endif
	}
}
