/// \file Bgo.cxx
/// \author G. Christian
/// \brief Implements Bgo.hxx
#include <cassert>
#include <numeric>
#include <algorithm>
#include <functional>
#include "midas/Database.hxx"
#include "utils/Error.hxx"
#include "vme/V1190.hxx"
#include "vme/V792.hxx"
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
	utils::reset_array(MAX_CHANNELS, q);
	utils::reset_array(MAX_CHANNELS, t);
	utils::reset_array(MAX_SORTED, qsort);
	utils::reset_data(qsum, x0, y0, z0);
}

void dragon::Bgo::read_data(const vme::V792& adc, const vme::V1190& tdc)
{
	for(int i=0; i< Bgo::MAX_CHANNELS; ++i) {
		q[i] = adc.get_data( variables.qdc_ch[i] );
		t[i] = tdc.get_data( variables.tdc_ch[i] );
	}
}


namespace {
template<typename T>
bool is_invalid (T t) { return !utils::is_valid(t); } 
}

void dragon::Bgo::calculate()
{
	int16_t temp[MAX_CHANNELS]; // temp array of sorted energies
	std::copy(this->q, this->q + MAX_CHANNELS, temp); // temp == q

	// remove invalid entries
	int16_t *tempBegin = temp, *tempEnd = temp + MAX_CHANNELS;
	std::remove_if(tempBegin, tempEnd, is_invalid<int16_t>);

	// put { tempBegin...tempEnd } in descending order
	std::sort(tempBegin, tempEnd, std::greater<int16_t>());

	// copy first MAX_SORTED elements to this->qsort
	const ptrdiff_t ncopy = std::max(static_cast<ptrdiff_t> (MAX_SORTED), tempEnd - tempBegin);
	std::copy(tempBegin, tempBegin + ncopy, this->qsort);
	if (ncopy < MAX_SORTED)
		std::fill(this->qsort + ncopy, this->qsort + MAX_SORTED, dragon::NO_DATA);

	// calculate qsum
	qsum = std::accumulate(tempBegin, tempEnd, 0);
	if(qsum == 0) qsum = dragon::NO_DATA;

	// calculate x0, y0, z0
	if(utils::is_valid(qsum)) {
		int which = std::max_element(q, q + MAX_CHANNELS) - q; // which detector was max hit?
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
	const char* const pathAdc  = "/Dragon/Bgo/Variables/Adc_channel";
	const char* const pathTdc  = "/Dragon/Bgo/Variables/Tdc_channel";
	const char* const pathXpos = "/Dragon/Bgo/Variables/Xpos";
	const char* const pathYpos = "/Dragon/Bgo/Variables/Ypos";
	const char* const pathZpos = "/Dragon/Bgo/Variables/Zpos";

	midas::Database database(odb);
	database.ReadArray(pathAdc,  qdc_ch, MAX_CHANNELS);
	database.ReadArray(pathTdc,  tdc_ch, MAX_CHANNELS);
	database.ReadArray(pathXpos, xpos,   MAX_CHANNELS);
	database.ReadArray(pathYpos, ypos,   MAX_CHANNELS);
	database.ReadArray(pathZpos, zpos,   MAX_CHANNELS);
}
