/// \file Bgo.cxx
/// \brief Implements Bgo.hxx
#include <assert.h>
#include <string>
#include <iostream>
#include <algorithm>
#include "Bgo.hxx"
#include "utils/copy_array.h"
#include "vme/Vme.hxx"
#include "midas/odb/Odb.hxx"
#include "midas/odb/MidasXML.hxx"

// ========== Class dragon::gamma::Bgo ========== //

dragon::gamma::Bgo::Bgo():
	variables()
{
	assert(Bgo::nsorted <= Bgo::nch);
	reset();
}	

namespace {
void copy_bgo(const dragon::gamma::Bgo& from, dragon::gamma::Bgo& to)
{
	to.variables = from.variables;
	copy_array(from.q, to.q, dragon::gamma::Bgo::nch);
	copy_array(from.t, to.t, dragon::gamma::Bgo::nch);
	copy_array(from.qsort, to.qsort, dragon::gamma::Bgo::nsorted);
	to.qsum = from.qsum;
	to.x0 = from.x0;
	to.y0 = from.y0;
	to.z0 = from.z0;
} }

dragon::gamma::Bgo::Bgo(const Bgo& other)
{
	copy_bgo(other, *this);
}

dragon::gamma::Bgo& dragon::gamma::Bgo::operator= (const Bgo& other)
{
	copy_bgo(other, *this);
	return *this;
}

void dragon::gamma::Bgo::reset()
{
	reset_array(Bgo::nch, q);
	reset_array(Bgo::nch, t);
	reset_array(Bgo::nsorted, qsort);
	reset_data(&qsum, &x0, &y0, &z0);
}

void dragon::gamma::Bgo::read_data(const dragon::gamma::Modules& modules)
{
	for(int i=0; i< Bgo::nch; ++i) {
		q[i] = modules.v792_data(variables.qdc_ch[i]);
		// t[i] = modules.v1190b_data(variables.tdc_ch[i]);
	}
}

namespace { bool greater_than(int i, int j)
{
	if(i == vme::NONE) return false;
	if(j == vme::NONE) return true;
	return (i > j);
} }

void dragon::gamma::Bgo::calculate()
{
	// calculate energy-sorted array
	int16_t temp[Bgo::nch];
	copy_array(this->q, temp, Bgo::nch);
	std::sort (temp, temp + Bgo::nch, ::greater_than);
	copy_array(temp, this->qsort, Bgo::nsorted);

	// calculate qsum
	qsum = 0;
	for(int i=0; i< Bgo::nch; ++i) {
		if(!is_valid(temp[i])) break;
		qsum += (double)temp[i];
	}
	if(!qsum) qsum = vme::NONE;

	// calculate x0, y0, z0
	if(is_valid(qsum)) {
		int which = std::max_element(q, q + Bgo::nch) - q; // which detector was max hit?
		x0 = variables.xpos[which];
		y0 = variables.ypos[which];
		z0 = variables.zpos[which];
	}
}


// ========== Class dragon::gamma::Bgo::Variables ========== //

dragon::gamma::Bgo::Variables::Variables()
{
	for(int i=0; i< Bgo::nch; ++i) {
		qdc_ch[i] = i;
		tdc_ch[i] = i;
		xpos[i]   = 0.;
		ypos[i]   = 0.;
		zpos[i]   = 0.;
	}
}

namespace {
void copy_bgo_variables(const dragon::gamma::Bgo::Variables& from, dragon::gamma::Bgo::Variables& to)
{
	copy_array(from.qdc_ch, to.qdc_ch, dragon::gamma::Bgo::nch);
	copy_array(from.tdc_ch, to.tdc_ch, dragon::gamma::Bgo::nch);

	copy_array(from.xpos, to.xpos, dragon::gamma::Bgo::nch);
	copy_array(from.ypos, to.ypos, dragon::gamma::Bgo::nch);
	copy_array(from.zpos, to.zpos, dragon::gamma::Bgo::nch);
} }

dragon::gamma::Bgo::Variables::Variables(const Variables& other)
{
	copy_bgo_variables(other, *this);
}

dragon::gamma::Bgo::Variables& dragon::gamma::Bgo::Variables::operator= (const Variables& other)
{
	copy_bgo_variables(other, *this);
	return *this;
}

void dragon::gamma::Bgo::Variables::set(const char* odb)
{
/// \todo Set actual ODB paths, TEST!!
	const std::string pathADC  = "/DRAGON/Bgo/Variables/ChADC";
	const std::string pathTDC  = "/DRAGON/Bgo/Variables/ChTDC";
	const std::string pathXpos = "/DRAGON/Bgo/Variables/Xpos";
	const std::string pathYpos = "/DRAGON/Bgo/Variables/Ypos";
	const std::string pathZpos = "/DRAGON/Bgo/Variables/Zpos";

	if(strcmp(odb, "online")) { // Read from offline XML file
		MidasXML mxml (odb);
		bool success = false;
		mxml.GetArray(pathADC.c_str(), Bgo::nch, qdc_ch, &success);
		mxml.GetArray(pathTDC.c_str(), Bgo::nch, tdc_ch, &success);

		mxml.GetArray(pathXpos.c_str(), Bgo::nch, xpos, &success);
		mxml.GetArray(pathYpos.c_str(), Bgo::nch, ypos, &success);
		mxml.GetArray(pathZpos.c_str(), Bgo::nch, zpos, &success);

		if(!success) {
			std::cerr << "Error (Bgo::Variables::set): Couldn't set one or more variable values properly.\n";
		}
	}
	else { // Read from online ODB.
#ifdef MIDASSYS
		for(int i=0; i< dragon::gamma::Bgo::nch; ++i) {
			qdc_ch[i] = odb::ReadInt(pathADC.c_str(), i, 0);
			tdc_ch[i] = odb::ReadInt(pathTDC.c_str(), i, 0);

			xpos[i] = odb::ReadInt(pathXpos.c_str(), i, 0);
			ypos[i] = odb::ReadInt(pathYpos.c_str(), i, 0);
			zpos[i] = odb::ReadInt(pathZpos.c_str(), i, 0);
		}
#else
		std::cerr << "MIDASSYS not defined, can't read from online ODB, no changes made.\n";
#endif
	}
}

