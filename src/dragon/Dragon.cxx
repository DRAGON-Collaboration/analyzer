/// \file Dragon.cxx
/// \author G. Christian
/// \brief Implements Dragon.hxx
#include "Dragon.hxx"


// ========== Class Dragon ========== //

dragon::Dragon::Dragon():
	head(), tail()
{
	reset();
}

void dragon::Dragon::reset()
{
	head.reset();
	tail.reset();
}

void dragon::Dragon::set_variables(const char* odb)
{
	/*!
	 * \param [in] odb_file Name of the ODB file (*.xml or *.mid) from which to read
	 * \note Passing \c "online" looks at the experiment's ODB, if connected
	 */
	head.set_variables(odb);
	tail.set_variables(odb);
}

void dragon::Dragon::read_event(const dragon::Head& head_, const dragon::Tail& tail_)
{
	/*!
	 * \param [in] head_ Head (gamma-ray) event
	 * \param [in] tail_ Tail (heavy-ion) event
	 */
	head = head_;
	tail = tail_;
}

































// DOXYGEN MAINPAGE //

/*!
	\mainpage
	\authors G. Christian
	\authors C. Stanford

	\todo Write main page





 */
