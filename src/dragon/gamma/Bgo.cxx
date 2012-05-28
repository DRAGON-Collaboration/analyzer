/// \file Bgo.cxx
/// \brief Implements Bgo.hxx
#include <string>
#include <iostream>
#include "dragon/gamma/Bgo.hxx"
#include "utils/copy_array.h"
#include "vme/vme.hxx"
#include "odb/MidasXML.hxx"

// ========== Class dragon::gamma::Bgo ========== //

dragon::gamma::Bgo::Bgo():
	variables()
{
	reset();
}	

namespace {
void copy_bgo(const dragon::gamma::Bgo& from, dragon::gamma::Bgo& to)
{
	to.variables = from.variables;
	copy_array(from.q, to.q, dragon::gamma::Bgo::nch);
	copy_array(from.t, to.t, dragon::gamma::Bgo::nch);
} }

dragon::gamma::Bgo::Bgo(const Bgo& other)
{
	copy_bgo(other, *this);
}

dragon::gamma::Bgo& dragon::gamma::Bgo::operator= (const Bgo& other)
{
	copy_bgo(other, *this);
	return *this;
}

void dragon::gamma::Bgo::reset()
{
	for(int i=0; i< Bgo::nch; ++i) {
		q[i] = vme::NONE;
		t[i] = vme::NONE;
	}
}

void dragon::gamma::Bgo::read_data(const dragon::gamma::Modules& modules)
{
	for(int i=0; i< Bgo::nch; ++i) {
		q[i] = modules.v792_data(variables.qdc_ch[i]);
		t[i] = modules.v1190b_data(variables.tdc_ch[i]);
	}
}


// ========== Class dragon::gamma::Bgo::Variables ========== //

dragon::gamma::Bgo::Variables::Variables()
{
	for(int i=0; i< Bgo::nch; ++i) {
		qdc_ch[i] = i;
		tdc_ch[i] = i;
	}		 
}

namespace {
void copy_bgo_variables(const dragon::gamma::Bgo::Variables& from, dragon::gamma::Bgo::Variables& to)
{
	copy_array(from.qdc_ch, to.qdc_ch, dragon::gamma::Bgo::nch);
	copy_array(from.tdc_ch, to.tdc_ch, dragon::gamma::Bgo::nch);
} }

dragon::gamma::Bgo::Variables::Variables(const Variables& other)
{
	copy_bgo_variables(other, *this);
}

dragon::gamma::Bgo::Variables& dragon::gamma::Bgo::Variables::operator= (const Variables& other)
{
	copy_bgo_variables(other, *this);
	return *this;
}

void dragon::gamma::Bgo::Variables::set(const char* odb)
{
/// \todo Set actual ODB paths, TEST!!
	const std::string pathADC = "Equipment/Bgo/Variables/ADCChannel";
	const std::string pathTDC = "Equipment/Bgo/Variables/TDCChannel";
	MidasXML mxml (odb);
	bool success = false;
	std::vector<int> ch_adc, ch_tdc;
	mxml.GetArray(pathADC.c_str(), ch_adc, &success);
	mxml.GetArray(pathTDC.c_str(), ch_tdc, &success);
	if(!success) {
		std::cerr << "Failure reading variable values from the odb file, no changes made.\n";
		return;
	}
	copy_array(&ch_adc[0], qdc_ch, dragon::gamma::Bgo::nch);
	copy_array(&ch_tdc[0], tdc_ch, dragon::gamma::Bgo::nch);
}
