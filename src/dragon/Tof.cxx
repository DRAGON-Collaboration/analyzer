/// \file Tof.cxx
/// \author G. Christian
/// \brief Implements Tof.hxx
#include "midas/Database.hxx"
#include "utils/Functions.hxx"
#include "vme/V1190.hxx"
#include "Channels.h"
#include "IonChamber.hxx"
#include "DSSSD.hxx"
#include "Bgo.hxx"
#include "MCP.hxx"
#include "Tof.hxx"



// ====== Class dragon::Xtdc ====== //

dragon::Xtdc::Xtdc(const char* odbBase):
	variables(odbBase)
{
	/*! Calls reset() */
	reset();
}

void dragon::Xtdc::reset()
{
	utils::reset_data(tcal);
}

void dragon::Xtdc::read_data(const vme::V1190& tdc)
{
	utils::channel_map(tcal, variables.tdc.channel, tdc);
}

void dragon::Xtdc::calculate()
{
	utils::linear_calibrate(tcal, variables.tdc);
}


// ====== Class dragon::Xtdc::Variables ====== //

dragon::Xtdc::Variables::Variables(const char* odbBase):
	fOdbBase(odbBase)
{
	/*! Calls reset() */
	reset();
}

void dragon::Xtdc::Variables::reset()
{
	if(fOdbBase == "/dragon/head")
		tdc.channel = TAIL_CROSS_TDC;
	else
		tdc.channel = HEAD_CROSS_TDC;
	tdc.slope   = 1.;
	tdc.offset  = 0.;
}

void dragon::Xtdc::Variables::set(const char* odb)
{
	/*!
	 * \param [in] odb_file Path of the odb file from which you are extracting variable values
	 * \todo Needs testing
	 */
	midas::Database database(odb);

	database.ReadValue((fOdbBase + "/variables/xtdc/channel").c_str(), tdc.channel);
	database.ReadValue((fOdbBase + "/variables/xtdc/slope").c_str(), tdc.slope);
	database.ReadValue((fOdbBase + "/variables/xtdc/offset").c_str(), tdc.offset);
}
	

// ====== Class dragon::TofTail ====== //

dragon::TofTail::TofTail()
{
	/*! Calls reset() */
	reset();
}

void dragon::TofTail::reset()
{
	utils::reset_data(mcp);
	
#ifndef DRAGON_OMIT_DSSSD
	utils::reset_data(mcp_dsssd);
#endif

#ifndef DRAGON_OMIT_IC
	utils::reset_data(mcp_ic);
#endif
}

void dragon::TofTail::calculate(const dragon::MCP& mcp_, const dragon::DSSSD& dsssd_, const dragon::IonChamber& ic_, const dragon::Xtdc& xover)
{
	mcp = utils::Tof::calculate(mcp_, 0, mcp_, 1);
	gamma_mcp = utils::Tof::calculate(xover, mcp_, 0);

#ifndef DRAGON_OMIT_DSSSD
	mcp_dsssd = utils::Tof::calculate(mcp_, 0, dsssd_);
	gamma_dsssd = utils::Tof::calculate(xover, dsssd_);
#endif

#ifndef DRAGON_OMIT_IC
	mcp_ic = utils::Tof::calculate(mcp_, 0, ic_);
	mcp_ic = utils::Tof::calculate(xover, ic_);
#endif
}


// ====== class dragon::TofHead ====== //

dragon::TofHead::TofHead()
{
	/*! Calls reset() */
	reset();
}

void dragon::TofHead::reset()
{
	utils::reset_data(gamma_tail);
}

void dragon::TofHead::calculate(const dragon::Bgo& bgo, const dragon::Xtdc& xover)
{
	gamma_tail = utils::Tof::calculate_bgo(bgo, xover);
}
