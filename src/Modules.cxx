/// \file Modules.cxx
/// \brief Implements Modules.hxx
#include <sstream>
#include "Modules.hxx"


// ====== Class dragon::gamma::Modules ======= //

dragon::gamma::Modules::Modules():
	v792(), v1190b(), io32() { };

void dragon::gamma::Modules::unpack()
{
	vme::caen::unpack_adc(event, "VADC", v792);
//	vme::caen::unpack_v1190b(event, "VTDC", v1190b);
//	vme::unpack_io32(event, "VI032", io32);
}


// ====== Class dragon::heavy_ion::Modules ======= //

dragon::heavy_ion::Modules::Modules():
	v792(), v1190b(), io32() { };

void dragon::heavy_ion::Modules::unpack()
{
	for(int i=0; i< 2; ++i) {
		std::stringstream bank; bank << "VADC" << i;
		vme::caen::unpack_adc(event, bank.str().c_str(), v785[i]);
	}
//	vme::caen::unpack_v1190b(event, "VTDC", v1190b);
//	vme::unpack_io32(event, "VI032", io32);
}
