/// \file Auxillary.cxx
/// \author G. Christian
/// \brief Implements Auxillary.hxx
#include <string>
#include <iostream>
#include <algorithm>
#include "midas/Database.hxx"
#include "vme/V1190.hxx"
#include "vme/V792.hxx"
#include "Tail.hxx"
#include "Auxillary.hxx"


// ====== class dragon::NaI ====== //

dragon::NaI::NaI()
{
	reset();
}

void dragon::NaI::reset()
{
	std::fill_n(qraw, MAX_CHANNELS, dragon::NO_DATA);
	std::fill_n(qcal, MAX_CHANNELS, dragon::NO_DATA);
}

void dragon::NaI::read_data(const vme::V785 adcs[], const vme::V1190& tdc)
{
	for(int i=0; i< MAX_CHANNELS; ++i) {
		const int whichAdc = variables.module[i];
		assert (whichAdc< Tail::NUM_ADC); ///\todo Don't use an assert here
		const int whichAdcChannel = variables.ch[i];

		qraw[i] = adcs[whichAdc].get_data(whichAdcChannel);
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
	const char* const pathModule = "Equipment/NaI/Variables/Module";
	const char* const pathCh     = "Equipment/NaI/Variables/Channel";
	const char* const pathSlope  = "Equipment/NaI/Variables/Slope";
	const char* const pathOffset = "Equipment/NaI/Variables/Offset";

	midas::Database database(odb);
	database.ReadArray(pathCh, ch, MAX_CHANNELS);
	database.ReadArray(pathModule, module, MAX_CHANNELS);
	database.ReadArray(pathSlope, slope, MAX_CHANNELS);
	database.ReadArray(pathOffset, offset, MAX_CHANNELS);
}


// ====== class dragon::NaI ====== //

dragon::Ge::Ge()
{
	reset();
}

void dragon::Ge::reset()
{
	std::fill_n(qraw, MAX_CHANNELS, dragon::NO_DATA);
	std::fill_n(qcal, MAX_CHANNELS, dragon::NO_DATA);
}

void dragon::Ge::read_data(const vme::V785 adcs[], const vme::V1190& tdc)
{
	for(int i=0; i< MAX_CHANNELS; ++i) {
		const int whichAdc = variables.module[i];
		assert (whichAdc< Tail::NUM_ADC); ///\todo Don't use an assert here
		const int whichAdcChannel = variables.ch[i];

		qraw[i] = adcs[whichAdc].get_data(whichAdcChannel);
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
	const char* const pathModule = "Equipment/Ge/Variables/Module";
	const char* const pathCh     = "Equipment/Ge/Variables/Channel";
	const char* const pathSlope  = "Equipment/Ge/Variables/Slope";
	const char* const pathOffset = "Equipment/Ge/Variables/Offset";

	midas::Database database(odb);
	database.ReadArray(pathCh, ch, MAX_CHANNELS);
	database.ReadArray(pathModule, module, MAX_CHANNELS);
	database.ReadArray(pathOffset, offset, MAX_CHANNELS);
	database.ReadArray(pathSlope, slope, MAX_CHANNELS);
}
