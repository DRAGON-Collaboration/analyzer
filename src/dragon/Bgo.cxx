/// \file Bgo.cxx
/// \brief Implements Bgo.hxx
#include <assert.h>
#include <string>
#include <iostream>
#include <algorithm>
#include "utils/copy_array.h"
#include "midas/Odb.hxx"
#include "midas/Xml.hxx"
#include "vme/Vme.hxx"
#include "vme/V1190.hxx"
#include "vme/V792.hxx"
#include "Bgo.hxx"

// ========== Class dragon::Bgo ========== //

dragon::Bgo::Bgo():
	variables()
{
	assert(Bgo::MAX_SORTED <= Bgo::MAX_CHANNELS);
	reset();
}	

void dragon::Bgo::reset()
{
	std::fill_n(q, Bgo::MAX_CHANNELS, vme::NONE);
	std::fill_n(t, Bgo::MAX_CHANNELS, vme::NONE);
	std::fill_n(qsort, Bgo::MAX_SORTED, vme::NONE);
	qsum = vme::NONE;
	x0   = vme::NONE;
	y0   = vme::NONE;
	z0   = vme::NONE;
}

void dragon::Bgo::read_data(const vme::caen::V792& adc, const vme::caen::V1190& tdc)
{
	for(int i=0; i< Bgo::MAX_CHANNELS; ++i) {
		q[i] = adc.get_data( variables.qdc_ch[i] );
		t[i] = tdc.get_data( variables.tdc_ch[i] );
	}
}

namespace { bool greater_than(int i, int j)
{
	if(i == vme::NONE) return false;
	if(j == vme::NONE) return true;
	return (i > j);
} }

void dragon::Bgo::calculate()
{
	// calculate energy-sorted array
	int16_t temp[Bgo::MAX_CHANNELS];
	copy_array(this->q, temp, Bgo::MAX_CHANNELS);
	std::sort (temp, temp + Bgo::MAX_CHANNELS, ::greater_than);
	copy_array(temp, this->qsort, Bgo::MAX_SORTED);

	// calculate qsum
	qsum = 0;
	for(int i=0; i< Bgo::MAX_CHANNELS; ++i) {
		if(!is_valid(temp[i])) break;
		qsum += (double)temp[i];
	}
	if(!qsum) qsum = vme::NONE;

	// calculate x0, y0, z0
	if(is_valid(qsum)) {
		int which = std::max_element(q, q + Bgo::MAX_CHANNELS) - q; // which detector was max hit?
		x0 = variables.xpos[which];
		y0 = variables.ypos[which];
		z0 = variables.zpos[which];
	}
}


// ========== Class dragon::Bgo::Variables ========== //

dragon::Bgo::Variables::Variables()
{
	for(int i=0; i< Bgo::MAX_CHANNELS; ++i) {
		qdc_ch[i] = i;
		tdc_ch[i] = i;
		xpos[i]   = 0.;
		ypos[i]   = 0.;
		zpos[i]   = 0.;
	}
}

namespace {
void copy_bgo_variables(const dragon::Bgo::Variables& from, dragon::Bgo::Variables& to)
{
	copy_array(from.qdc_ch, to.qdc_ch, dragon::Bgo::MAX_CHANNELS);
	copy_array(from.tdc_ch, to.tdc_ch, dragon::Bgo::MAX_CHANNELS);

	copy_array(from.xpos, to.xpos, dragon::Bgo::MAX_CHANNELS);
	copy_array(from.ypos, to.ypos, dragon::Bgo::MAX_CHANNELS);
	copy_array(from.zpos, to.zpos, dragon::Bgo::MAX_CHANNELS);
} }

dragon::Bgo::Variables::Variables(const Variables& other)
{
	copy_bgo_variables(other, *this);
}

dragon::Bgo::Variables& dragon::Bgo::Variables::operator= (const Variables& other)
{
	copy_bgo_variables(other, *this);
	return *this;
}

void dragon::Bgo::Variables::set(const char* odb)
{
/// \todo Set actual ODB paths, TEST!!
	const std::string pathADC  = "/DRAGON/Bgo/Variables/ChADC";
	const std::string pathTDC  = "/DRAGON/Bgo/Variables/ChTDC";
	const std::string pathXpos = "/DRAGON/Bgo/Variables/Xpos";
	const std::string pathYpos = "/DRAGON/Bgo/Variables/Ypos";
	const std::string pathZpos = "/DRAGON/Bgo/Variables/Zpos";

	if(strcmp(odb, "online")) { // Read from offline XML file
		midas::Xml mxml (odb);
		bool success = false;
		mxml.GetArray(pathADC.c_str(), Bgo::MAX_CHANNELS, qdc_ch, &success);
		mxml.GetArray(pathTDC.c_str(), Bgo::MAX_CHANNELS, tdc_ch, &success);

		mxml.GetArray(pathXpos.c_str(), Bgo::MAX_CHANNELS, xpos, &success);
		mxml.GetArray(pathYpos.c_str(), Bgo::MAX_CHANNELS, ypos, &success);
		mxml.GetArray(pathZpos.c_str(), Bgo::MAX_CHANNELS, zpos, &success);

		if(!success) {
			std::cerr << "Error (Bgo::Variables::set): Couldn't set one or more variable values properly.\n";
		}
	}
	else { // Read from online ODB.
#ifdef MIDASSYS
		for(int i=0; i< dragon::Bgo::MAX_CHANNELS; ++i) {
			qdc_ch[i] = midas::Odb::ReadInt(pathADC.c_str(), i, 0);
			tdc_ch[i] = midas::Odb::ReadInt(pathTDC.c_str(), i, 0);

			xpos[i]   = midas::Odb::ReadInt(pathXpos.c_str(), i, 0);
			ypos[i]   = midas::Odb::ReadInt(pathYpos.c_str(), i, 0);
			zpos[i]   = midas::Odb::ReadInt(pathZpos.c_str(), i, 0);
		}
#else
		std::cerr << "MIDASSYS not defined, can't read from online ODB, no changes made.\n";
#endif
	}
}

