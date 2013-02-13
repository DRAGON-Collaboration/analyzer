///
/// \file Coinc.cxx
/// \author G. Christian
/// \brief Implements Coinc.hxx
///
#include "utils/definitions.h"
#include "utils/ErrorDragon.hxx"
#include "utils/Valid.hxx"
#include "utils/Functions.hxx"
#include "Coinc.hxx"


// ========== Class dragon::Coinc ========== //

dragon::Coinc::Coinc():
	head(), tail()
{
	reset();
}

dragon::Coinc::Coinc(const dragon::Head& head, const dragon::Tail& tail)
{
	compose_event(head, tail);
}

void dragon::Coinc::reset()
{
	head.reset();
	tail.reset();
	utils::reset_data(xtrig);
}

void dragon::Coinc::set_variables(const char* odb)
{
	/*!
	 * \param [in] odb_file Name of the ODB file (*.xml or *.mid) from which to read
	 * \note Passing \c "online" looks at the experiment's ODB, if connected
	 */
	head.set_variables(odb);
	tail.set_variables(odb);
}

void dragon::Coinc::compose_event(const dragon::Head& head_, const dragon::Tail& tail_)
{
	/*!
	 * \param [in] head_ Head (gamma-ray) event
	 * \param [in] tail_ Tail (heavy-ion) event
	 */
	head = head_;
	tail = tail_;

	try {
		uint32_t tTail = tail.io32.tsc4.fifo[0].at(0) & 0x3fffffff;
		uint32_t tHead = head.io32.tsc4.fifo[0].at(0) & 0x3fffffff;
		xtrig = utils::time_diff30(tTail, tHead) / DRAGON_TSC_FREQ;
	} catch (std::exception& e) {
		std::cerr << "Problem: " << e.what() << " at: " << DRAGON_ERR_FILE_LINE;
		throw (e);
	}
}

void dragon::Coinc::unpack(const midas::CoincEvent& coincEvent)
{
	/*!
	 * Calls the respective unpack() functions on the head and tail part
	 * of the coincidence events.
	 */
	head.unpack( *(coincEvent.fGamma) );
	tail.unpack( *(coincEvent.fHeavyIon) );
	xtrig = coincEvent.xtrig;
}

void dragon::Coinc::calculate()
{
	/*!
	 * Does head and tail calculations.
	 */
	head.calculate();
	tail.calculate();
}
