#include "Bgo.hxx"
#include "vme/vme.hxx"

// ========== Class dragon::gamma::Bgo ========== //

dragon::gamma::Bgo::Bgo():
	variables()
{
	reset();
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

void dragon::gamma::Bgo::Variables::set(const char* odb)
{
	//\ todo
}



// ========== Class dragon::gamma::Gamma ========== //
dragon::gamma::Gamma::Gamma() :
	evt_count(0), bgo()
{
	reset();
}

void dragon::gamma::Gamma::reset()
{
	bgo.reset();
}

void dragon::gamma::Gamma::unpack(TMidasEvent& event)
{
	reset();
	modules.unpack(event);
}

void dragon::gamma::Gamma::read_data()
{
	evt_count++;
	bgo.read_data(modules);
}
