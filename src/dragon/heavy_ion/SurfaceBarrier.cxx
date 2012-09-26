/// \file SurfaceBarrier.cxx
/// \brief Implements SurfaceBarrier.hxx
#include <string>
#include <iostream>
#include "utils/copy_array.h"
#include "dragon/heavy_ion/SurfaceBarrier.hxx"
#include "midas/Odb.hxx"
#include "midas/MidasXML.hxx"


// ====== struct dragon::hion::SurfaceBarrier ====== //

dragon::hion::SurfaceBarrier::SurfaceBarrier()
{
	reset();
}

namespace {
inline void copy_sb(const dragon::hion::SurfaceBarrier& from, dragon::hion::SurfaceBarrier& to)
{
	to.variables = from.variables;
	copy_array(from.q, to.q, dragon::hion::SurfaceBarrier::nch);
} }

dragon::hion::SurfaceBarrier::SurfaceBarrier(const dragon::hion::SurfaceBarrier& other)
{
	copy_sb(other, *this);
}

dragon::hion::SurfaceBarrier& dragon::hion::SurfaceBarrier::operator= (const dragon::hion::SurfaceBarrier& other)
{
	copy_sb(other, *this);
	return *this;
}

void dragon::hion::SurfaceBarrier::reset()
{
	for(int i=0; i< SurfaceBarrier::nch; ++i) {
		q[i] = vme::NONE;
	}
}

void dragon::hion::SurfaceBarrier::read_data(const dragon::hion::Modules& modules)
{
	for(int i=0; i< SurfaceBarrier::nch; ++i) {
		q[i] = modules.v785_data(variables.module[i], variables.ch[i]);
	}
}

// ====== struct dragon::hion::SurfaceBarrier::Variables ====== //

dragon::hion::SurfaceBarrier::Variables::Variables()
{
	for(int i=0; i< SurfaceBarrier::nch; ++i) {
		module[i] = 1;
		ch[i] = i;
	}
}

namespace {
inline void copy_sb_variables(const dragon::hion::SurfaceBarrier::Variables& from, dragon::hion::SurfaceBarrier::Variables& to)
{
	copy_array(from.module, to.module, dragon::hion::SurfaceBarrier::nch);
	copy_array(from.ch, to.ch, dragon::hion::SurfaceBarrier::nch);
} }

dragon::hion::SurfaceBarrier::Variables::Variables(const dragon::hion::SurfaceBarrier::Variables& other)
{
	copy_sb_variables(other, *this);
}

dragon::hion::SurfaceBarrier::Variables& dragon::hion::SurfaceBarrier::Variables::operator= (const dragon::hion::SurfaceBarrier::Variables& other)
{
	copy_sb_variables(other, *this);
	return *this;
}

void dragon::hion::SurfaceBarrier::Variables::set(const char* odb)
{
	/// \todo Set actual ODB paths, TEST!!
	const std::string pathModule = "Equipment/SurfaceBarrier/Variables/AnodeModule";
	const std::string pathCh     = "Equipment/SurfaceBarrier/Variables/AnodeChannel";

	if(strcmp(odb, "online")) { // Read from offline XML file
		MidasXML mxml (odb);
		bool success = false;
		mxml.GetArray(pathModule.c_str(), SurfaceBarrier::nch, module, &success);
		mxml.GetArray(pathCh.c_str(), SurfaceBarrier::nch, ch, &success);

		if(!success) {
			std::cerr << "Error (SurfaceBarrier::Variables::set): Couldn't set one or more variable values properly.\n";
		}
	}
	else { // Read from online ODB.
#ifdef MIDASSYS
		for(int i=0; i< dragon::hion::SurfaceBarrier::nch; ++i) {
			ch[i] = midas::Odb::ReadInt(pathCh.c_str(), i, 0);
			module[i] = midas::Odb::ReadInt(pathModule.c_str(), i, 0);
		}
#else
		std::cerr << "MIDASSYS not defined, can't read from online ODB, no changes made.\n";
#endif
	}
}

