/// \file Bgo.cxx
/// \brief Implements Bgo.hxx
#include <assert.h>
#include <string>
#include <iostream>
#include <algorithm>
#include "midas/Database.hxx"
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
	std::fill_n(q, Bgo::MAX_CHANNELS, dragon::NO_DATA);
	std::fill_n(t, Bgo::MAX_CHANNELS, dragon::NO_DATA);
	std::fill_n(qsort, Bgo::MAX_SORTED, dragon::NO_DATA);
	qsum = dragon::NO_DATA;
	x0   = dragon::NO_DATA;
	y0   = dragon::NO_DATA;
	z0   = dragon::NO_DATA;
}

void dragon::Bgo::read_data(const vme::V792& adc, const vme::V1190& tdc)
{
	for(int i=0; i< Bgo::MAX_CHANNELS; ++i) {
		q[i] = adc.get_data( variables.qdc_ch[i] );
		t[i] = tdc.get_data( variables.tdc_ch[i] );
	}
}

namespace { bool greater_than(int i, int j)
{
	if(i == dragon::NO_DATA) return false;
	if(j == dragon::NO_DATA) return true;
	return (i > j);
} }

void dragon::Bgo::calculate()
{
	// calculate energy-sorted array
	int16_t temp[Bgo::MAX_CHANNELS];
	std::copy(temp, temp + MAX_CHANNELS, q);
	std::sort(temp, temp + MAX_CHANNELS, ::greater_than);
	std::copy(qsort, qsort + MAX_SORTED, temp);

	// calculate qsum
	qsum = 0;
	for(int i=0; i< Bgo::MAX_CHANNELS; ++i) {
		if(!is_valid(temp[i])) break;
		qsum += (double)temp[i];
	}
	if(!qsum) qsum = dragon::NO_DATA;

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

void dragon::Bgo::Variables::set(const char* odb)
{
/// \todo Set actual ODB paths, TEST!!
	const char* const pathAdc  = "/DRAGON/Bgo/Variables/ChADC";
	const char* const pathTdc  = "/DRAGON/Bgo/Variables/ChTDC";
	const char* const pathXpos = "/DRAGON/Bgo/Variables/Xpos";
	const char* const pathYpos = "/DRAGON/Bgo/Variables/Ypos";
	const char* const pathZpos = "/DRAGON/Bgo/Variables/Zpos";

	midas::Database database(odb);
	database.ReadArray(pathAdc,  qdc_ch, MAX_CHANNELS);
	database.ReadArray(pathTdc,  tdc_ch, MAX_CHANNELS);
	database.ReadArray(pathXpos, xpos,   MAX_CHANNELS);
	database.ReadArray(pathYpos, ypos,   MAX_CHANNELS);
	database.ReadArray(pathZpos, zpos,   MAX_CHANNELS);
}

