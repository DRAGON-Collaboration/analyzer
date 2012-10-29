/// \file Head.cxx
/// \author G. Christian
/// \brief Implements Head.hxx
#include "Head.hxx"


// ========== Class dragon::Head ========== //

dragon::Head::Head() :
	header(), io32(), v792(), v1190(), bgo(), xtdc("/dragon/head"), tof()
{
	reset();
}

void dragon::Head::reset()
{
	io32.reset();
	v792.reset();
	v1190.reset();
	bgo.reset();
	xtdc.reset();
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
	xtdc.variables.set(odb);
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
	 *
	 * \todo Don't hard code bank names
	 */
	const bool report = false;
	io32.unpack (event, "VRTH", report);
	v792.unpack (event, "ADC0", report);
	v1190.unpack(event, "TDC0", report);
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
	xtdc.read_data(v1190);
	bgo.calculate();
	xtdc.calculate();
	tof.calculate(bgo, xtdc);
}
