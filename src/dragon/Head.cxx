/// \file Head.cxx
/// \author G. Christian
/// \brief Implements Head.hxx
#include "midas/Database.hxx"
#include "utils/Functions.hxx"
#include "Channels.h"
#include "Head.hxx"


// ========== Class dragon::Head ========== //

dragon::Head::Head() :
	header(), io32(), v792(), v1190(), bgo(), tof(this)
{
	utils::Banks::Set(banks.io32, "VRTH");
	utils::Banks::Set(banks.adc,  "ADC0");
	utils::Banks::Set(banks.tdc,  "TDC0");
	utils::Banks::Set(banks.tsc,  "TSCH");
	reset();
}

void dragon::Head::reset()
{
	io32.reset();
	v792.reset();
	v1190.reset();
	bgo.reset();
	tof.reset();
	const midas::Event::Header temp = { 0, 0, 0, 0, 0 };
	header = temp;
}

void dragon::Head::set_variables(const char* odb)
{
	/*!
	 * \param [in] odb_file Name of the ODB file
	 * \note Passing \c "online" looks at the online ODB.
	 */
	bgo.variables.set(odb);
	tof.variables.set(odb);

	// Set bank names
	midas::Database database(odb);
	if(database.IsZombie()) return;

	utils::Banks::OdbSet(banks.io32, database, "dragon/head/bank_names/io32");
	utils::Banks::OdbSet(banks.adc,  database, "dragon/head/bank_names/adc");
	utils::Banks::OdbSet(banks.tdc,  database, "dragon/head/bank_names/tdc");
	utils::Banks::OdbSet(banks.tsc,  database, "dragon/head/bank_names/tsc");
}

void dragon::Head::unpack(const midas::Event& event)
{
	/*!
	 * \param [in] event Reference to a Midas event structure
	 *
	 * Here is where the low-level work is done to take bitpacked data read directly
	 * from a MIDAS file and convert it into the corresponding measurement values
	 * of various ADC modules.
	 *
	 * In the specific case of unpacking the dragon::Head VME data, we delegate
	 * the work to the unpack() routines of the relevant vme::* classes.
	 *
	 * \note Recompile with <c> report = true </c> to print warning messages
	 *  for missing banks
	 */
	const bool report = false;
	io32.unpack (event, banks.io32, report);
	v792.unpack (event, banks.adc , report);
	v1190.unpack(event, banks.tdc,  report);
	event.CopyHeader(header);
}

void dragon::Head::calculate()
{
	/*!
	 * This is where the raw data recorded in VME fields is mapped to a more
	 * abstract location in a detector class. This covers a wide range of parameter
	 * calculation, from, e.g. simply mapping ADC channels to corresponding (uncalibrated)
	 * detector signals, up to calculation of abstract physics quantities that depend on multiple
	 * calibrated detector signals.
	 *
	 * In the specific implementation, we delegate to functions in the dragon::Bgo class.
	 */
	bgo.read_data(v792, v1190);
	tof.read_data(v792, v1190);
	bgo.calculate();
	tof.calculate();
}


// ====== Class dragon::Head::Tof ====== //

dragon::Head::Tof::Tof(const dragon::Head* parent):
	variables(), fParent(parent)
{
	reset();
}

void dragon::Head::Tof::reset()
{
	utils::reset_data(tcalx, gamma_tail);
}

void dragon::Head::Tof::read_data(const vme::V785&, const vme::V1190& v1190)
{
	/*!
	 * Sets xtdc from v1190 data using utils::channel_map()
	 */
	utils::channel_map(tcalx, variables.xtdc.channel, v1190);
}

void dragon::Head::Tof::calculate()
{
	/*!
	 * Calibrates xtof (linear), calculates all the times-of-flight by subtracting
	 * < downstream tdc > - < upstream tdc >
	 */
	utils::linear_calibrate(tcalx, variables.xtdc); 
	gamma_tail = utils::calculate_tof(tcalx, fParent->bgo.t0);
}


// ====== Class dragon::Head::Tof::Variables ====== //

dragon::Head::Tof::Variables::Variables()
{
	reset();
}

void dragon::Head::Tof::Variables::reset()
{
	xtdc.channel = HEAD_CROSS_TDC;
	xtdc.slope = 1.;
	xtdc.offset = 0.;
}

void dragon::Head::Tof::Variables::set(const char* odb)
{
	midas::Database database(odb);
	if(database.IsZombie()) return;

	database.ReadValue("/dragon/head/variables/xtdc/channel", xtdc.channel);
	database.ReadValue("/dragon/head/variables/xtdc/slope",   xtdc.slope);
	database.ReadValue("/dragon/head/variables/xtdc/offset",  xtdc.offset);
}
