#include "Dragon.hxx"

// ========== Class Dragon ========== //

dragon::Dragon::Dragon(): bgo()
{
	reset();
}

void dragon::Dragon::reset()
{
	bgo.reset();
}

void dragon::Dragon::set(const char* odb_file)
{
	bgo.variables.set(odb_file);
}

void dragon::Dragon::unpack(TMidasEvent& event)
{
	bgo.unpack(event);
}
