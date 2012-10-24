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
	sb()
#ifndef DRAGON_OMIT_NAI
	, nai()
#endif
#ifndef DRAGON_OMIT_GE
	, ge()
#endif
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

// void dragon::Tail::read_data()
// {
// 	++evt_count;
// #ifndef DRAGON_OMIT_DSSSD
// 	dsssd.read_data(modules, variables.v1190_trigger_ch);
// #endif
// #ifndef DRAGON_OMIT_IC
// 	ic.read_data(modules, variables.v1190_trigger_ch);
// #endif
// 	mcp.read_data(modules);
// 	sb.read_data(modules);
// #ifndef DRAGON_OMIT_NAI
// 	nai.read_data(modules);
// #endif
// #ifndef DRAGON_OMIT_GE
// 	ge.read_data(modules);
// #endif
// }

void dragon::Tail::calculate()
{
// 	mcp.read_data(
// 	mcp.calculate();

// #ifndef DRAGON_OMIT_NAI
// 	nai.calculate();
// #endif

// #ifndef DRAGON_OMIT_GE
// 	ge.calculate();
// #endif

}

// ====== struct dragon::Tail::Variables ====== //

dragon::Tail::Variables::Variables() :
	v1190_trigger_ch(0)
{
	// nothing else to do
}

void dragon::Tail::Variables::set(const char* odb)
{
	/*!
	 * \param [in] odb_file Path of the odb file from which you are extracting variable values
	 * \todo Needs to be implemented once ODB is set up
	 */
	/// \todo Set actual ODB paths, TEST!!
	const char* const path = "Equipment/V1190/HeavyIon/TriggerCh";

	midas::Database database(odb);
	database.ReadValue(path, v1190_trigger_ch);
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
	variables.set(odb);
#ifndef DRAGON_OMIT_NAI
	nai.variables.set(odb);
#endif
#ifndef DRAGON_OMIT_GE
	ge.variables.set(odb);
#endif
}
