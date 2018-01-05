///
/// \file Dragon.cxx
/// \author G. Christian
/// \brief Implements Dragon.hxx
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


namespace dutils = dragon::utils;

//
// Helper function for ::variables::set(const char*)
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
// Special function implementations for classes whose method is "set_variables"
template <>
bool do_setv(dragon::Head* t, const char* dbfile)
{
	midas::Database db(dbfile);
	if(db.IsZombie())	{
		dutils::Error("", __FILE__, __LINE__)
			<< "Zombie database: " << dbfile;
		return false;
	}
	return t->set_variables(&db);
}
template <>
bool do_setv(dragon::Tail* t, const char* dbfile)
{
	midas::Database db(dbfile);
	if(db.IsZombie()) {
		dutils::Error("", __FILE__, __LINE__)
			<< "Zombie database: " << dbfile;
		return false;
	}
	return t->set_variables(&db);
}
template <>
bool do_setv(dragon::Coinc* t, const char* dbfile)
{
	midas::Database db(dbfile);
	if(db.IsZombie())	{
		dutils::Error("", __FILE__, __LINE__)
			<< "Zombie database: " << dbfile;
		return false;
	}
	return t->set_variables(&db);
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

//
// Set a bank name from the ODB
bool odb_set_bank(midas::Bank_t* bank, const midas::Database* db, const char* path, int arrayLen = 0)
{
	bool success;
	if(!arrayLen) {
		std::string bkName;
		success = db->ReadValue(path, bkName);
		if(success) dutils::set_bank_name(bkName.data(), *bank);
	} else {
		std::vector<std::string> bkNames(arrayLen);
		success = db->ReadArray(path, &bkNames[0], arrayLen);
		if(success) {
			for(int i=0; i< arrayLen; ++i) {
				dutils::set_bank_name(bkNames[i].data(), *(bank+i));
			}
		}
	}
	return success;
}

} // namespace


// ==================== Class dragon::RunParameters ==================== //

dragon::RunParameters::RunParameters()
{
	/// ::
	reset();
}

void dragon::RunParameters::reset()
{
	/// ::
	double* parrays[4] = { run_start, run_stop, trigger_start, trigger_stop };
	for(int i=0; i< 4; ++i) {
		std::fill(parrays[i], parrays[i] + MAX_FRONTENDS, 0.);
	}
}

bool dragon::RunParameters::read_data(const midas::Database* db)
{
	/// ::
	if(db == 0 || db->IsZombie()) {
		dutils::Error("dragon::RunParameters::read_data", __FILE__, __LINE__)
			<< "Zombie database";
		return false;
	}

	bool success = true;
	if(success) success = db->ReadArray("/Experiment/Run Parameters/TSC_RunStart", run_start, MAX_FRONTENDS);
	if(success) success = db->ReadArray("/Experiment/Run Parameters/TSC_RunStop", run_stop, MAX_FRONTENDS);
	if(success) success = db->ReadArray("/Experiment/Run Parameters/TSC_TriggerStart", trigger_start, MAX_FRONTENDS);
	if(success) success = db->ReadArray("/Experiment/Run Parameters/TSC_TriggerStop", trigger_stop, MAX_FRONTENDS);

	if(!success) {
		dutils::Error("dragon::RunParameters::read_data", __FILE__, __LINE__)
			<< "Failed reading one of the ODB parameters.";
	}

	return success;
}


// ==================== Class dragon::Bgo ==================== //

dragon::Bgo::Bgo():
	variables()
{
	/// ::
	reset();
}

void dragon::Bgo::reset()
{
	/// ::
	dutils::reset_array(MAX_CHANNELS, ecal);
	dutils::reset_array(MAX_CHANNELS, tcal);
	dutils::reset_array(MAX_CHANNELS, esort);
	dutils::reset_data(sum, x0, y0, z0, t0, hit0);
}

void dragon::Bgo::read_data(const vme::V792& adc, const vme::V1190& tdc)
{
	/*!
	 * Does channel mapping from ADC and TDC modules
	 * \param [in] adc Adc module
	 * \param [in] tdc Tdc module
	 */
	dutils::channel_map(ecal, MAX_CHANNELS, variables.adc.channel, adc);
	dutils::channel_map(tcal, MAX_CHANNELS, variables.tdc.channel, tdc);
}

void dragon::Bgo::calculate()
{
	/*!
	 * Does the following:
	 */
	/// - Pedestal subtract, zero suppress and calibrate energy values
	dutils::pedestal_subtract(ecal, MAX_CHANNELS, variables.adc);
	dutils::zero_suppress1(ecal, MAX_CHANNELS, 10.);
	dutils::linear_calibrate(ecal, MAX_CHANNELS, variables.adc);

	/// - Calibrate time values
	dutils::linear_calibrate(tcal, MAX_CHANNELS, variables.tdc);

	/// - Calculate descending-order energy indices and map into \c esort[]
	int isort[MAX_CHANNELS];
	dutils::index_sort(ecal, ecal+MAX_CHANNELS, isort, dutils::greater_and_valid<double>());
	dutils::channel_map_from_array(esort, MAX_CHANNELS, isort, ecal);

	/// - If we have at least one good hit, calculate sum, x0, y0, z0, and t0
	if(dutils::is_valid(esort[0])) {
		hit0 = isort[0];
		sum = dutils::calculate_sum(ecal, ecal + MAX_CHANNELS);
		x0 = variables.pos.x[ isort[0] ];
		y0 = variables.pos.y[ isort[0] ];
		z0 = variables.pos.z[ isort[0] ];
		t0 = tcal[ isort[0] ];
	}
}


// ==================== Class dragon::Bgo::Variables ==================== //

dragon::Bgo::Variables::Variables()
{
	/*! Calls reset() */
	reset();
}

void dragon::Bgo::Variables::reset()
{
	/// ::
	dutils::index_fill(adc.channel, adc.channel + MAX_CHANNELS, BGO_ADC0);
	std::fill(adc.pedestal, adc.pedestal + MAX_CHANNELS, 0);
	std::fill(adc.offset, adc.offset + MAX_CHANNELS, 0.);
	std::fill(adc.slope, adc.slope + MAX_CHANNELS, 1.);

	dutils::index_fill(tdc.channel, tdc.channel + MAX_CHANNELS, BGO_TDC0);
	std::fill(tdc.offset, tdc.offset + MAX_CHANNELS, 0.);
	std::fill(tdc.slope, tdc.slope + MAX_CHANNELS, 1.);


	const double bgoCoords[MAX_CHANNELS][3] = BGO_COORDS;
	for(int i=0; i< MAX_CHANNELS; ++i) {
		pos.x[i] = bgoCoords[i][0];
		pos.y[i] = bgoCoords[i][1];
		pos.z[i] = bgoCoords[i][2];
	}
}

bool dragon::Bgo::Variables::set(const char* dbfile)
{
	/*!
	 * \param [in] dbfile Path of the XML file from which you are extracting variable values
	 */
	return do_setv(this, dbfile);
}

bool dragon::Bgo::Variables::set(const midas::Database* db)
{
	/*!
	 * \param [in] Pointer to a constructed database.
	 */
	bool success = check_db(db, "dragon::Bgo");

	if(success) success = db->ReadArray("/dragon/bgo/variables/adc/channel",  adc.channel,  MAX_CHANNELS);
	if(success) success = db->ReadArray("/dragon/bgo/variables/adc/pedestal", adc.pedestal, MAX_CHANNELS);
	if(success) success = db->ReadArray("/dragon/bgo/variables/adc/slope",    adc.slope,    MAX_CHANNELS);
	if(success) success = db->ReadArray("/dragon/bgo/variables/adc/offset",   adc.offset,   MAX_CHANNELS);

	if(success) success = db->ReadArray("/dragon/bgo/variables/tdc/channel", tdc.channel, MAX_CHANNELS);
	if(success) success = db->ReadArray("/dragon/bgo/variables/tdc/slope",   tdc.slope,   MAX_CHANNELS);
	if(success) success = db->ReadArray("/dragon/bgo/variables/tdc/offset",  tdc.offset,  MAX_CHANNELS);

	if(success) success = db->ReadArray("/dragon/bgo/variables/position/x",  pos.x, MAX_CHANNELS);
	if(success) success = db->ReadArray("/dragon/bgo/variables/position/y",  pos.y, MAX_CHANNELS);
	if(success) success = db->ReadArray("/dragon/bgo/variables/position/z",  pos.z, MAX_CHANNELS);

	return success;
}


// ================ class dragon::Dsssd ========================= //

dragon::Dsssd::Dsssd():
	variables()
{
	/// ::
	reset();
}

void dragon::Dsssd::reset()
{
	/// ::
	dutils::reset_data(efront, eback, hit_front, hit_back, tfront, tback);
	dutils::reset_array(MAX_CHANNELS, ecal);
}

void dragon::Dsssd::read_data(const vme::V785 adcs[], const vme::V1190& tdc)
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
	dutils::channel_map(tfront, variables.tdc_front.channel, tdc);
	dutils::channel_map(tback,  variables.tdc_back.channel,  tdc);
}

void dragon::Dsssd::calculate()
{
	/*!
	 * Does a linear transformation on each element in \c this->ecal[] using the slopes and offsets
	 * from variables.adc_slope and variables.adc_offset, respectively. Also calibrates the TDC
	 * signal; calculates efront, hit_front, eback, and hit_back.
	 *
	 * Delegates the work to dutils::linear_calibrate
	 * \note Do we want to add a zero suppression threshold here?
	 */
	dutils::linear_calibrate(ecal, MAX_CHANNELS, variables.adc);
	dutils::linear_calibrate(tfront, variables.tdc_front);
	dutils::linear_calibrate(tback,  variables.tdc_back);

	if(dutils::is_valid_any(ecal, 16)) {
		const double* const pmax_front = std::max_element(ecal, ecal+16);
		efront = *pmax_front;
		hit_front = pmax_front - ecal;
	}

	if(dutils::is_valid_any(ecal+16, 16)) {
		const double* const pmax_back  = std::max_element(ecal+16, ecal+32);
		eback  = *pmax_back;
		hit_back = pmax_back - ecal;
	}
}


// ================ class dragon::Dsssd::Variables ================ //

dragon::Dsssd::Variables::Variables()
{
	/*! Calls reset() */
	reset();
}

void dragon::Dsssd::Variables::reset()
{
	/// ::
	std::fill(adc.module, adc.module + MAX_CHANNELS, DSSSD_MODULE);
	dutils::index_fill(adc.channel, adc.channel + MAX_CHANNELS, DSSSD_ADC0);

	std::fill(adc.pedestal, adc.pedestal + MAX_CHANNELS, 0);
	std::fill(adc.slope,  adc.slope + MAX_CHANNELS, 1.);
	std::fill(adc.offset, adc.offset + MAX_CHANNELS, 0.);

	tdc_front.module  = 0; // unused
	tdc_front.channel = DSSSD_TDC0;
	tdc_front.slope   = 1.;
	tdc_front.offset  = 0.;

	tdc_back.module  = 0; // unused
	tdc_back.channel = DSSSD_TDC0 + 1;
	tdc_back.slope   = 1.;
	tdc_back.offset  = 0.;
}

bool dragon::Dsssd::Variables::set(const char* dbfile)
{
	/*!
	 * \param [in] dbfile Path of the odb file from which you are extracting variable values
	 */
	return do_setv(this, dbfile);
}

bool dragon::Dsssd::Variables::set(const midas::Database* db)
{
	/*!
	 * \param [in] Pointer to a constructed database.
	 */
	bool success = check_db(db, "dragon::Dsssd");

	if(success) success = db->ReadArray("/dragon/dsssd/variables/adc/module",   adc.module,   MAX_CHANNELS);
	if(success) success = db->ReadArray("/dragon/dsssd/variables/adc/channel",  adc.channel,  MAX_CHANNELS);
	if(success) success = db->ReadArray("/dragon/dsssd/variables/adc/slope",    adc.slope,    MAX_CHANNELS);
	if(success) success = db->ReadArray("/dragon/dsssd/variables/adc/offset",   adc.offset,   MAX_CHANNELS);

	if(success) success = db->ReadValue("/dragon/dsssd/variables/tdc_front/channel", tdc_front.channel);
	if(success) success = db->ReadValue("/dragon/dsssd/variables/tdc_front/slope",   tdc_front.slope);
	if(success) success = db->ReadValue("/dragon/dsssd/variables/tdc_front/offset",  tdc_front.offset);

	if(success) success = db->ReadValue("/dragon/dsssd/variables/tdc_back/channel", tdc_back.channel);
	if(success) success = db->ReadValue("/dragon/dsssd/variables/tdc_back/slope",   tdc_back.slope);
	if(success) success = db->ReadValue("/dragon/dsssd/variables/tdc_back/offset",  tdc_back.offset);

	return success;
}


// ================ Class dragon::IonChamber ================ //

dragon::IonChamber::IonChamber()
{
	/// ::
	reset();
}

void dragon::IonChamber::reset()
{
	/// ::
	dutils::reset_array(MAX_CHANNELS, anode);
	dutils::reset_array(MAX_TDC, tcal);
	dutils::reset_data(sum);
}

void dragon::IonChamber::read_data(const vme::V785 adcs[], const vme::V1190& tdc)
{
	/*!
	 * \param modules Heavy-ion module structure
	 * \param [in] v1190_trigger_ch Channel number of the v1190b trigger
	 */
	dutils::channel_map(anode, MAX_CHANNELS, variables.adc.channel, variables.adc.module, adcs);
	dutils::channel_map(tcal, MAX_TDC, variables.tdc.channel, tdc);
}

void dragon::IonChamber::calculate()
{
	/*!
	 * Calibrates anode and time signals, calculates anode sum
	 */
	dutils::linear_calibrate(anode, MAX_CHANNELS, variables.adc);
	dutils::linear_calibrate(tcal, MAX_TDC, variables.tdc);

	if(dutils::is_valid_any(anode, MAX_CHANNELS)) {
		sum = dutils::calculate_sum(anode, anode + MAX_CHANNELS);
	}
}


// ================ struct dragon::IonChamber::Variables ================ //

dragon::IonChamber::Variables::Variables()
{
	/*! Calls reset() */
	reset();
}

void dragon::IonChamber::Variables::reset()
{
	/// ::
	std::fill(adc.module, adc.module + MAX_CHANNELS, DEFAULT_HI_MODULE);
	dutils::index_fill(adc.channel, adc.channel + MAX_CHANNELS, IC_ADC0);

	std::fill(adc.pedestal, adc.pedestal + MAX_CHANNELS, 0);
	std::fill(adc.slope, adc.slope + MAX_CHANNELS, 1.);
	std::fill(adc.offset, adc.offset + MAX_CHANNELS, 0.);


	std::fill(tdc.module, tdc.module + MAX_TDC, 0); // unused
	dutils::index_fill(tdc.channel, tdc.channel+MAX_TDC, IC_TDC0);
	std::fill(tdc.offset, tdc.offset + MAX_TDC, 0.);
	std::fill(tdc.slope, tdc.slope + MAX_TDC, 1.);
}

bool dragon::IonChamber::Variables::set(const char* dbfile)
{
	/*!
	 * \param [in] dbfile Path of the odb file from which you are extracting variable values
	 */
	return do_setv(this, dbfile);
}

bool dragon::IonChamber::Variables::set(const midas::Database* db)
{
	/*!
	 * \param [in] db Pointer to a constructed database.
	 */
	bool success = check_db(db, "dragon::IonChamber");

	if(success) success = db->ReadArray("/dragon/ic/variables/adc/module",   adc.module,   MAX_CHANNELS);
	if(success) success = db->ReadArray("/dragon/ic/variables/adc/channel",  adc.channel,  MAX_CHANNELS);
	if(success) success = db->ReadArray("/dragon/ic/variables/adc/slope",    adc.slope,    MAX_CHANNELS);
	if(success) success = db->ReadArray("/dragon/ic/variables/adc/offset",   adc.offset,   MAX_CHANNELS);

	if(success) success = db->ReadArray("/dragon/ic/variables/tdc/channel",  tdc.channel, MAX_TDC);
	if(success) success = db->ReadArray("/dragon/ic/variables/tdc/slope",    tdc.slope,   MAX_TDC);
	if(success) success = db->ReadArray("/dragon/ic/variables/tdc/offset",   tdc.offset,  MAX_TDC);

	return success;
}


// ================ class dragon::Mcp ================ //

dragon::Mcp::Mcp()
{
	/// ::
	reset();
}

void dragon::Mcp::reset()
{
	/// ::
	dutils::reset_data(esum, tac, x, y);
	dutils::reset_array(MAX_CHANNELS, anode);
	dutils::reset_array(NUM_DETECTORS, tcal);
}

void dragon::Mcp::read_data(const vme::V785 adcs[], const vme::V1190& tdc)
{
	/*!
	 * Copies ADC and TDC data into anode, tcal, and tac parameters.
	 *
	 * Delegates work to dutils::channel_map()
	 * \param [in] adcs Array of vme::V785 adc modules from which data can be taken
	 * \param [in] tdc vme::V1190 tdc module from which data can be read
	 */
	dutils::channel_map(anode, MAX_CHANNELS, variables.adc.channel, variables.adc.module, adcs);
	dutils::channel_map(tcal, NUM_DETECTORS, variables.tdc.channel, tdc);
	dutils::channel_map(tac, variables.tac_adc.channel, variables.tac_adc.module, adcs);
}

void dragon::Mcp::calculate()
{
	/*!
	 * Calibrates anode, tcal, and tac values; calculates x- and
	 * y-positions.
	 *
	 * \note Position calculation algorithm taken from the MSc thesis of
	 * Michael Lamey, Simon Fraser University, 2001, p. 25; available online at
	 * <a href="http://dragon.triumf.ca/docs/Lamey_thesis.pdf">
	 * dragon.triumf.ca/docs/Lamey_thesis.pdf</a>
	 */
	dutils::linear_calibrate(anode, MAX_CHANNELS, variables.adc);
	dutils::linear_calibrate(tcal, NUM_DETECTORS, variables.tdc);
	dutils::linear_calibrate(tac, variables.tac_adc);

	dutils::calculate_sum(anode, anode + MAX_CHANNELS);

	// Position calculation if we have all valid anode signals
	if(dutils::is_valid_all(anode, MAX_CHANNELS)) {
		const double Lhalf = 25.;  // half the length of a single side of the MCP (50/2 [mm])
		double sum = 0;
		for(int i=0; i< MAX_CHANNELS; ++i) sum += anode[i];
		if(sum != 0) {
			x = Lhalf * ( (anode[1] + anode[2]) - (anode[0] + anode[3]) ) / sum;
			y = Lhalf * ( (anode[0] + anode[1]) - (anode[2] + anode[3]) ) / sum;
		}
	}
}


// ================ class dragon::Mcp::Variables ================ //

dragon::Mcp::Variables::Variables()
{
	/*! Calls reset() */
	reset();
}

void dragon::Mcp::Variables::reset()
{
	/// ::
	std::fill(adc.module, adc.module + MAX_CHANNELS, DEFAULT_HI_MODULE);
	dutils::index_fill(adc.channel, adc.channel + MAX_CHANNELS, MCP_ADC0);

	std::fill(adc.pedestal, adc.pedestal + MAX_CHANNELS, 0);
	std::fill(adc.offset, adc.offset + MAX_CHANNELS, 0.);
	std::fill(adc.slope, adc.slope + MAX_CHANNELS, 1.);

	tac_adc.module   = DEFAULT_HI_MODULE;
	tac_adc.channel  = MCP_TAC_ADC0;

	tac_adc.pedestal = 0;
	tac_adc.offset   = 0.;
	tac_adc.slope    = 1.;

	std::fill(tdc.module, tdc.module + NUM_DETECTORS, 0); // unused
	dutils::index_fill(tdc.channel, tdc.channel + NUM_DETECTORS, MCP_TDC0);

	std::fill(tdc.offset, tdc.offset + NUM_DETECTORS, 0.);
	std::fill(tdc.slope, tdc.slope + NUM_DETECTORS, 1.);
}

bool dragon::Mcp::Variables::set(const char* dbfile)
{
	/*!
	 * \param [in] dbfile Path of the odb file from which you are extracting variable values
	 */
	return do_setv(this, dbfile);
}

bool dragon::Mcp::Variables::set(const midas::Database* db)
{
	/*!
	 * \param [in] db Pointer to a constructed database.
	 */
	bool success = check_db(db, "dragon::Mcp");

	if(success) success = db->ReadArray("/dragon/mcp/variables/adc/channel",  adc.channel,  MAX_CHANNELS);
	if(success) success = db->ReadArray("/dragon/mcp/variables/adc/module",   adc.module,   MAX_CHANNELS);
	if(success) success = db->ReadArray("/dragon/mcp/variables/adc/slope",    adc.slope,    MAX_CHANNELS);
	if(success) success = db->ReadArray("/dragon/mcp/variables/adc/offset",   adc.offset,   MAX_CHANNELS);

	if(success) success = db->ReadValue("/dragon/mcp/variables/tac_adc/channel",  tac_adc.channel);
	if(success) success = db->ReadValue("/dragon/mcp/variables/tac_adc/module",   tac_adc.module);
	if(success) success = db->ReadValue("/dragon/mcp/variables/tac_adc/slope",    tac_adc.slope);
	if(success) success = db->ReadValue("/dragon/mcp/variables/tac_adc/offset",   tac_adc.offset);

	if(success) success = db->ReadArray("/dragon/mcp/variables/tdc/channel", tdc.channel, NUM_DETECTORS);
	if(success) success = db->ReadArray("/dragon/mcp/variables/tdc/slope",   tdc.slope,   NUM_DETECTORS);
	if(success) success = db->ReadArray("/dragon/mcp/variables/tdc/offset",  tdc.offset,  NUM_DETECTORS);

	return success;
}


// ==================== Class dragon::SurfaceBarrier ==================== //

dragon::SurfaceBarrier::SurfaceBarrier()
{
	/// ::
	reset();
}

void dragon::SurfaceBarrier::reset()
{
	/// ::
	dutils::reset_array(MAX_CHANNELS, ecal);
}

void dragon::SurfaceBarrier::read_data(const vme::V785 adcs[], const vme::V1190&)
{
	/*!
	 * Copies adc data into \c this->ecal[] with channel and module mapping taken
	 * from variables.adc.channel and variables.adc.modules
	 *
	 * Delegates work to dutils::channel_map()
	 * \param [in] adcs Array of vme::V785 adc modules from which data can be taken
	 */
	dutils::channel_map(ecal, MAX_CHANNELS, variables.adc.channel, variables.adc.module, adcs);
}

void dragon::SurfaceBarrier::calculate()
{
	/*!
	 * Performs calibration of energies.
	 *
	 * Delegates work to dutils::linear_calibrate()
	 */
	dutils::linear_calibrate(ecal, MAX_CHANNELS, variables.adc);
}


// ==================== class dragon::SurfaceBarrier::Variables ==================== //

dragon::SurfaceBarrier::Variables::Variables()
{
	/*! Calls reset() */
	reset();
}

void dragon::SurfaceBarrier::Variables::reset()
{
	/// ::
	std::fill(adc.module, adc.module + MAX_CHANNELS, DEFAULT_HI_MODULE);
	dutils::index_fill(adc.channel, adc.channel + MAX_CHANNELS, SB_ADC0);

	std::fill(adc.pedestal, adc.pedestal + MAX_CHANNELS, 0);
	std::fill(adc.offset, adc.offset + MAX_CHANNELS, 0.);
	std::fill(adc.slope, adc.slope + MAX_CHANNELS, 1.);
}

bool dragon::SurfaceBarrier::Variables::set(const char* dbfile)
{
	/*!
	 * \param [in] dbfile Path of the odb file from which you are extracting variable values
	 */
	return do_setv(this, dbfile);
}

bool dragon::SurfaceBarrier::Variables::set(const midas::Database* db)
{
	/*!
	 * \param [in] db Pointer to a constructed database.
	 */
	bool success = check_db(db, "dragon::SurfaceBarrier");

	if(success) success = db->ReadArray("/dragon/sb/variables/adc/module",   adc.module,   MAX_CHANNELS);
	if(success) success = db->ReadArray("/dragon/sb/variables/adc/channel",  adc.channel,  MAX_CHANNELS);
	if(success) success = db->ReadArray("/dragon/sb/variables/adc/slope",    adc.slope,    MAX_CHANNELS);
	if(success) success = db->ReadArray("/dragon/sb/variables/adc/offset",   adc.offset,   MAX_CHANNELS);

	return success;
}


// ========================== Class dragon::NaI ========================== //

dragon::NaI::NaI()
{
	/// ::
	reset();
}

void dragon::NaI::reset()
{
	/// ::
	dutils::reset_array(MAX_CHANNELS, ecal);
}

void dragon::NaI::read_data(const vme::V785 adcs[], const vme::V1190&)
{
	/*!
	 * Copies adc data into \c this->ecal[] with channel and module mapping taken
	 * from variables.adc.channel and variables.adc.modules
	 *
	 * Delegates work to dutils::channel_map()
	 * \param [in] adcs Array of vme::V785 adc modules from which data can be taken
	 */
	dutils::channel_map(ecal, MAX_CHANNELS, variables.adc.channel, variables.adc.module, adcs);
}

void dragon::NaI::calculate()
{
	/// Linear calibration of energies
	dutils::linear_calibrate(ecal, MAX_CHANNELS, variables.adc);
}


// ====== class dragon::NaI::Variables ====== //

dragon::NaI::Variables::Variables()
{
	/*! Calls reset() */
	reset();
}

void dragon::NaI::Variables::reset()
{
	/// ::
	std::fill(adc.module, adc.module + MAX_CHANNELS, DEFAULT_HI_MODULE);
	dutils::index_fill(adc.channel, adc.channel + MAX_CHANNELS, NAI_ADC0);
	std::fill(adc.pedestal, adc.pedestal + MAX_CHANNELS, 0);
	std::fill(adc.offset, adc.offset + MAX_CHANNELS, 0.);
	std::fill(adc.slope, adc.slope + MAX_CHANNELS, 1.);
}

bool dragon::NaI::Variables::set(const char* dbfile)
{
	/*!
	 * \param [in] dbfile Path of the odb file from which you are extracting variable values
	 */
	return do_setv(this, dbfile);
}

bool dragon::NaI::Variables::set(const midas::Database* db)
{
	/*!
	 * \param [in] db Pointer to a constructed database.
	 */
	bool success = check_db(db, "dragon::NaI");

	if(success) success = db->ReadArray("/dragon/nai/variables/adc/module",   adc.module,   MAX_CHANNELS);
	if(success) success = db->ReadArray("/dragon/nai/variables/adc/channel",  adc.channel,  MAX_CHANNELS);
	if(success) success = db->ReadArray("/dragon/nai/variables/adc/slope",    adc.slope,    MAX_CHANNELS);
	if(success) success = db->ReadArray("/dragon/nai/variables/adc/offset",   adc.offset,   MAX_CHANNELS);

	return success;
}


// ====== class dragon::Ge ====== //

dragon::Ge::Ge()
{
	/// ::
	reset();
}

void dragon::Ge::reset()
{
	/// ::
	dutils::reset_data(ecal);
}

void dragon::Ge::read_data(const vme::V785 adcs[], const vme::V1190&)
{
	/*!
	 * Copies adc data into \c this->ecal[] with channel and module mapping taken
	 * from variables.adc.channel and variables.adc.module
	 *
	 * Delegates work to dutils::channel_map()
	 * \param [in] adcs Array of vme::V785 adc modules from which data can be taken
	 */
	dutils::channel_map(ecal, variables.adc.channel, variables.adc.module, adcs);
}

void dragon::Ge::calculate()
{
	/// Calibration of energies
	dutils::linear_calibrate(ecal, variables.adc);
}


// ==================== Class dragon::Ge::Variables ==================== //

dragon::Ge::Variables::Variables()
{
	/*! Calls reset() */
	reset();
}

void dragon::Ge::Variables::reset()
{
	/// ::
	adc.module = DEFAULT_HI_MODULE;
	adc.channel = GE_ADC0;
	adc.pedestal = 0;
	adc.offset = 0.;
	adc.slope = 1.;
}

bool dragon::Ge::Variables::set(const char* dbfile)
{
	/*!
	 * \param [in] dbfile Path of the odb file from which you are extracting variable values
	 */
	return do_setv(this, dbfile);
}

bool dragon::Ge::Variables::set(const midas::Database*db)
{
	/*!
	 * \param [in] db Pointer to a constructed database.
	 */
	bool success = check_db(db, "dragon::Ge");

	if(success) success = db->ReadValue("/dragon/ge/variables/adc/module",   adc.module);
	if(success) success = db->ReadValue("/dragon/ge/variables/adc/channel",  adc.channel);
	if(success) success = db->ReadValue("/dragon/ge/variables/adc/slope",    adc.slope);
	if(success) success = db->ReadValue("/dragon/ge/variables/adc/offset",   adc.offset);

	return success;
}


// ====== Class dragon::HiTof ====== //

dragon::HiTof::HiTof()
{
	/*! Calls reset() */
	reset();
	assert(Mcp::NUM_DETECTORS > 1);
}

void dragon::HiTof::reset()
{
	/*! Set all data to dragon::NO_DATA */
	dutils::reset_data(     mcp
#ifndef DRAGON_OMIT_DSSSD
											 , mcp_dsssd
#endif
#ifndef DRAGON_OMIT_IC
											 , mcp_ic
#endif
		);
}

void dragon::HiTof::calculate(const dragon::Tail* tail)
{
	/*!
	 * Calculate TOF from detector parameters
	 * \param tail Pointer to tail instance containing tdc data
	 */
	mcp = dutils::calculate_tof(tail->mcp.tcal[1], tail->mcp.tcal[0]);
#ifndef DRAGON_OMIT_DSSSD
	mcp_dsssd = dutils::calculate_tof(tail->dsssd.tfront, tail->mcp.tcal[0]);
#endif
#ifndef DRAGON_OMIT_IC
	mcp_ic    = dutils::calculate_tof(tail->ic.tcal[0], tail->mcp.tcal[0]);
#endif
}


// ==================== Class dragon::Head ==================== //

dragon::Head::Head()
{
	/*!
	 * Set bank names, resets data to default values
	 */
	reset();
}

void dragon::Head::reset()
{
	/*!
	 * Call reset() for all members that are classes, reset raw data to defaults.
	 */
	const midas::Event::Header temp = { 0, 0, 0, 0, 0 };
	header = temp;
	io32.reset();
	v792.reset();
	v1190.reset();
	bgo.reset();
	trf.reset();
	dutils::reset_data(tcalx, tcal0, tcal_rf);
}

bool dragon::Head::set_variables(const char* dbfile)
{
	/*!
	 * \param [in] dbfile Name of the XML database file
	 * \note Passing \c "online" looks at the online ODB.
	 */
	return do_setv(this, dbfile);
}

bool dragon::Head::set_variables(const midas::Database* db)
{
	/*!
	 * \param [in] odb_file Pointer to the database from which to read the variables.
	 */
	bool success = check_db(db, "dragon::Head::set_variables");

	if(success) success = bgo.variables.set(db);
	if(success) success = trf.variables.set(db, "/dragon/head/variables/rf_tdc");
	if(success) success = this->variables.set(db);

	if(success) {
		dragon::utils::ChangeErrorIgnore dummy(9001);
		int period = 0;
		if(db->ReadValue("/dragon/errormessage/v1190/period", period))
			v1190.fMessagePeriod = period;
		else
			v1190.fMessagePeriod = 0;
	}

	return success;
}

void dragon::Head::unpack(const midas::Event& event)
{
	/*!
	 * \param [in] event Reference to a Midas event structure
	 *
	 * Here is where the low-level work is done to take bitpacked data read directly
	 * from a MIDAS file and convert it into the corresponding measurement values
	 * of various ADC modules.
	 *
	 * In the specific case of unpacking the dragon::Head VME data, we delegate
	 * the work to the unpack() routines of the relevant vme::* classes.
	 *
	 * \note Recompile with <c> report = true </c> to print warning messages
	 *  for missing banks
	 */
	const bool report = true;
	io32.unpack (event, variables.bk_io32, report);
	v792.unpack (event, variables.bk_adc , report);
	v1190.unpack(event, variables.bk_tdc,  report);
	event.CopyHeader(header);
}

void dragon::Head::calculate()
{
	/*!
	 * This is where the raw data recorded in VME fields is mapped to a more
	 * abstract location in a detector class. This covers a wide range of parameter
	 * calculation, from, e.g. simply mapping ADC channels to corresponding (uncalibrated)
	 * detector signals, up to calculation of abstract physics quantities that depend on multiple
	 * calibrated detector signals.
	 *
	 * In the specific implementation, the following are done:
	 */
	/// - Read BGO data and calculate (see dragon::Head::Bgo).
	bgo.read_data(v792, v1190);
	bgo.calculate();

	trf.read_data(v1190);
	trf.calculate();

	/// - Read and calibrate "crossover" TDC channel.
	dutils::channel_map(tcalx, variables.xtdc.channel, v1190);
	dutils::linear_calibrate(tcalx, variables.xtdc);

	/// - Read and calibrate RF TDC channel.
	dutils::channel_map(tcal_rf, variables.rf_tdc.channel, v1190);
	dutils::linear_calibrate(tcal_rf, variables.rf_tdc);

	/// - Read and calibrate t0 (trigger) channel
	dutils::channel_map(tcal0, variables.tdc0.channel, v1190);
	dutils::linear_calibrate(tcal0, variables.tdc0);
}


// ================ Class dragon::Head::Variables ================ //

dragon::Head::Variables::Variables()
{
	/// ::
	reset();
}

void dragon::Head::Variables::reset()
{
	/// ::
	dutils::set_bank_name(HEAD_IO32_BANK, bk_io32);
	dutils::set_bank_name(HEAD_TSC_BANK,  bk_tsc);
	dutils::set_bank_name(HEAD_TDC_BANK,  bk_tdc);
	dutils::set_bank_name(HEAD_ADC_BANK,  bk_adc);

	xtdc.channel = HEAD_CROSS_TDC;
	xtdc.slope  = 1.;
	xtdc.offset = 0.;

	rf_tdc.channel = HEAD_CROSS_TDC - 2;
	rf_tdc.slope  = 1.;
	rf_tdc.offset = 0.;

	tdc0.channel = HEAD_CROSS_TDC - 1;
	tdc0.slope  = 1.;
	tdc0.offset = 0.;
}

bool dragon::Head::Variables::set(const char* dbfile)
{
	/*!
	 * \param [in] dbfile Name of the database file from which to read variables ("online" for the ODB).
	 */
	return do_setv(this, dbfile);
}

bool dragon::Head::Variables::set(const midas::Database* db)
{
	/*!
	 * \param [in] db Pointer to a constructed database.
	 */
	bool success = check_db(db, "dragon::Head");

	if(success) success = odb_set_bank(&bk_io32, db, "/dragon/head/variables/bank_names/io32");
	if(success) success = odb_set_bank(&bk_tsc,  db, "/dragon/head/variables/bank_names/tsc");
	if(success) success = odb_set_bank(&bk_adc,  db, "/dragon/head/variables/bank_names/adc");
	if(success) success = odb_set_bank(&bk_tdc,  db, "/dragon/head/variables/bank_names/tdc");

	if(success) success = db->ReadValue("/dragon/head/variables/xtdc/channel", xtdc.channel);
	if(success) success = db->ReadValue("/dragon/head/variables/xtdc/slope",   xtdc.slope);
	if(success) success = db->ReadValue("/dragon/head/variables/xtdc/offset",  xtdc.offset);

	if(success) success = db->ReadValue("/dragon/head/variables/rf_tdc/channel", rf_tdc.channel);
	if(success) success = db->ReadValue("/dragon/head/variables/rf_tdc/slope",   rf_tdc.slope);
	if(success) success = db->ReadValue("/dragon/head/variables/rf_tdc/offset",  rf_tdc.offset);

	if(success) success = db->ReadValue("/dragon/head/variables/tdc0/channel", tdc0.channel);
	if(success) success = db->ReadValue("/dragon/head/variables/tdc0/slope",   tdc0.slope);
	if(success) success = db->ReadValue("/dragon/head/variables/tdc0/offset",  tdc0.offset);

	return success;
}


// ================ Class dragon::Tail ================ //

dragon::Tail::Tail()
{
	/// ::
	reset();
}

void dragon::Tail::reset()
{
	/// ::
	io32.reset();
	v1190.reset();
	for (int i=0; i< NUM_ADC; ++i) {
		v785[i].reset();
	}
	trf.reset();
#ifndef DRAGON_OMIT_DSSSD
	dsssd.reset();
#endif
#ifndef DRAGON_OMIT_IC
	ic.reset();
#endif
#ifndef DRAGON_OMIT_NAI
	nai.reset();
#endif
#ifndef DRAGON_OMIT_GE
	ge.reset();
#endif
	mcp.reset();
	sb.reset();
	tof.reset();
	dutils::reset_data(tcalx, tcal0, tcal_rf);
}

void dragon::Tail::unpack(const midas::Event& event)
{
	/*!
	 * \param [in] event Reference to a Midas event structure
	 *
	 * Here is where the low-level work is done to take bitpacked data read directly
	 * from a MIDAS file and convert it into the corresponding measurement values
	 * of various ADC modules.
	 *
	 * In the specific case of unpacking the dragon::Tail VME data, we delegate
	 * the work to the unpack() routines of the relevant vme::* classes.
	 *
	 * \note Recompile with <c> report = true </c> to print warning messages
	 *  for missing banks
	 */
	const bool report = false;
	io32.unpack (event, variables.bk_io32, report);
	for (int i=0; i< NUM_ADC; ++i) {
		v785[i].unpack(event, variables.bk_adc[i], report);
	}
	v1190.unpack(event, variables.bk_tdc, report);

	event.CopyHeader(header);
}

void dragon::Tail::calculate()
{
	/// - Read data from VME modules into data structures
#ifndef DRAGON_OMIT_DSSSD
	dsssd.read_data(v785, v1190);
#endif
#ifndef DRAGON_OMIT_IC
	ic.read_data(v785, v1190);
#endif
	mcp.read_data(v785, v1190);
	sb.read_data(v785, v1190);
#ifndef DRAGON_OMIT_NAI
	nai.read_data(v785, v1190);
#endif
#ifndef DRAGON_OMIT_GE
	ge.read_data(v785, v1190);
#endif
	trf.read_data(v1190);

	/// - Perform calibrations, higher-order calculations, etc, detector-by-detector
#ifndef DRAGON_OMIT_DSSSD
	dsssd.calculate();
#endif
#ifndef DRAGON_OMIT_IC
	ic.calculate();
#endif
	mcp.calculate();
	sb.calculate();
#ifndef DRAGON_OMIT_NAI
	nai.calculate();
#endif
#ifndef DRAGON_OMIT_GE
	ge.calculate();
#endif
	trf.calculate();

	/// - Calculate TOF between HI detectors
	tof.calculate(this);

	/// - Map and calibrate "crossover" TDC
	dutils::channel_map(tcalx, variables.xtdc.channel, v1190);
	dutils::linear_calibrate(tcalx, variables.xtdc);

	/// - Map and calibrate RF TDC
	dutils::channel_map(tcal_rf, variables.rf_tdc.channel, v1190);
	dutils::linear_calibrate(tcal_rf, variables.rf_tdc);

	/// - Map and calibrate "trigger" TDC
	dutils::channel_map(tcal0, variables.tdc0.channel, v1190);
	dutils::linear_calibrate(tcal0, variables.tdc0);
}

bool dragon::Tail::set_variables(const char* dbfile)
{
	/*!
	 * \param [in] dbfile Name of the XML database file
	 * \note Passing \c "online" looks at the online ODB.
	 */
	return do_setv(this, dbfile);
}

bool dragon::Tail::set_variables(const midas::Database* db)
{
	/*!
	 * \param [in] Pointer to a constructed database.
	 */
	bool success = check_db(db, "dragon::Tail::set_variables");

#ifndef DRAGON_OMIT_DSSSD
	if(success) success = dsssd.variables.set(db);
#endif

#ifndef DRAGON_OMIT_IC
	if(success) success = ic.variables.set(db);
#endif

	if(success) success = mcp.variables.set(db);
	if(success) success = sb.variables.set(db);

#ifndef DRAGON_OMIT_NAI
	if(success) success = nai.variables.set(db);
#endif

#ifndef DRAGON_OMIT_GE
	if(success) success = ge.variables.set(db);
#endif

	if(success) success = trf.variables.set(db, "/dragon/tail/variables/rf_tdc");
	if(success) success = this->variables.set(db);

	if(success) {
		dragon::utils::ChangeErrorIgnore dummy(9001);
		int period = 0;
		if(db->ReadValue("/dragon/errormessage/v1190/period", period))
			v1190.fMessagePeriod = period;
		else
			v1190.fMessagePeriod = 0;
	}

	return success;
}

// ================ Class dragon::Tail::Variables ================ //

dragon::Tail::Variables::Variables()
{
	/// ::
	reset();
}

void dragon::Tail::Variables::reset()
{
	/// ::
	dutils::set_bank_name(TAIL_IO32_BANK,   bk_io32);
	dutils::set_bank_name(TAIL_TSC_BANK,    bk_tsc);
	dutils::set_bank_name(TAIL_TDC_BANK,    bk_tdc);
	dutils::set_bank_name(TAIL_ADC_BANK_0,  bk_adc[0]);
	dutils::set_bank_name(TAIL_ADC_BANK_1,  bk_adc[1]);

	xtdc.channel = TAIL_CROSS_TDC;
	xtdc.slope  = 1.;
	xtdc.offset = 0.;

	rf_tdc.channel = TAIL_RF_TDC;
	rf_tdc.slope  = 1.;
	rf_tdc.offset = 0.;

	tdc0.channel = TAIL_RF_TDC + 1;
	tdc0.slope  = 1.;
	tdc0.offset = 0.;
}

bool dragon::Tail::Variables::set(const char* dbfile)
{
	/*!
	 * \param [in] dbfile Name of the database file from which to read variables ("online" for the ODB).
	 */
	return do_setv(this, dbfile);
}

bool dragon::Tail::Variables::set(const midas::Database* db)
{
	/*!
	 * \param [in] db Pointer to a constructed database.
	 */
	bool success = check_db(db, "dragon::Tail");

	if(success) success = odb_set_bank(&bk_io32, db, "/dragon/tail/variables/bank_names/io32");
	if(success) success = odb_set_bank(&bk_tsc,  db, "/dragon/tail/variables/bank_names/tsc");
	if(success) success = odb_set_bank(&bk_tdc,  db, "/dragon/tail/variables/bank_names/tdc");
	if(success) success = odb_set_bank(bk_adc,   db, "/dragon/tail/variables/bank_names/adc", NUM_ADC);

	if(success) success = db->ReadValue("/dragon/tail/variables/xtdc/channel", xtdc.channel);
	if(success) success = db->ReadValue("/dragon/tail/variables/xtdc/slope",   xtdc.slope);
	if(success) success = db->ReadValue("/dragon/tail/variables/xtdc/offset",  xtdc.offset);

	if(success) success = db->ReadValue("/dragon/tail/variables/rf_tdc/channel", rf_tdc.channel);
	if(success) success = db->ReadValue("/dragon/tail/variables/rf_tdc/slope",   rf_tdc.slope);
	if(success) success = db->ReadValue("/dragon/tail/variables/rf_tdc/offset",  rf_tdc.offset);

	if(success) success = db->ReadValue("/dragon/tail/variables/tdc0/channel", tdc0.channel);
	if(success) success = db->ReadValue("/dragon/tail/variables/tdc0/slope",   tdc0.slope);
	if(success) success = db->ReadValue("/dragon/tail/variables/tdc0/offset",  tdc0.offset);

	return success;
}


// ==================== Class dragon::Scaler ==================== //

dragon::Scaler::Scaler():
	variables()
{
	/*! Calls reset() */
	reset();
}

void dragon::Scaler::reset()
{
	/*! Sets all count, rate, and sum to zero */
	std::fill(count, count + MAX_CHANNELS, 0);
	std::fill(sum, sum + MAX_CHANNELS, 0);
	std::fill(rate, rate + MAX_CHANNELS, 0.);
}

bool dragon::Scaler::set_variables(const char* dbfile, const char* dir)
{
	/*!
	 * Delegates to dragon::Scaler::Variables::Set()
	 */
	return variables.set(dbfile, dir);
}

bool dragon::Scaler::set_variables(const midas::Database* db, const char* dir)
{
	/*!
	 * Delegates to dragon::Scaler::Variables::Set()
	 */
	return variables.set(db, dir);
}

namespace {
inline bool check_bank_len(int expected, int gotten, const char* bkname)
{
	bool retval = true;
	if (expected != gotten) {
		dutils::Error("dragon::Scaler::unpack", __FILE__, __LINE__)
			<< "Unexpected length of bank \"" << bkname << "\": expected " << expected
			<< ", got " << gotten << DRAGON_ERR_FILE_LINE;
		retval = false;
	}
	return retval;
} }

void dragon::Scaler::unpack(const midas::Event& event)
{
	/*!
	 * Unpacks scaler data directly into the various array structures.
	 */
	bool report = true;
	int bank_len;
	uint32_t* pcount = event.GetBankPointer<uint32_t>(variables.bk_count, &bank_len, report, true);
	if ( check_bank_len(MAX_CHANNELS, bank_len, variables.bk_count) )
		std::copy(pcount, pcount + bank_len, count);

	uint32_t* psum = event.GetBankPointer<uint32_t>(variables.bk_sum, &bank_len, report, true);
	if ( check_bank_len(MAX_CHANNELS, bank_len, variables.bk_sum) )
		std::copy(psum, psum + bank_len, sum);

	double* prate = event.GetBankPointer<double>(variables.bk_rate, &bank_len, report, true);
	if ( check_bank_len(MAX_CHANNELS, bank_len, variables.bk_rate) )
		std::copy(prate, prate + bank_len, rate);
}

const std::string& dragon::Scaler::channel_name(int ch) const
{
	/*!
	 * \param ch Channel number
	 */
	if (ch >= 0 && ch < MAX_CHANNELS) return variables.names[ch];
	dutils::Error("dragon::Scaler::channel_name", __FILE__, __LINE__)
		<< "Invalid channel number: " << ch << ". Valid arguments are 0 <= ch < " << MAX_CHANNELS;
	static std::string junk = "";
	return junk;
}


// ==================== Class Scaler::Variables ==================== //

dragon::Scaler::Variables::Variables():
	odb_path("/dragon/scaler/")
{
	/*!
	 * Calls reset()
	 */
	reset();
}

void dragon::Scaler::Variables::reset()
{
	/*! Resets every channel to a default name: \c channel_n*/
	for (int i=0; i< MAX_CHANNELS; ++i) {
		std::stringstream strm_name;
		strm_name << "channel_" << i;
		names[i] = strm_name.str();
	}

	/// - Set bank names to dummy values
	dutils::set_bank_name("NULD", bk_count);
	dutils::set_bank_name("NULR", bk_rate);
	dutils::set_bank_name("NULS", bk_sum);
}

bool dragon::Scaler::Variables::set(const char* dbfile, const char* dir)
{
	/*!
	 * \param [in] dbfile Name of the database file from which to read variables ("online" for the ODB).
	 * \param [in] dir String specifying the scaler subdirectory in which to look for
	 *  the variables data, e.g. "head" looks in "/dragon/scaler/head", "tail":
	 * "/dragon/scaler/tail".
	 */
	midas::Database db(dbfile);
	if(db.IsZombie()) {
		dutils::Error("Scaler::Variables::Set", __FILE__, __LINE__)
			<< "Zombie database: " << dbfile;
		return false;
	}
	return this->set(&db, dir);
}

bool dragon::Scaler::Variables::set(const midas::Database* db, const char* dir)
{
	/*!
	 * \param [in] db Pointer to a valid database.
	 * \param [in] dir String specifying the scaler subdirectory in which to look for
	 *  the variables data, e.g. "head" looks in "/dragon/scaler/head", "tail":
	 * "/dragon/scaler/tail".
	 */
	std::string path1 = dir;
	if (path1 != "head" && path1 != "tail" && path1 != "aux") {
		dutils::Error("Scaler::Variables::set", __FILE__, __LINE__)
			<< "Invalid subdirectory: \"" << path1 << "\"."
			<< "Must be either \"head\" or \"tail\"";
		return false;
	}
	path1 = odb_path + path1;

	bool success = check_db(db, "dragon::Scaler");

	if(success) success = db->ReadArray((path1 + "/names").c_str(), names, MAX_CHANNELS);

	if(success) success = odb_set_bank(&bk_count, db, (path1 + "/bank_names/count").c_str());
	if(success) success = odb_set_bank(&bk_rate, db, (path1 + "/bank_names/rate").c_str());
	if(success) success = odb_set_bank(&bk_sum, db, (path1 + "/bank_names/sum").c_str());

	return success;
}


// ==================== Class dragon::Coinc ==================== //

dragon::Coinc::Coinc()
{
	/// ::
	reset();
}

dragon::Coinc::Coinc(const dragon::Head& head_, const dragon::Tail& tail_)
{
	/// ::
	compose_event(head_, tail_);
}

void dragon::Coinc::reset()
{
	/// ::
	head.reset();
	tail.reset();
	dutils::reset_data(xtrig, xtofh, xtoft);
}

bool dragon::Coinc::set_variables(const char* dbfile)
{
	/*!
	 * \param [in] dbfile Name of the ODB file (*.xml or *.mid) from which to read
	 * \note Passing \c "online" looks at the experiment's ODB, if connected
	 */
	return do_setv(this, dbfile);
}

bool dragon::Coinc::set_variables(const midas::Database* db)
{
	/*!
	 * \param [in] db Pointer to a constructed database from which to read the variables.
	 */
	bool success = check_db(db, "dragon::Coinc::set_varibles");
	if(success) success = head.set_variables(db);
	if(success) success = tail.set_variables(db);
	if(success) success = variables.set(db);

	return success;
}

void dragon::Coinc::compose_event(const dragon::Head& head_, const dragon::Tail& tail_)
{
	/*!
	 * \param [in] head_ Head (gamma-ray) event
	 * \param [in] tail_ Tail (heavy-ion) event
	 */
	head = head_;
	tail = tail_;
}

void dragon::Coinc::unpack(const midas::CoincEvent& coincEvent)
{
	/*!
	 * Calls the respective unpack() functions on the head and tail part
	 * of the coincidence events.
	 */
	head.unpack( *(coincEvent.fGamma) );
	tail.unpack( *(coincEvent.fHeavyIon) );
}

void dragon::Coinc::calculate()
{
	/*!
	 * Does head and tail calculations.
	 */
	head.calculate();
	tail.calculate();
	xtrig = dutils::calculate_tof(tail.io32.tsc4.trig_time, head.io32.tsc4.trig_time);
	xtoft = dutils::calculate_tof(tail.tcal0, tail.tcalx);
	xtofh = dutils::calculate_tof(head.tcalx, head.tcal0);
}


// ==================== Class dragon::Coinc::Variables ==================== //

dragon::Coinc::Variables::Variables()
{
	/*! Calls reset() */
	reset();
}

void dragon::Coinc::Variables::reset()
{
	/*! Reads defaults from Defaults.hxx */
	window = DRAGON_DEFAULT_COINC_WINDOW;
	buffer_time = DRAGON_DEFAULT_COINC_BUFFER_TIME;
}

bool dragon::Coinc::Variables::set(const char* dbfile)
{
	/*!
	 * \param [in] dbfile Name of the database file from which to read variables ("online" for the ODB).
	 */
	return do_setv(this, dbfile);
}

bool dragon::Coinc::Variables::set(const midas::Database* db)
{
	/// ::
	bool success = check_db(db, "dragon::Coinc");

	if(success) success = db->ReadValue("/dragon/coinc/variables/window", window);
	if(success) success = db->ReadValue("/dragon/coinc/variables/buffer_time", buffer_time);

	return success;
}


// ====================== Class dragon::Epics ====================== //

dragon::Epics::Epics()
{
	/*! Calls reset() */
	reset();
}

void dragon::Epics::reset()
{
	/*! Sets all ch and val to vme::NONE, header to all zeroes. */
	const midas::Event::Header temp = { 0, 0, 0, 0, 0 };
	header = temp;
	utils::reset_data(ch, val);
}

bool dragon::Epics::set_variables(const char* dbfile)
{
	/*!
	 * Delegates to dragon::Epics::Variables::Set()
	 */
	return variables.set(dbfile);
}

bool dragon::Epics::set_variables(const midas::Database* db)
{
	/*!
	 * Delegates to dragon::Epics::Variables::Set()
	 */
	return variables.set(db);
}

void dragon::Epics::unpack(const midas::Event& event)
{
	/// ::
	bool report = true;
	int bank_len;
	float* pch = event.GetBankPointer<float>(variables.bkname, &bank_len, report, true);

	/// - Read channel number and value from the bank
	if(bank_len == 2) {
		ch  = static_cast<int32_t>(*pch++);
		val = *pch++;
	} else {
		dutils::Error("Epics::unpack", __FILE__, __LINE__)
			<< "Bank len != 2, skipping!";
	}

	/// - Copy event header
	event.CopyHeader(header);
}

const std::string& dragon::Epics::channel_name(int ch) const
{
	/*!
	 * \param ch Channel number
	 */
	if (ch >= 0 && (unsigned)ch < variables.names.size())
		return variables.names[ch];
	dutils::Error("dragon::Epics::channel_name", __FILE__, __LINE__)
		<< "Invalid channel number: " << ch << ". Valid arguments are 0 <= ch < " << variables.names.size();
	static std::string junk = "";
	return junk;
}


// ====================== Class dragon::Epics::Variables ====================== //

dragon::Epics::Variables::Variables()
{
	/*! Calls reset() */
	reset();
}

void dragon::Epics::Variables::reset()
{
	/// Call `vector::clear()` on names
	names.clear();
	/// Set bank name to `"EPCS"`
	dutils::set_bank_name("EPCS", bkname);
}

bool dragon::Epics::Variables::set(const char* dbfile)
{
	/*!
	 * \param [in] dbfile Name of the database file from which to read variables ("online" for the ODB).
	 */
	midas::Database db(dbfile);
	if(db.IsZombie()) {
		dutils::Error("Epics::Variables::Set", __FILE__, __LINE__)
			<< "Zombie database: " << dbfile;
		return false;
	}
	return this->set(&db);
}

bool dragon::Epics::Variables::set(const midas::Database* db)
{
	/*!
	 * \param [in] db Pointer to a valid database.
	 */
	bool success = check_db(db, "dragon::Epics");

	int length = db->ReadArrayLength("/Equipment/Epics/Settings/Names EPICS_Values");
	if(length < 0) success = false;

	if(success) {
		names.clear();
		names.resize(length);
		success = db->ReadArray("/Equipment/Epics/Settings/Names EPICS_Values", &names[0], length);
	}
	if(success) success = odb_set_bank(&bkname, db, "/dragon/epics/bank_name");

	return success;
}
