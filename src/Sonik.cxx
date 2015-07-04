///
/// \file Sonik.cxx
/// \author G. Christian
/// \brief Implements Sonik.hxx
///
#include <string>
#include <sstream>
#include <iostream>
#include "midas/Database.hxx"
#include "utils/Functions.hxx"
#include "utils/Valid.hxx"
#include "utils/ErrorDragon.hxx"
#include "utils/definitions.h"
#include "Defaults.hxx"
#include "Dragon.hxx"
#include "Sonik.hxx"



namespace dutils = dragon::utils;

//
// Helper function for ::variables::set(const char*) [copied from Dsssd.cxx]
namespace {

//
// Perform the action of creating a database from it's string name,
// checking if it's valid, giving error if not, or forwarding on to
// the overloaded method taking a db pointer if it's OK.
template <class T>
bool do_setv(T* t, const char* dbfile)
{
	midas::Database db(dbfile);
	if(db.IsZombie()) {
		dutils::Error("", __FILE__, __LINE__)
			<< "Zombie database: " << dbfile;
		return false;
	}
	return t->set(&db);
}

//
// Make sure a database is valid
bool check_db(const midas::Database* db, const char* cl)
{
	bool success = db && !db->IsZombie();
	if(!success) {
		std::stringstream zmsg, where;
		where << cl << "::Variables::set";
		if(db) zmsg << ", IsZombie() " << db->IsZombie();
		dutils::Error(where.str().c_str(), __FILE__, __LINE__)
			<< "Invalid database: 0x" << db << zmsg.str();
	}
	return success;
}

} // namespace





// ================ class Sonik ========================= //

Sonik::Sonik():
	variables()
{
  /// ::
	reset();
}

Sonik::~Sonik()
{
	/// ::
	;
}

void Sonik::reset()
{
	/// ::
	trf.reset();
	dutils::reset_data(ehit, hit, thit, rf_tof, tcal0);
	dutils::reset_array(MAX_CHANNELS, ecal);
}


void Sonik::read_data(const vme::V785 adcs[], const vme::V1190& tdc)
{
	/*!
	 * Copies adc data into \c this->ecal[] with channel and module mapping taken
	 * from variables.adc.channel and variables.adc.modules
	 *
	 * Delegates work to dutils::channel_map()
	 * \param [in] adcs Array of vme::V785 adc modules from which data can be taken
	 * \param [in] tdc vme::V1190 tdc module from which data can be read
	 */
	dutils::channel_map(ecal, MAX_CHANNELS, variables.adc.channel, variables.adc.module, adcs);
	dutils::channel_map(thit, variables.tdc.channel, tdc);
	dutils::channel_map(tcal0, variables.tdc0.channel, tdc);
	trf.read_data(tdc);
}

void Sonik::calculate()
{
	/*!
	 * Does a linear transformation on each element in \c this->ecal[] using the slopes and offsets
	 * from variables.adc_slope and variables.adc_offset, respectively. Also calibrates the TDC
	 * signal; calculates ehit, hit.
 	 */
	dutils::linear_calibrate(ecal, MAX_CHANNELS, variables.adc);

	dutils::linear_calibrate(thit, variables.tdc);
	dutils::linear_calibrate(tcal0, variables.tdc0);
	trf.calculate();
	rf_tof = trf.get_tof(tcal0, 1, vme::V1190::TRAILING);

	if(dutils::is_valid_any(ecal, MAX_CHANNELS)) {
		const double* const pmax_front = std::max_element(ecal, ecal+MAX_CHANNELS);
		ehit = *pmax_front;
		hit  = pmax_front - ecal;
	}
}

bool Sonik::set_variables(midas::Database* db)
{
	bool success = variables.set(db);
	if(success) success = trf.variables.set(db, "/sonik/variables/rf_tdc");
	return success;
}


// ================ class Sonik::Variables ========================= //


Sonik::Variables::Variables()
{
	/*! Calls reset() */
	reset();
}

void Sonik::Variables::reset()
{
	/// ::
	std::fill(adc.module, adc.module + MAX_CHANNELS, DSSSD_MODULE);
	dutils::index_fill(adc.channel, adc.channel + MAX_CHANNELS, DSSSD_ADC0);

	std::fill(adc.pedestal, adc.pedestal + MAX_CHANNELS, 0);
	std::fill(adc.slope,  adc.slope + MAX_CHANNELS, 1.);
	std::fill(adc.offset, adc.offset + MAX_CHANNELS, 0.);

	tdc.module  = 0; // unused
	tdc.channel = DSSSD_TDC0;
	tdc.slope   = 1.;
	tdc.offset  = 0.;

	tdc0.module  = 0; // unused
	tdc0.channel = 9;
	tdc0.slope   = 1.;
	tdc0.offset  = 0.;

	rf_tdc.module  = 0; // unused
	rf_tdc.channel = 8;
	rf_tdc.slope   = 1.;
	rf_tdc.offset  = 0.;
}


bool Sonik::Variables::set(const char* dbfile)
{
	/*!
	 * \param [in] dbfile Path of the odb file from which you are extracting variable values
	 */
	return do_setv(this, dbfile);
}

bool Sonik::Variables::set(const midas::Database* db)
{
	/*!
	 * \param [in] Pointer to a constructed database.
	 */
	bool success = check_db(db, "Sonik");

	if(success) success = db->ReadArray("/sonik/variables/adc/module",   adc.module,   MAX_CHANNELS);
	if(success) success = db->ReadArray("/sonik/variables/adc/channel",  adc.channel,  MAX_CHANNELS);
	if(success) success = db->ReadArray("/sonik/variables/adc/slope",    adc.slope,    MAX_CHANNELS);
	if(success) success = db->ReadArray("/sonik/variables/adc/offset",   adc.offset,   MAX_CHANNELS);

	if(success) success = db->ReadValue("/sonik/variables/tdc/channel", tdc.channel);
	if(success) success = db->ReadValue("/sonik/variables/tdc/slope",   tdc.slope);
	if(success) success = db->ReadValue("/sonik/variables/tdc/offset",  tdc.offset);

	if(success) success = db->ReadValue("/sonik/variables/tdc0/channel", tdc0.channel);
	if(success) success = db->ReadValue("/sonik/variables/tdc0/slope",   tdc0.slope);
	if(success) success = db->ReadValue("/sonik/variables/tdc0/offset",  tdc0.offset);

	return success;
}





