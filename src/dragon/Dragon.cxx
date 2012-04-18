/// \file Dragon.cxx
/// \brief Implements Dragon.hxx
#include "dragon/Dragon.hxx"


// ========== Class Dragon ========== //

dragon::Dragon::Dragon(): gamma(), hi()
{
	reset();
}

namespace {
void copy_dragon(const dragon::Dragon& from, dragon::Dragon& to)
{
	to.gamma = from.gamma;
	to.hi = from.hi;
} }

dragon::Dragon::Dragon(const dragon::Dragon& other)
{
	copy_dragon(other, *this);
}

dragon::Dragon& dragon::Dragon::operator= (const dragon::Dragon& other)
{
	copy_dragon(other, *this);
	return *this;
}

void dragon::Dragon::reset()
{
	gamma.reset();
	hi.reset();
}

void dragon::Dragon::read_event(const dragon::gamma::Gamma& head, const dragon::hion::HeavyIon& tail)
{
	gamma = head;
	hi = tail;
}
