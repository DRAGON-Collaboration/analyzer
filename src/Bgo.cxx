#include "Bgo.hxx"

// ========== Class Bgo ========== //

dragon::Bgo::Bgo(): qdc(), variables(), evt_count(0)
{
	reset();
}	

void dragon::Bgo::reset()
{
	for(int i=0; i< 30; ++i) {
		qraw[i] = vme::NONE;
		qcal[i] = vme::NONE;
		traw[i] = vme::NONE;
	}
}

void dragon::Bgo::unpack(TMidasEvent& event)
{
	evt_count++;
	vme::caen::unpack_adc(event, "VADC", qdc);
}

void dragon::Bgo::calibrate()
{
	for(int i=0; i< 30; ++i) {
// set bgo data from raw qdc values (mapping)
		qraw[i] = qdc.data[variables.qdc_ch[i]];
		traw[i] = qdc.data[variables.tdc_ch[i]];

// calibrate qdc values
		qcal[i] = qraw[i] * variables.slope[i] + variables.offset[i];
	}
}

// ========== Class Bgo::Variables ========== //

dragon::Bgo::Variables::Variables()
{
	for(int i=0; i< 30; ++i) {
		slope[i]  = 1.;
		offset[i] = 0.;
		qdc_ch[i] = i;
		tdc_ch[i] = i;
	}		 
}

void dragon::Bgo::Variables::set(const char* odb)
{
	//\ todo
}
