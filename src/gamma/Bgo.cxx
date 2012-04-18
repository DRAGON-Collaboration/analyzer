/// \file Bgo.cxx
/// \brief Implements Bgo.hxx
#include "gamma/Bgo.hxx"
#include "utils/copy_array.h"
#include "vme/vme.hxx"

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
	copy_array(from.tdc_ch, to.tdc_ch, dragon::gamma::Bgo::nch);;
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
	/// \todo Implement
}

