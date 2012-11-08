/// \file Bgo.cxx
/// \author G. Christian
/// \brief Implements Bgo.hxx
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
	reset();
}

void dragon::Bgo::reset()
{
	utils::reset_array(MAX_CHANNELS, ecal);
	utils::reset_array(MAX_CHANNELS, tcal);
	utils::reset_array(MAX_CHANNELS, esort);
	utils::reset_data(sum, x0, y0, z0, t0, hit0);
}

void dragon::Bgo::read_data(const vme::V792& adc, const vme::V1190& tdc)
{
	/*!
	 * Does channel mapping from ADC and TDC modules
	 * \param [in] adc Adc module
	 * \param [in] tdc Tdc module
	 */
	utils::channel_map(ecal, MAX_CHANNELS, variables.adc.channel, adc);
	utils::channel_map(tcal, MAX_CHANNELS, variables.tdc.channel, tdc);
}


void dragon::Bgo::calculate()
{
	/*!
	 * Does the following:
	 */
	/// - Pedestal subtract and calibrate energy values
	utils::pedestal_subtract(ecal, MAX_CHANNELS, variables.adc);
	utils::linear_calibrate(ecal, MAX_CHANNELS, variables.adc);

	/// - Calibrate time values
	utils::linear_calibrate(tcal, MAX_CHANNELS, variables.tdc);

	/// - Calculate descending-order energy indices and map into \c esort[]
	int isort[MAX_CHANNELS];
	utils::index_sort(ecal, ecal+MAX_CHANNELS, isort, utils::greater_and_valid<double>());
	utils::channel_map_from_array(esort, MAX_CHANNELS, isort, ecal);

	/// - If we have at least one good hit, calculate sum, x0, y0, z0, and t0
	if(utils::is_valid(esort[0])) {
		hit0 = isort[0];
		sum = utils::calculate_sum(ecal, ecal + MAX_CHANNELS);
		x0 = variables.pos.x[ isort[0] ];
		y0 = variables.pos.y[ isort[0] ];
		z0 = variables.pos.z[ isort[0] ];
		t0 = tcal[ isort[0] ];
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

	// Coordinates copied from dragon_event.c [old analyzer]
	const double bgoCoords[MAX_CHANNELS][3] =  {
		{ 4, -2.6,  9.2}, { 0, -4.8,-15.3}, { 0,-10.1,-12.2}, { 0,  5.0,-12.2}, { 0,  9.9, -9.2},
		{ 0,  8.0, -3.1}, { 0,  8.0,  3.1}, { 0,  9.9,  9.2}, { 0,-10.1, 12.2}, { 0,  5.0, 12.2},
		{ 0, -4.8, 15.3}, {-4, -2.6, -9.2}, { 4, -2.6, -9.2}, {-4, -7.9, -6.1}, { 4, -7.9, -6.1},
		{-4,  2.7, -6.1}, { 4,  2.7, -6.1}, {-4, -2.6, -3.1}, { 4, -2.6, -3.1}, {-4, -7.9,    0},
		{ 4, -7.9,    0}, {-4,  2.7,    0}, { 4,  2.7,    0}, {-4, -2.6,  3.1}, { 4, -2.6,  3.1},
		{-4, -7.9,  6.1}, { 4, -7.9,  6.1}, {-4,  2.7,  6.1}, { 4,  2.7,  6.1}, {-4, -2.6,  9.2}
	};

	for(int i=0; i< MAX_CHANNELS; ++i) {
		pos.x[i] = bgoCoords[i][0];
		pos.y[i] = bgoCoords[i][1];
		pos.z[i] = bgoCoords[i][2];
	}
}

void dragon::Bgo::Variables::set(const char* odb)
{
	/*!
	 * \param [in] odb_file Path of the odb file from which you are extracting variable values
	 */
	midas::Database database(odb);
	if(database.IsZombie()) return;

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
