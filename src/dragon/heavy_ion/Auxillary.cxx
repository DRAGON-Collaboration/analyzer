//! \file Auxillary.cxx
//! \brief Implements Auxillary.hxx
#include <string>
#include <iostream>
#include "utils/copy_array.h"
#include "dragon/heavy_ion/Auxillary.hxx"
#include "odb/Odb.hxx"
#include "odb/MidasXML.hxx"


// ====== struct dragon::hion::NaI ====== //

dragon::hion::NaI::NaI()
{
	reset();
}

namespace {
inline void copy_nai(const dragon::hion::NaI& from, const dragon::hion::NaI& to)
{
	to.variables = from.variables;
	copy_array(from.qraw, to.qraw, dragon::hion::NaI::nch);
	copy_array(from.qcal, to.qcal, dragon::hion::NaI::nch);
} }

dragon::hion::NaI::NaI(const dragon::hion::NaI& other)
{
	copy_nai(other, *this);
}

dragon::hion::NaI& dragon::hion::NaI::operator= (const dragon::hion::NaI& other)
{
	copy_nai(other, *this);
	return *this;
}

void dragon::hion::NaI::reset()
{
	for(int i=0; i< NaI::nch; ++i) {
		qraw[i] = vme::NONE;
		qcal[i] = (double)vme::NONE;
	}
}

void dragon::hion::NaI::read_data(const dragon::hion::Modules& modules)
{
	for(int i=0; i< NaI::nch; ++i) {
		qraw[i] = modules.v785_data(variables.module[i], variables.ch[i]);
	}
}

void dragon::hion::NaI::caculate()
{
	for(int i=0; i< NaI::nch; ++i) {
		if(!is_valid(qraw[i])) continue;
		qcal[i] = variables.slope[i] * qraw[i] + variables.offset[i];
	}		
}


// ====== struct dragon::hion::NaI::Variables ====== //

dragon::hion::NaI::Variables::Variables()
{
	for(int i=0; i< NaI::nch; ++i) {
		module[i] = 1;
		ch[i] = i;
		slope[i]  = 1.;
		offset[i] = 0.;
	}
}

namespace {
inline void copy_nai_variables(const dragon::hion::NaI::Variables& from, const dragon::hion::NaI::Variables& to)
{
	copy_array(from.module, to.module, dragon::hion::NaI::nch);
	copy_array(from.ch,     to.ch,     dragon::hion::NaI::nch);
	copy_array(from.slope,  to.slope,  dragon::hion::NaI::nch);
	copy_array(from.offset, to.offset, dragon::hion::NaI::nch);
} }

dragon::hion::NaI::Variables::Variables(const dragon::hion::NaI::Variables& other)
{
	copy_nai_variables(other, *this);
}

dragon::hion::NaI::Variables::Variables& dragon::hion::NaI::Variables::operator= (const dragon::hion::NaI::Variables& other)
{
	copy_nai_variables(other, *this);
	return *this;
}

void dragon::hion::NaI::Variables::Set(const char* odb)
{
	/// \todo Set actual ODB paths, TEST!!
	const std::string pathModule = "Equipment/NaI/Variables/Module";
	const std::string pathCh     = "Equipment/NaI/Variables/Channel";
	const std::string pathSlope  = "Equipment/NaI/Variables/Slope";
	const std::string pathOffset = "Equipment/NaI/Variables/Offset";
	if(strcmp(odb, "online")) { // Read from offline XML file
		MidasXML mxml (odb);
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

		copy_array(&chOdb[0], ch, dragon::hion::SurfaceBarrier::nch);
		copy_array(&moduleOdb[0], module, dragon::hion::SurfaceBarrier::nch);

		copy_array(&chSlope, slope, dragon::hion::NaI::nch);
		copy_array(&chOffset, offset, dragon::hion::NaI::nch);

	}
	else { // Read from online ODB.
#ifdef MIDASSYS
		for(int i=0; i< dragon::hion::SurfaceBarrier::nch; ++i) {
			ch[i]     = odb::ReadInt(pathCh.c_str(), i, 0);
			module[i] = odb::ReadInt(pathModule.c_str(), i, 0);
			slope[i]  = odb::ReadDouble(pathSlope.c_str(), i, 0);
			offset[i] = odb::ReadDouble(pathOffset.c_str(), i, 0);
		}
#else
		std::cerr << "MIDASSYS not defined, can't read from online ODB, no changes made.\n";
#endif
	}
}
