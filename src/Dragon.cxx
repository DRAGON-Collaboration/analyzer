/// \file Dragon.cxx
/// \brief Implements Dragon.hxx
#include "Dragon.hxx"


// ========== Class Dragon ========== //

dragon::Dragon::Dragon(): gamma(), hi()
{
	reset();
}

void dragon::Dragon::reset()
{
	gamma.reset();
	hi.reset();
}

void dragon::Dragon::copy(const dragon::gamma::Gamma& head, const dragon::hion::HeavyIon& tail)
{
	gamma = head;
}
