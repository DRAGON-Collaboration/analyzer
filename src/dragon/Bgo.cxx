/// \file Bgo.cxx
/// \author G. Christian
/// \brief Implements Bgo.hxx
#include <cassert>
#include <numeric>
#include <algorithm>
#include <functional>
#include "midas/Database.hxx"
#include "utils/Functions.hxx"
#include "utils/Error.hxx"
#include "vme/V1190.hxx"
#include "vme/V792.hxx"
#include "Channels.h"
#include "Bgo.hxx"

// ========== Class dragon::Bgo ========== //

dragon::Bgo::Bgo():
	variables()
{
	if (MAX_CHANNELS < MAX_SORTED) {
		dragon::err::Error("dragon::Bgo::Bgo") 
			<< "(Fatal): Bgo::MAX_CHANNELS (==" << MAX_CHANNELS << ") < Bgo::MAX_SORTED (=="
			<< MAX_SORTED << ")" << DRAGON_ERR_FILE_LINE;
		assert(0);
	}
	reset();
}	

void dragon::Bgo::reset()
{
	utils::reset_array(MAX_CHANNELS, ecal);
	utils::reset_array(MAX_CHANNELS, tcal);
	utils::reset_array(MAX_SORTED, esort);
	utils::reset_data(sum, x0, y0, z0);
}

void dragon::Bgo::read_data(const vme::V792& adc, const vme::V1190& tdc)
{
	utils::channel_map(ecal, MAX_CHANNELS, variables.adc.channel, adc);
	utils::channel_map(tcal, MAX_CHANNELS, variables.tdc.channel, tdc);
}


namespace {
template<typename T>
bool is_invalid (T t) { return !utils::is_valid(t); } 
}

void dragon::Bgo::calculate()
{
	/*!
	 * \todo WRITE DESCRIPTION!!
	 */
	utils::pedestal_subtract(ecal, MAX_CHANNELS, variables.adc);
	utils::linear_calibrate(ecal, MAX_CHANNELS, variables.adc);

	utils::linear_calibrate(tcal, MAX_CHANNELS, variables.tdc);

	double temp[MAX_CHANNELS]; // temp array of sorted energies
	std::copy(ecal, ecal + MAX_CHANNELS, temp); // temp == ecal

	// remove invalid entries
	double *tempBegin = temp, *tempEnd = temp + MAX_CHANNELS;
	std::remove_if(tempBegin, tempEnd, is_invalid<double>);

	// put { tempBegin...tempEnd } in descending order
	std::sort(tempBegin, tempEnd, std::greater<double>());

	// copy first MAX_SORTED elements to this->esort
	const ptrdiff_t ncopy = std::max(static_cast<ptrdiff_t> (MAX_SORTED), tempEnd - tempBegin);
	std::copy(tempBegin, tempBegin + ncopy, esort);
	if (ncopy < MAX_SORTED)
		std::fill(esort + ncopy, esort + MAX_SORTED, dragon::NO_DATA);

	// calculate sum
	sum = std::accumulate(tempBegin, tempEnd, 0);
	if(sum == 0) utils::reset_data(sum);

	// calculate x0, y0, z0
	if(utils::is_valid(sum)) {
		int which = std::max_element(ecal, ecal + MAX_CHANNELS) - ecal; // which detector was max hit?
		x0 = variables.pos.x[which];
		y0 = variables.pos.y[which];
		z0 = variables.pos.z[which];
	}
}


// ========== Class dragon::Bgo::Variables ========== //

dragon::Bgo::Variables::Variables()
{
	/*! Calls reset() */
	reset();
}

void dragon::Bgo::Variables::reset()
{
	utils::index_fill(adc.channel, adc.channel + MAX_CHANNELS, BGO_ADC0);
	std::fill(adc.pedestal, adc.pedestal + MAX_CHANNELS, 0);
	std::fill(adc.offset, adc.offset + MAX_CHANNELS, 0.);
	std::fill(adc.slope, adc.slope + MAX_CHANNELS, 1.);

	utils::index_fill(tdc.channel, tdc.channel + MAX_CHANNELS, BGO_TDC0);
	std::fill(tdc.offset, tdc.offset + MAX_CHANNELS, 0.);
	std::fill(tdc.slope, tdc.slope + MAX_CHANNELS, 1.);

	std::fill(pos.x, pos.x + MAX_CHANNELS, 1.);
	std::fill(pos.y, pos.y + MAX_CHANNELS, 1.);
	std::fill(pos.z, pos.z + MAX_CHANNELS, 1.);
}

void dragon::Bgo::Variables::set(const char* odb)
{
	/*!
	 * \param [in] odb_file Path of the odb file from which you are extracting variable values
	 * \todo Needs testing
	 */
	midas::Database database(odb);

	database.ReadArray("/dragon/bgo/variables/adc/module",   adc.module,   MAX_CHANNELS);
	database.ReadArray("/dragon/bgo/variables/adc/channel",  adc.channel,  MAX_CHANNELS);
	database.ReadArray("/dragon/bgo/variables/adc/pedestal", adc.pedestal, MAX_CHANNELS);
	database.ReadArray("/dragon/bgo/variables/adc/slope",    adc.slope,    MAX_CHANNELS);
	database.ReadArray("/dragon/bgo/variables/adc/offset",   adc.offset,   MAX_CHANNELS);

	database.ReadArray("/dragon/bgo/variables/tdc/channel", tdc.channel, MAX_CHANNELS);
	database.ReadArray("/dragon/bgo/variables/tdc/slope",   tdc.slope,   MAX_CHANNELS);
	database.ReadArray("/dragon/bgo/variables/tdc/offset",  tdc.offset,  MAX_CHANNELS);

	database.ReadArray("/dragon/bgo/variables/position/x",  pos.x, MAX_CHANNELS);
	database.ReadArray("/dragon/bgo/variables/position/y",  pos.y, MAX_CHANNELS);
	database.ReadArray("/dragon/bgo/variables/position/z",  pos.z, MAX_CHANNELS);
}
