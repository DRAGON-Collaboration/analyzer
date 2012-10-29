/// \file Tail.cxx
/// \author G. Christian
/// \brief Implements Tail.hxx
#include <string>
#include <cstdio>
#include <cassert>
#include <iostream>
#include "midas/Database.hxx"
#include "Tail.hxx"


// ====== struct dragon::hion::Tail ====== //

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
	xtdc("/dragon/tail"),
	tof()
{
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
	xtdc.reset();
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
	 *
	 * \todo Don't hard code bank names
	 */
	const bool report = false;

	io32.unpack (event, "VTRT", report);
	
	assert (NUM_ADC < 10);
	for (int i=0; i< NUM_ADC; ++i) {
		char bkname[5];
		sprintf(bkname, "TLQ%d", i);
		v785[i].unpack (event, bkname, report);
	}

	v1190.unpack (event, "TLT0", report);
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
	xtdc.read_data(v1190);
	
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
	tof.calculate(mcp, dsssd, ic, xtdc);
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
	xtdc.variables.set(odb);
}
