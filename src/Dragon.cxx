/// \file Dragon.cxx
/// \brief Implements Dragon.hxx
#include "Dragon.hxx"


// ========== Class Dragon ========== //

dragon::Dragon::Dragon(): modules(), gamma(), hi()
{
	reset();
}

void dragon::Dragon::reset()
{
	gamma.reset();
	hi.reset();
}

void dragon::Dragon::set_variables(const char* odb_file)
{
	gamma.bgo.variables.set(odb_file);
}

void dragon::Dragon::unpack(TMidasEvent& event)
{
	reset();
	modules.gamma.unpack(event);
	modules.heavy_ion.unpack(event);
	gamma.read_data();
}

void dragon::Dragon::calibrate()
{
	// nothing to do so far
}
