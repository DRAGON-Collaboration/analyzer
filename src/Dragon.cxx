#include "Dragon.hxx"

// ========== Class Dragon ========== //

dragon::Dragon::Dragon(): gamma_modules(), hi_modules(), bgo()
{
	reset();
}

void dragon::Dragon::reset()
{
	bgo.reset();
}

void dragon::Dragon::set_variables(const char* odb_file)
{
	bgo.variables.set(odb_file);
}

void dragon::Dragon::unpack(TMidasEvent& event)
{
	gamma_modules.unpack(event);
	hi_modules.unpack(event);
	bgo.read_data(gamma_modules);
}

void dragon::Dragon::calibrate()
{
	// nothing to do so far
}
