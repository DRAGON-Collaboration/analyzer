/// \file Dragon.cxx
/// \brief Implements Dragon.hxx
#include "dragon/Dragon.hxx"


// ========== Class Dragon ========== //

dragon::Dragon::Dragon():
	head(), hi()
{
	reset();
}

void dragon::Dragon::reset()
{
	head.reset();
	hi.reset();
}

void dragon::Dragon::set_variables(const char* odb)
{
	head.set_variables(odb);
	hi.set_variables(odb);
}

void dragon::Dragon::read_event(const dragon::Head& head_, const dragon::hion::HeavyIon& tail_)
{
	head = head_;
	hi = tail_;
}

































// DOXYGEN MAINPAGE //

/*!
	\mainpage
	\authors G. Christian
	\authors C. Stanford

	\todo Write main page





 */
