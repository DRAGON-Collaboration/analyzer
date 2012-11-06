/// \file Tail.cxx
/// \author G. Christian
/// \brief Implements Tail.hxx
#include <cstdio>
#include <cassert>
#include <iostream>
#include "utils/Functions.hxx"
#include "midas/Database.hxx"
#include "Channels.h"
#include "Tail.hxx"


// ====== Class dragon::Tail ====== //

dragon::Tail::Tail() :
#ifndef DRAGON_OMIT_DSSSD
	dsssd(),
#endif
#ifndef DRAGON_OMIT_IC
	ic(),
#endif
	mcp(),
	sb(),
#ifndef DRAGON_OMIT_NAI
	nai(),
#endif
#ifndef DRAGON_OMIT_GE
	ge(),
#endif
	tof(this)
{
	utils::Banks::Set(banks.io32,   "TLIO");
	utils::Banks::Set(banks.adc[0], "TLQ0");
	utils::Banks::Set(banks.adc[1], "TLQ1");
	utils::Banks::Set(banks.tdc,    "TLT0");
	utils::Banks::Set(banks.tsc,    "TSCT");
	reset();
}

void dragon::Tail::reset()
{
	io32.reset();
	v1190.reset();
	for (int i=0; i< NUM_ADC; ++i) {
		v785[i].reset();
	}
#ifndef DRAGON_OMIT_DSSSD
	dsssd.reset();
#endif
#ifndef DRAGON_OMIT_IC
	ic.reset();
#endif
	mcp.reset();
	sb.reset();
#ifndef DRAGON_OMIT_NAI
	nai.reset();
#endif
#ifndef DRAGON_OMIT_GE
	ge.reset();
#endif
	tof.reset();
}

void dragon::Tail::unpack(const midas::Event& event)
{
	/*!
	 * \param [in] event Reference to a Midas event structure
	 *
	 * Here is where the low-level work is done to take bitpacked data read directly
	 * from a MIDAS file and convert it into the corresponding measurement values
	 * of various ADC modules.
	 *
	 * In the specific case of unpacking the dragon::Tail VME data, we delegate
	 * the work to the unpack() routines of the relevant vme::* classes.
	 *
	 * \note Recompile with <c> report = true </c> to print warning messages
	 *  for missing banks
	 */
	const bool report = false;
	io32.unpack (event, banks.io32, report);
	for (int i=0; i< NUM_ADC; ++i) {
		v785[i].unpack(event, banks.adc[i], report);
	}
	v1190.unpack(event, banks.tdc, report);
	event.CopyHeader(header);
}

void dragon::Tail::calculate()
{
	/// - Read data from VME modules into data structures
#ifndef DRAGON_OMIT_DSSSD
	dsssd.read_data(v785, v1190);
#endif
#ifndef DRAGON_OMIT_IC
	ic.read_data(v785, v1190);
#endif
	mcp.read_data(v785, v1190);
	sb.read_data(v785, v1190);
#ifndef DRAGON_OMIT_NAI
	nai.read_data(v785, v1190);
#endif
#ifndef DRAGON_OMIT_GE
	ge.read_data(v785, v1190);
#endif
	
	/// - Perform calibrations, higher-order calculations, etc
#ifndef DRAGON_OMIT_DSSSD
	dsssd.calculate();
#endif
#ifndef DRAGON_OMIT_IC
	ic.calculate();
#endif
	mcp.calculate();
	sb.calculate();
#ifndef DRAGON_OMIT_NAI
	nai.calculate();
#endif
#ifndef DRAGON_OMIT_GE
	ge.calculate();
#endif
	tof.calculate();
}

void dragon::Tail::set_variables(const char* odb)
{
	/*!
	 * \param [in] odb_file Name of the ODB file
	 * \note Passing \c "online" looks at the online ODB.
	 */
#ifndef DRAGON_OMIT_DSSSD
	dsssd.variables.set(odb);
#endif
#ifndef DRAGON_OMIT_IC
	ic.variables.set(odb);
#endif
	mcp.variables.set(odb);
	sb.variables.set(odb);
#ifndef DRAGON_OMIT_NAI
	nai.variables.set(odb);
#endif
#ifndef DRAGON_OMIT_GE
	ge.variables.set(odb);
#endif
	tof.variables.set(odb);

	// Set bank names
	midas::Database database(odb);
	utils::Banks::OdbSet(banks.io32,   database, "dragon/tail/bank_names/io32");
	utils::Banks::OdbSet(banks.adc[0], database, "dragon/tail/bank_names/adc[0]");
	utils::Banks::OdbSet(banks.adc[1], database, "dragon/tail/bank_names/adc[1]");
	utils::Banks::OdbSet(banks.tdc,    database, "dragon/tail/bank_names/tdc");
	utils::Banks::OdbSet(banks.tsc,    database, "dragon/tail/bank_names/tsc");
}


// ====== Class dragon::Tail::Tof ====== //

dragon::Tail::Tof::Tof(const dragon::Tail* parent):
	variables(), fParent(parent)
{
	reset();
}

void dragon::Tail::Tof::reset()
{
	utils::reset_data(tcalx, gamma_mcp, mcp);
#ifndef DRAGON_OMIT_DSSSD
	utils::reset_data(gamma_dsssd, mcp_dsssd);
#endif
#ifndef DRAGON_OMIT_IC
	utils::reset_data(gamma_ic, mcp_ic);
#endif
}

void dragon::Tail::Tof::read_data(const vme::V785[], const vme::V1190& v1190)
{
	/*!
	 * Sets xtdc from v1190 data using utils::channel_map()
	 */
	utils::channel_map(tcalx, variables.xtdc.channel, v1190);
}

void dragon::Tail::Tof::calculate()
{
	/*!
	 * Calibrates xtof (linear), calculates all the times-of-flight by subtracting
	 * <downstream tdc> - <upstream tdc>
	 */
	utils::linear_calibrate(tcalx, variables.xtdc); 
	mcp = utils::calculate_tof(fParent->mcp.tcal[1], fParent->mcp.tcal[0]);
	gamma_mcp = utils::calculate_tof(fParent->mcp.tcal[0], tcalx);
	
#ifndef DRAGON_OMIT_DSSSD
	mcp_dsssd   = utils::calculate_tof(fParent->dsssd.tcal, fParent->mcp.tcal[0]);
	gamma_dsssd = utils::calculate_tof(fParent->dsssd.tcal, tcalx);
#endif

#ifndef DRAGON_OMIT_IC
	mcp_ic   = utils::calculate_tof(fParent->ic.tcal, fParent->mcp.tcal[0]);
	gamma_ic = utils::calculate_tof(fParent->ic.tcal, tcalx);
#endif
}


// ====== Class dragon::Tail::Tof::Variables ====== //

dragon::Tail::Tof::Variables::Variables()
{
	reset();
}

void dragon::Tail::Tof::Variables::reset()
{
	xtdc.channel = TAIL_CROSS_TDC;
	xtdc.slope = 1.;
	xtdc.offset = 0.;
}

void dragon::Tail::Tof::Variables::set(const char* odb)
{
	midas::Database database(odb);
	
	database.ReadValue("/dragon/tail/variables/xtdc/channel", xtdc.channel);
	database.ReadValue("/dragon/tail/variables/xtdc/slope",   xtdc.slope);
	database.ReadValue("/dragon/tail/variables/xtdc/offset",  xtdc.offset);
}
