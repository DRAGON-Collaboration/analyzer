#include "Bgo.hxx"

// ========== Class Bgo ========== //

dragon::Bgo::Bgo():
	nch(30), variables(nch), evt_count(0), q(nch), t(nch)
{
	reset();
}	

void dragon::Bgo::reset()
{
	for(int i=0; i< nch; ++i) {
		q[i] = vme::NONE;
		t[i] = vme::NONE;
	}
}

void dragon::Bgo::read_data(const dragon::gamma::Modules& modules)
{
	evt_count++;
	for(int i=0; i< nch; ++i) {
		q[i] = modules.v792.data[ variables.qdc_ch[i] ];
		t[i] = modules.v792.data[ variables.tdc_ch[i] ];
	}
}


// ========== Class Bgo::Variables ========== //

dragon::Bgo::Variables::Variables(int nch_): nch(nch_)
{
	for(int i=0; i< nch; ++i) {
		qdc_ch[i] = i;
		tdc_ch[i] = i;
	}		 
}

void dragon::Bgo::Variables::set(const char* odb)
{
	//\ todo
}
