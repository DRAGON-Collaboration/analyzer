///
/// \file Dragon.cxx
/// \author G. Christian
/// \brief Implements Dragon.hxx
///
#include <string>
#include <iostream>
#include "midas/Database.hxx"
#include "utils/Functions.hxx"
#include "utils/Valid.hxx"
#include "utils/ErrorDragon.hxx"
#include "utils/definitions.h"
#include "Channels.h"
#include "Dragon.hxx"




// ==================== Class dragon::RunParameters ==================== //



dragon::RunParameters::RunParameters()
{
	reset();
}

void dragon::RunParameters::reset()
{
	double* parrays[4] = { run_start, run_stop, trigger_start, trigger_stop };
	for(int i=0; i< 4; ++i) {
		std::fill(parrays[i], parrays[i] + MAX_FRONTENDS, 0.);
	}
}

void dragon::RunParameters::read_data(const midas::Database& db)
{
	if(db.IsZombie()) {
		utils::err::Error("dragon::RunParameters::read_data") << "Zombie database";
		return;
	}

	bool success;
	success = db.ReadArray("/Experiment/Run Parameters/TSC_RunStart", run_start, MAX_FRONTENDS);
	success = db.ReadArray("/Experiment/Run Parameters/TSC_RunStop", run_stop, MAX_FRONTENDS);
	success = db.ReadArray("/Experiment/Run Parameters/TSC_TriggerStart", trigger_start, MAX_FRONTENDS);
	success = db.ReadArray("/Experiment/Run Parameters/TSC_TriggerStop", trigger_stop, MAX_FRONTENDS);

	if(!success) {
		utils::err::Error("dragon::RunParameters::read_data") << "Failed reading one of the ODB parameters.";
	}
}


// ==================== Class dragon::Bgo ==================== //

dragon::Bgo::Bgo():
	variables()
{
	reset();
}

void dragon::Bgo::reset()
{
	utils::reset_array(MAX_CHANNELS, ecal);
	utils::reset_array(MAX_CHANNELS, tcal);
	utils::reset_array(MAX_CHANNELS, esort);
	utils::reset_data(sum, x0, y0, z0, t0, hit0);
}

void dragon::Bgo::read_data(const vme::V792& adc, const vme::V1190& tdc)
{
	/*!
	 * Does channel mapping from ADC and TDC modules
	 * \param [in] adc Adc module
	 * \param [in] tdc Tdc module
	 */
	utils::channel_map(ecal, MAX_CHANNELS, variables.adc.channel, adc);
	utils::channel_map(tcal, MAX_CHANNELS, variables.tdc.channel, tdc);
}

void dragon::Bgo::calculate()
{
	/*!
	 * Does the following:
	 */
	/// - Pedestal subtract and calibrate energy values
	utils::pedestal_subtract(ecal, MAX_CHANNELS, variables.adc);
	utils::linear_calibrate(ecal, MAX_CHANNELS, variables.adc);

	/// - Calibrate time values
	utils::linear_calibrate(tcal, MAX_CHANNELS, variables.tdc);

	/// - Calculate descending-order energy indices and map into \c esort[]
	int isort[MAX_CHANNELS];
	utils::index_sort(ecal, ecal+MAX_CHANNELS, isort, utils::greater_and_valid<double>());
	utils::channel_map_from_array(esort, MAX_CHANNELS, isort, ecal);

	/// - If we have at least one good hit, calculate sum, x0, y0, z0, and t0
	if(utils::is_valid(esort[0])) {
		hit0 = isort[0];
		sum = utils::calculate_sum(ecal, ecal + MAX_CHANNELS);
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
	utils::index_fill(adc.channel, adc.channel + MAX_CHANNELS, BGO_ADC0);
	std::fill(adc.pedestal, adc.pedestal + MAX_CHANNELS, 0);
	std::fill(adc.offset, adc.offset + MAX_CHANNELS, 0.);
	std::fill(adc.slope, adc.slope + MAX_CHANNELS, 1.);

	utils::index_fill(tdc.channel, tdc.channel + MAX_CHANNELS, BGO_TDC0);
	std::fill(tdc.offset, tdc.offset + MAX_CHANNELS, 0.);
	std::fill(tdc.slope, tdc.slope + MAX_CHANNELS, 1.);

	// Coordinates copied from dragon_event.c [old analyzer]
	const double bgoCoords[MAX_CHANNELS][3] =  {
		{ 4, -2.6,  9.2}, { 0, -4.8,-15.3}, { 0,-10.1,-12.2}, { 0,  5.0,-12.2}, { 0,  9.9, -9.2},
		{ 0,  8.0, -3.1}, { 0,  8.0,  3.1}, { 0,  9.9,  9.2}, { 0,-10.1, 12.2}, { 0,  5.0, 12.2},
		{ 0, -4.8, 15.3}, {-4, -2.6, -9.2}, { 4, -2.6, -9.2}, {-4, -7.9, -6.1}, { 4, -7.9, -6.1},
		{-4,  2.7, -6.1}, { 4,  2.7, -6.1}, {-4, -2.6, -3.1}, { 4, -2.6, -3.1}, {-4, -7.9,    0},
		{ 4, -7.9,    0}, {-4,  2.7,    0}, { 4,  2.7,    0}, {-4, -2.6,  3.1}, { 4, -2.6,  3.1},
		{-4, -7.9,  6.1}, { 4, -7.9,  6.1}, {-4,  2.7,  6.1}, { 4,  2.7,  6.1}, {-4, -2.6,  9.2}
	};

	for(int i=0; i< MAX_CHANNELS; ++i) {
		pos.x[i] = bgoCoords[i][0];
		pos.y[i] = bgoCoords[i][1];
		pos.z[i] = bgoCoords[i][2];
	}
}

void dragon::Bgo::Variables::set(const char* odb)
{
	/*!
	 * \param [in] odb_file Path of the odb file from which you are extracting variable values
	 */
	midas::Database database(odb);
	if(database.IsZombie()) return;

	database.ReadArray("/dragon/bgo/variables/adc/channel",  adc.channel,  MAX_CHANNELS);
	database.ReadArray("/dragon/bgo/variables/adc/pedestal", adc.pedestal, MAX_CHANNELS);
	database.ReadArray("/dragon/bgo/variables/adc/slope",    adc.slope,    MAX_CHANNELS);
	database.ReadArray("/dragon/bgo/variables/adc/offset",   adc.offset,   MAX_CHANNELS);

	database.ReadArray("/dragon/bgo/variables/tdc/channel", tdc.channel, MAX_CHANNELS);
	database.ReadArray("/dragon/bgo/variables/tdc/slope",   tdc.slope,   MAX_CHANNELS);
	database.ReadArray("/dragon/bgo/variables/tdc/offset",  tdc.offset,  MAX_CHANNELS);

	database.ReadArray("/dragon/bgo/variables/position/x",  pos.x, MAX_CHANNELS);
	database.ReadArray("/dragon/bgo/variables/position/y",  pos.y, MAX_CHANNELS);
	database.ReadArray("/dragon/bgo/variables/position/z",  pos.z, MAX_CHANNELS);
}


// ================ class dragon::Dsssd ========================= //

dragon::Dsssd::Dsssd():
	variables()
{
	reset();
}

void dragon::Dsssd::reset()
{
	utils::reset_data(efront, eback, hit_front, hit_back);
	utils::reset_array(MAX_CHANNELS, ecal);
}

void dragon::Dsssd::read_data(const vme::V785 adcs[], const vme::V1190& tdc)
{
	/*!
	 * Copies adc data into \c this->ecal[] with channel and module mapping taken
	 * from variables.adc.channel and variables.adc.modules
	 *  
	 * Delegates work to utils::channel_map()
	 * \param [in] adcs Array of vme::V785 adc modules from which data can be taken
	 * \param [in] tdc vme::V1190 tdc module from which data can be read
	 */
	utils::channel_map(ecal, MAX_CHANNELS, variables.adc.channel, variables.adc.module, adcs);
	utils::channel_map(tcal, variables.tdc.channel, tdc);
}

void dragon::Dsssd::calculate()
{
	/*!
	 * Does a linear transformation on each element in \c this->ecal[] using the slopes and offsets
	 * from variables.adc_slope and variables.adc_offset, respectively. Also calibrates the TDC
	 * signal; calculates efront, hit_front, eback, and hit_back.
	 *
	 * Delegates the work to utils::pedestal_subtract and utils::linear_calibrate
	 */
	utils::pedestal_subtract(ecal, MAX_CHANNELS, variables.adc);
	utils::linear_calibrate(ecal, MAX_CHANNELS, variables.adc);

	utils::linear_calibrate(tcal, variables.tdc);

	const double* const pmax_front = std::max_element(ecal, ecal+16);
	efront = *pmax_front;
	hit_front = pmax_front - ecal;

	const double* const pmax_back  = std::max_element(ecal+16, ecal+32);
	eback  = *pmax_back;
	hit_back = pmax_back - ecal;
}


// ================ class dragon::Dsssd::Variables ================ //

dragon::Dsssd::Variables::Variables()
{
	/*! Calls reset() */
	reset();
}

void dragon::Dsssd::Variables::reset()
{
	std::fill(adc.module, adc.module + MAX_CHANNELS, DSSSD_MODULE);
	utils::index_fill(adc.channel, adc.channel + MAX_CHANNELS, DSSSD_ADC0);

	std::fill(adc.pedestal, adc.pedestal + MAX_CHANNELS, 0);
	std::fill(adc.slope,  adc.slope + MAX_CHANNELS, 1.);
	std::fill(adc.offset, adc.offset + MAX_CHANNELS, 0.);

	tdc.module  = 0; // unused
	tdc.channel = DSSSD_TDC0;
	tdc.slope   = 1.;
	tdc.offset  = 0.;
}

void dragon::Dsssd::Variables::set(const char* odb)
{
	/*!
	 * \param [in] odb_file Path of the odb file from which you are extracting variable values
	 */
	midas::Database database(odb);
	if(database.IsZombie()) return;

	database.ReadArray("/dragon/dsssd/variables/adc/module",   adc.module,   MAX_CHANNELS);
	database.ReadArray("/dragon/dsssd/variables/adc/channel",  adc.channel,  MAX_CHANNELS);
	database.ReadArray("/dragon/dsssd/variables/adc/pedestal", adc.pedestal, MAX_CHANNELS);
	database.ReadArray("/dragon/dsssd/variables/adc/slope",    adc.slope,    MAX_CHANNELS);
	database.ReadArray("/dragon/dsssd/variables/adc/offset",   adc.offset,   MAX_CHANNELS);

	database.ReadValue("/dragon/dsssd/variables/tdc/channel", tdc.channel);
	database.ReadValue("/dragon/dsssd/variables/tdc/slope",   tdc.slope);
	database.ReadValue("/dragon/dsssd/variables/tdc/offset",  tdc.offset);
}


// ================ Class dragon::IonChamber ================ //

dragon::IonChamber::IonChamber()
{
	reset();
}

void dragon::IonChamber::reset()
{
	utils::reset_array(MAX_CHANNELS, anode);
	utils::reset_data(tcal, sum);
}

void dragon::IonChamber::read_data(const vme::V785 adcs[], const vme::V1190& tdc)
{
	/*!
	 * \param modules Heavy-ion module structure
	 * \param [in] v1190_trigger_ch Channel number of the v1190b trigger
	 */
	utils::channel_map(anode, MAX_CHANNELS, variables.adc.channel, variables.adc.module, adcs);
	utils::channel_map(tcal, variables.tdc.channel, tdc);
}

void dragon::IonChamber::calculate()
{
	/*!
	 * Calibrates anode and time signals, calculates anode sum
	 */
	utils::pedestal_subtract(anode, MAX_CHANNELS, variables.adc);
	utils::linear_calibrate(anode, MAX_CHANNELS, variables.adc);

	utils::linear_calibrate(tcal, variables.tdc);

	if(utils::is_valid(anode, MAX_CHANNELS)) {
		sum = utils::calculate_sum(anode, anode + MAX_CHANNELS);
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
	std::fill(adc.module, adc.module + MAX_CHANNELS, DEFAULT_HI_MODULE);
	utils::index_fill(adc.channel, adc.channel + MAX_CHANNELS, IC_ADC0);

	std::fill(adc.pedestal, adc.pedestal + MAX_CHANNELS, 0);
	std::fill(adc.slope, adc.slope + MAX_CHANNELS, 1.);
	std::fill(adc.offset, adc.offset + MAX_CHANNELS, 0.);

	tdc.module = 0; // unused
	tdc.channel = IC_TDC0;

	tdc.offset = 0.;
	tdc.slope = 1.;
}

void dragon::IonChamber::Variables::set(const char* odb)
{
	/*!
	 * \param [in] odb_file Path of the odb file from which you are extracting variable values
	 */
	midas::Database database(odb);
	if(database.IsZombie()) return;

	database.ReadArray("/dragon/ic/variables/adc/module",   adc.module,   MAX_CHANNELS);
	database.ReadArray("/dragon/ic/variables/adc/channel",  adc.channel,  MAX_CHANNELS);
	database.ReadArray("/dragon/ic/variables/adc/pedestal", adc.pedestal, MAX_CHANNELS);
	database.ReadArray("/dragon/ic/variables/adc/slope",    adc.slope,    MAX_CHANNELS);
	database.ReadArray("/dragon/ic/variables/adc/offset",   adc.offset,   MAX_CHANNELS);

	database.ReadValue("/dragon/ic/variables/tdc/channel",  tdc.channel);
	database.ReadValue("/dragon/ic/variables/tdc/slope",    tdc.slope);
	database.ReadValue("/dragon/ic/variables/tdc/offset",   tdc.offset);
}


// ================ class dragon::Mcp ================ //

dragon::Mcp::Mcp()
{
	reset();
}

void dragon::Mcp::reset()
{
	utils::reset_data(esum, tac, x, y);
	utils::reset_array(MAX_CHANNELS, anode);
	utils::reset_array(NUM_DETECTORS, tcal);
}

void dragon::Mcp::read_data(const vme::V785 adcs[], const vme::V1190& tdc)
{
	/*!
	 * Copies ADC and TDC data into anode, tcal, and tac parameters.
	 *
	 * Delegates work to utils::channel_map()
	 * \param [in] adcs Array of vme::V785 adc modules from which data can be taken
	 * \param [in] tdc vme::V1190 tdc module from which data can be read
	 */
	utils::channel_map(anode, MAX_CHANNELS, variables.adc.channel, variables.adc.module, adcs);
	utils::channel_map(tcal, NUM_DETECTORS, variables.tdc.channel, tdc);
	utils::channel_map(tac, variables.tac_adc.channel, variables.tac_adc.module, adcs);
}

void dragon::Mcp::calculate()
{
  /*!
	 * Pedestal-subtracts and calibrates anode, tcal, and tac values; calculates x- and
	 * y-positions.
	 *
	 * \note Position calculation algorithm taken from the MSc thesis of
	 * Michael Lamey, Simon Fraser University, 2001, p. 25; available online at
	 * <a href="http://dragon.triumf.ca/docs/Lamey_thesis.pdf">
	 * dragon.triumf.ca/docs/Lamey_thesis.pdf</a>
	 */
	utils::pedestal_subtract(anode, MAX_CHANNELS, variables.adc);
	utils::linear_calibrate(anode, MAX_CHANNELS, variables.adc);

	utils::linear_calibrate(tcal, NUM_DETECTORS, variables.tdc);

	utils::pedestal_subtract(tac, variables.tac_adc);
	utils::linear_calibrate(tac, variables.tac_adc);

	utils::calculate_sum(anode, anode + MAX_CHANNELS);

	// Position calculation if we have all valid anode signals
	if(utils::is_valid(anode, MAX_CHANNELS)) {
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
	std::fill(adc.module, adc.module + MAX_CHANNELS, DEFAULT_HI_MODULE);
	utils::index_fill(adc.channel, adc.channel + MAX_CHANNELS, MCP_ADC0);

	std::fill(adc.pedestal, adc.pedestal + MAX_CHANNELS, 0);
	std::fill(adc.offset, adc.offset + MAX_CHANNELS, 0.);
	std::fill(adc.slope, adc.slope + MAX_CHANNELS, 1.);

	tac_adc.module   = DEFAULT_HI_MODULE;
	tac_adc.channel  = MCP_TAC_ADC0;

	tac_adc.pedestal = 0;
	tac_adc.offset   = 0.;
	tac_adc.slope    = 1.;

	std::fill(tdc.module, tdc.module + NUM_DETECTORS, 0); // unused
	utils::index_fill(tdc.channel, tdc.channel + NUM_DETECTORS, MCP_TDC0);

	std::fill(tdc.offset, tdc.offset + NUM_DETECTORS, 0.);
	std::fill(tdc.slope, tdc.slope + NUM_DETECTORS, 1.);
}

void dragon::Mcp::Variables::set(const char* odb)
{
	/*!
	 * \param [in] odb_file Path of the odb file from which you are extracting variable values
	 */
	midas::Database database(odb);
	if(database.IsZombie()) return;

	database.ReadArray("/dragon/mcp/variables/adc/channel",  adc.channel,  MAX_CHANNELS);
	database.ReadArray("/dragon/mcp/variables/adc/module",   adc.module,   MAX_CHANNELS);
	database.ReadArray("/dragon/mcp/variables/adc/pedestal", adc.pedestal, MAX_CHANNELS);
	database.ReadArray("/dragon/mcp/variables/adc/slope",    adc.slope,    MAX_CHANNELS);
	database.ReadArray("/dragon/mcp/variables/adc/offset",   adc.offset,   MAX_CHANNELS);

	database.ReadValue("/dragon/mcp/variables/tac_adc/channel",  tac_adc.channel);
	database.ReadValue("/dragon/mcp/variables/tac_adc/module",   tac_adc.module);
	database.ReadValue("/dragon/mcp/variables/tac_adc/pedestal", tac_adc.pedestal);
	database.ReadValue("/dragon/mcp/variables/tac_adc/slope",    tac_adc.slope);
	database.ReadValue("/dragon/mcp/variables/tac_adc/offset",   tac_adc.offset);
	
	database.ReadArray("/dragon/mcp/variables/tdc/channel", tdc.channel, NUM_DETECTORS);
	database.ReadArray("/dragon/mcp/variables/tdc/slope",   tdc.slope,   NUM_DETECTORS);
	database.ReadArray("/dragon/mcp/variables/tdc/offset",  tdc.offset,  NUM_DETECTORS);
}


// ==================== Class dragon::SurfaceBarrier ==================== //

dragon::SurfaceBarrier::SurfaceBarrier()
{
	reset();
}

void dragon::SurfaceBarrier::reset()
{
	utils::reset_array(MAX_CHANNELS, ecal);
}

void dragon::SurfaceBarrier::read_data(const vme::V785 adcs[], const vme::V1190&)
{
	/*!
	 * Copies adc data into \c this->ecal[] with channel and module mapping taken
	 * from variables.adc.channel and variables.adc.modules
	 *  
	 * Delegates work to utils::channel_map()
	 * \param [in] adcs Array of vme::V785 adc modules from which data can be taken
	 */
	utils::channel_map(ecal, MAX_CHANNELS, variables.adc.channel, variables.adc.module, adcs);
}

void dragon::SurfaceBarrier::calculate()
{
  /*!
	 * Performs pedestal subtraction & calibration of energies.
	 *
	 * Delegates work to utils::pedestal_aubtract() and utils::linear_calibrate()
	 */
	utils::pedestal_subtract(ecal, MAX_CHANNELS, variables.adc);
	utils::linear_calibrate(ecal, MAX_CHANNELS, variables.adc);
}


// ==================== class dragon::SurfaceBarrier::Variables ==================== //

dragon::SurfaceBarrier::Variables::Variables()
{
	/*! Calls reset() */
	reset();
}

void dragon::SurfaceBarrier::Variables::reset()
{
	std::fill(adc.module, adc.module + MAX_CHANNELS, DEFAULT_HI_MODULE);
	utils::index_fill(adc.channel, adc.channel + MAX_CHANNELS, SB_ADC0);
	
	std::fill(adc.pedestal, adc.pedestal + MAX_CHANNELS, 0);
	std::fill(adc.offset, adc.offset + MAX_CHANNELS, 0.);
	std::fill(adc.slope, adc.slope + MAX_CHANNELS, 1.);
}

void dragon::SurfaceBarrier::Variables::set(const char* odb)
{
	/*!
	 * \param [in] odb_file Path of the odb file from which you are extracting variable values
	 */
	midas::Database database(odb);
	if(database.IsZombie()) return;

	database.ReadArray("/dragon/sb/variables/adc/module",   adc.module,   MAX_CHANNELS);
	database.ReadArray("/dragon/sb/variables/adc/channel",  adc.channel,  MAX_CHANNELS);
	database.ReadArray("/dragon/sb/variables/adc/pedestal", adc.pedestal, MAX_CHANNELS);
	database.ReadArray("/dragon/sb/variables/adc/slope",    adc.slope,    MAX_CHANNELS);
	database.ReadArray("/dragon/sb/variables/adc/offset",   adc.offset,   MAX_CHANNELS);
}


// ========================== Class dragon::NaI ========================== //

dragon::NaI::NaI()
{
	reset();
}

void dragon::NaI::reset()
{
	utils::reset_array(MAX_CHANNELS, ecal);
}

void dragon::NaI::read_data(const vme::V785 adcs[], const vme::V1190&)
{
	/*!
	 * Copies adc data into \c this->ecal[] with channel and module mapping taken
	 * from variables.adc.channel and variables.adc.modules
	 *
	 * Delegates work to utils::channel_map()
	 * \param [in] adcs Array of vme::V785 adc modules from which data can be taken
	 */
	utils::channel_map(ecal, MAX_CHANNELS, variables.adc.channel, variables.adc.module, adcs);
}

void dragon::NaI::calculate()
{
	utils::pedestal_subtract(ecal, MAX_CHANNELS, variables.adc);
	utils::linear_calibrate(ecal, MAX_CHANNELS, variables.adc);
}


// ====== class dragon::NaI::Variables ====== //

dragon::NaI::Variables::Variables()
{
	/*! Calls reset() */
	reset();
}

void dragon::NaI::Variables::reset()
{
	std::fill(adc.module, adc.module + MAX_CHANNELS, DEFAULT_HI_MODULE);
	utils::index_fill(adc.channel, adc.channel + MAX_CHANNELS, NAI_ADC0);
	std::fill(adc.pedestal, adc.pedestal + MAX_CHANNELS, 0);
	std::fill(adc.offset, adc.offset + MAX_CHANNELS, 0.);
	std::fill(adc.slope, adc.slope + MAX_CHANNELS, 1.);
}

void dragon::NaI::Variables::set(const char* odb)
{
	/*!
	 * \param [in] odb_file Path of the odb file from which you are extracting variable values
	 */
	midas::Database database(odb);
	if(database.IsZombie()) return;

	database.ReadArray("/dragon/nai/variables/adc/module",   adc.module,   MAX_CHANNELS);
	database.ReadArray("/dragon/nai/variables/adc/channel",  adc.channel,  MAX_CHANNELS);
	database.ReadArray("/dragon/nai/variables/adc/pedestal", adc.pedestal, MAX_CHANNELS);
	database.ReadArray("/dragon/nai/variables/adc/slope",    adc.slope,    MAX_CHANNELS);
	database.ReadArray("/dragon/nai/variables/adc/offset",   adc.offset,   MAX_CHANNELS);
}


// ====== class dragon::Ge ====== //

dragon::Ge::Ge()
{
	reset();
}

void dragon::Ge::reset()
{
	utils::reset_data(ecal);
}

void dragon::Ge::read_data(const vme::V785 adcs[], const vme::V1190&)
{
	/*!
	 * Copies adc data into \c this->ecal[] with channel and module mapping taken
	 * from variables.adc.channel and variables.adc.module
	 *
	 * Delegates work to utils::channel_map()
	 * \param [in] adcs Array of vme::V785 adc modules from which data can be taken
	 */
	utils::channel_map(ecal, variables.adc.channel, variables.adc.module, adcs);
}

void dragon::Ge::calculate()
{
	/*!
	 * Delegates work to functions in utils namespace.
	 */
	utils::pedestal_subtract(ecal, variables.adc);
	utils::linear_calibrate(ecal, variables.adc);
}


// ==================== Class dragon::Ge::Variables ==================== //

dragon::Ge::Variables::Variables()
{
	/*! Calls reset() */
	reset();
}

void dragon::Ge::Variables::reset()
{
	adc.module = DEFAULT_HI_MODULE;
	adc.channel = GE_ADC0;
	adc.pedestal = 0;
	adc.offset = 0.;
	adc.slope = 1.;
}

void dragon::Ge::Variables::set(const char* odb)
{
	/*!
	 * \param [in] odb_file Path of the odb file from which you are extracting variable values
	 */
	midas::Database database(odb);

	database.ReadValue("/dragon/ge/variables/adc/module",   adc.module);
	database.ReadValue("/dragon/ge/variables/adc/channel",  adc.channel);
	database.ReadValue("/dragon/ge/variables/adc/pedestal", adc.pedestal);
	database.ReadValue("/dragon/ge/variables/adc/slope",    adc.slope);
	database.ReadValue("/dragon/ge/variables/adc/offset",   adc.offset);
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
	utils::reset_data(     mcp
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
	mcp = utils::calculate_tof(tail->mcp.tcal[1], tail->mcp.tcal[0]);
#ifndef DRAGON_OMIT_DSSSD
	mcp_ic = utils::calculate_tof(tail->dsssd.tcal, tail->mcp.tcal[0]);
#endif
#ifndef DRAGON_OMIT_IC
	mcp_ic = utils::calculate_tof(tail->ic.tcal, tail->mcp.tcal[0]);
#endif
}


// ==================== Class dragon::Head ==================== //

dragon::Head::Head()
{
	/*!
	 * Set bank names, resets data to default values
	 */
	utils::Banks::Set(banks.io32, "VTRH");
	utils::Banks::Set(banks.adc,  "ADC0");
	utils::Banks::Set(banks.tdc,  "TDC0");
	utils::Banks::Set(banks.tsc,  "TSCH");
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
	utils::reset_data(tcalx, tcal0);
}

void dragon::Head::set_variables(const char* odb)
{
	/*!
	 * \param [in] odb_file Name of the ODB file
	 * \note Passing \c "online" looks at the online ODB.
	 */
	bgo.variables.set(odb);
	this->variables.set(odb);

	// Set bank names
	midas::Database database(odb);
	if(database.IsZombie()) return;

	utils::Banks::OdbSet(banks.io32, database, "dragon/head/bank_names/io32");
	utils::Banks::OdbSet(banks.adc,  database, "dragon/head/bank_names/adc");
	utils::Banks::OdbSet(banks.tdc,  database, "dragon/head/bank_names/tdc");
	utils::Banks::OdbSet(banks.tsc,  database, "dragon/head/bank_names/tsc");
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
	io32.unpack (event, banks.io32, report);
	v792.unpack (event, banks.adc , report);
	v1190.unpack(event, banks.tdc,  report);
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
	 * In the specific implementation, we delegate to functions in the dragon::Bgo class.
	 */
	bgo.read_data(v792, v1190);
	bgo.calculate();
	tcal0 = bgo.t0;
	utils::channel_map(tcalx, variables.xtdc.channel, v1190);
	utils::linear_calibrate(tcalx, variables.xtdc); 
}


// ================ Class dragon::Head::Variables ================ //

dragon::Head::Variables::Variables()
{
	reset();
}

void dragon::Head::Variables::reset()
{
	xtdc.channel = HEAD_CROSS_TDC;
	xtdc.slope  = 1.;
	xtdc.offset = 0.;
}

void dragon::Head::Variables::set(const char* odb)
{
	midas::Database database(odb);
	if(database.IsZombie()) return;

	database.ReadValue("/dragon/head/variables/xtdc/channel", xtdc.channel);
	database.ReadValue("/dragon/head/variables/xtdc/slope",   xtdc.slope);
	database.ReadValue("/dragon/head/variables/xtdc/offset",  xtdc.offset);
}


// ================ Class dragon::Tail ================ //

dragon::Tail::Tail()
{
	utils::Banks::Set(banks.io32,   "VTRT");
	utils::Banks::Set(banks.adc[0], "TLQ0");
	utils::Banks::Set(banks.adc[1], "TLQ1");
	utils::Banks::Set(banks.tdc,    "TLT0");
	utils::Banks::Set(banks.tsc,    "TSCT");
	reset();
}

void dragon::Tail::reset()
{
	io32.reset();
	v1190.reset();
	for (int i=0; i< NUM_ADC; ++i) {
		v785[i].reset();
	}
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
	utils::reset_data(tcalx, tcal0);
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
	io32.unpack (event, banks.io32, report);
	for (int i=0; i< NUM_ADC; ++i) {
		v785[i].unpack(event, banks.adc[i], report);
	}
	v1190.unpack(event, banks.tdc, report);
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
	
	/// - Perform calibrations, higher-order calculations, etc
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

	tof.calculate(this);
	utils::channel_map(tcalx, variables.xtdc.channel, v1190);
	utils::linear_calibrate(tcalx, variables.xtdc);

	// Decide what detector to use for tcal0
#if   defined DRAGON_OMIT_IC     && !defined DRAGON_OMIT_DSSSD
	tcal0 = dsssd.tcal;  // use dsssd
#elif defined DRAGON_OMIT_DSSSD  && !defined DRAGON_OMIT_IC
	tcal0 = ic.tcal;     // use ic
#elif !defined DRAGON_OMIT_DSSSD && !defined DRAGON_OMIT_IC
	tcal0 = mcp.tcal[0]; // use mcp (no end detector)
#else // try to figure it out from present signals
	if (0)  { ; }
	else if (utils::is_valid(dsssd.tcal) && !utils::is_valid(ic.tcal))
		tcal0 = dsssd.tcal[0];
	else if (utils::is_valid(ic.tcal)    && !utils::is_valid(dsssd.tcal))
		tcal0 = ic.tcal[0];
	else if (!utils::is_valid(ic.tcal)   && !utils::is_valid(dsssd.tcal))
		tcal0 = mcp.tcal[0];
	else {
		tcal0 = dsssd.tcal[0];
		dragon::err::Warning("dragon::Tail::calculate", false)
			<< "Both DSSSD and IC have valid times; using DSSSD for tcal0.";
	}
#endif
}

void dragon::Tail::set_variables(const char* odb)
{
	/*!
	 * \param [in] odb_file Name of the ODB file
	 * \note Passing \c "online" looks at the online ODB.
	 */
#ifndef DRAGON_OMIT_DSSSD
	dsssd.variables.set(odb);
#endif
#ifndef DRAGON_OMIT_IC
	ic.variables.set(odb);
#endif
	mcp.variables.set(odb);
	sb.variables.set(odb);
#ifndef DRAGON_OMIT_NAI
	nai.variables.set(odb);
#endif
#ifndef DRAGON_OMIT_GE
	ge.variables.set(odb);
#endif
	this->variables.set(odb);

	// Set bank names
	midas::Database database(odb);
	if(database.IsZombie()) return;

	utils::Banks::OdbSet(banks.io32,   database, "dragon/tail/bank_names/io32");
	utils::Banks::OdbSet(banks.adc[0], database, "dragon/tail/bank_names/adc[0]");
	utils::Banks::OdbSet(banks.adc[1], database, "dragon/tail/bank_names/adc[1]");
	utils::Banks::OdbSet(banks.tdc,    database, "dragon/tail/bank_names/tdc");
	utils::Banks::OdbSet(banks.tsc,    database, "dragon/tail/bank_names/tsc");
}

// ================ Class dragon::Tail::Variables ================ //

dragon::Tail::Variables::Variables()
{
	reset();
}

void dragon::Tail::Variables::reset()
{
	xtdc.channel = TAIL_CROSS_TDC;
	xtdc.slope  = 1.;
	xtdc.offset = 0.;
}

void dragon::Tail::Variables::set(const char* odb)
{
	midas::Database database(odb);
	if(database.IsZombie()) return;

	database.ReadValue("/dragon/tail/variables/xtdc/channel", xtdc.channel);
	database.ReadValue("/dragon/tail/variables/xtdc/slope",   xtdc.slope);
	database.ReadValue("/dragon/tail/variables/xtdc/offset",  xtdc.offset);
}


// ==================== Class dragon::Scaler ==================== //

dragon::Scaler::Scaler(const char* name):
	variables(name)
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

void dragon::Scaler::set_variables(const char* odb)
{
	/*!
	 * Delegates to dragon::Scaler::Variables::Set()
	 */
	variables.set(odb);
}

namespace {
inline bool check_bank_len(int expected, int gotten, const char* bkname)
{
	bool retval = true;
	if (expected != gotten) {
		utils::err::Error("dragon::Scaler::unpack")
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
	uint32_t* pcount = event.GetBankPointer<uint32_t>(variables.bank_names.count, &bank_len, report, true);
	if ( check_bank_len(MAX_CHANNELS, bank_len, variables.bank_names.count) )
		std::copy(pcount, pcount + bank_len, count);

	uint32_t* psum = event.GetBankPointer<uint32_t>(variables.bank_names.sum, &bank_len, report, true);
	if ( check_bank_len(MAX_CHANNELS, bank_len, variables.bank_names.sum) )
		std::copy(psum, psum + bank_len, sum);

	double* prate = event.GetBankPointer<double>(variables.bank_names.rate, &bank_len, report, true);
	if ( check_bank_len(MAX_CHANNELS, bank_len, variables.bank_names.rate) )
		std::copy(prate, prate + bank_len, rate);
}

const std::string& dragon::Scaler::channel_name(int ch) const
{
	/*!
	 * \param ch Channel number
	 */
	if (ch >= 0 && ch < MAX_CHANNELS) return variables.names[ch];
	utils::err::Error("dragon::Scaler::channel_name")
		<< "Invalid channel number: " << ch << ". Valid arguments are 0 <= ch < " << MAX_CHANNELS;
	static std::string junk = "";
	return junk;
}


// ==================== Class Scaler::Variables ==================== //

dragon::Scaler::Variables::Variables(const char* name):
	odb_path("/dragon/scaler/")
{
	/*!
	 * Calls reset() and sets odb path to:
	 * <tt>"/dragon/scaler/<name>"</tt>
	 */
	reset();
	odb_path += name;

	if(std::string(name) != "head" && std::string(name) != "tail") {
		utils::err::Warning("Scaler::Variables")
			<< "Invalid name specification: \"" << name << "\". "
			<< "Synchronization with the ODB will not work as a result!"
			<< DRAGON_ERR_FILE_LINE;
	}
}

void dragon::Scaler::Variables::reset()
{
	/*! Resets every channel to a default name: \c channel_n */
	for (int i=0; i< MAX_CHANNELS; ++i) {
		std::stringstream strm_name;
		strm_name << "channel_" << i;
		names[i] = strm_name.str();
	}
	set_bank_names("SCH");
}

void dragon::Scaler::Variables::set(const char* odb)
{
	midas::Database database(odb);
	if(database.IsZombie()) {
		utils::err::Error("Scaler::Variables::set")
			<< "Zombie database" << DRAGON_ERR_FILE_LINE;
		return;
	}

	database.ReadArray((odb_path + "/names").c_str(), names, MAX_CHANNELS);

	// std::string sCount, sSum, sRate;
	// database.ReadValue((odb_path + "/bank_names/count").c_str(), sCount);
	// database.ReadValue((odb_path + "/bank_names/rate").c_str(), sRate);
	// database.ReadValue((odb_path + "/bank_names/sum").c_str(), sSum);
	// utils::Banks::Set(bank_names.count, sCount.c_str());
	// utils::Banks::Set(bank_names.rate,  sRate.c_str());
	// utils::Banks::Set(bank_names.sum ,  sSum.c_str());

	utils::Banks::OdbSet(bank_names.count, database, (odb_path + "/bank_names/count").c_str());
	utils::Banks::OdbSet(bank_names.rate, database, (odb_path + "/bank_names/rate").c_str());
	utils::Banks::OdbSet(bank_names.sum, database, (odb_path + "/bank_names/sum").c_str());
}

void dragon::Scaler::Variables::set_bank_names(const char* base)
{
	std::string strbase(base);
	if(strbase.size() != 3) {
		utils::err::Warning("dragon::Scaler::Variables::set_bank_names")
			<< "Length of base: \"" << base << "\" != 3. Truncating...";
		if(strbase.size() > 3) strbase = strbase.substr(0, 3);
		else while (strbase.size() != 3) strbase.push_back('0');
		std::cerr << "Base = " << strbase << "\n";
	}
	utils::Banks::Set(bank_names.count, (strbase + "D").c_str());
	utils::Banks::Set(bank_names.rate,  (strbase + "R").c_str());
	utils::Banks::Set(bank_names.sum ,  (strbase + "S").c_str());
}


// ==================== Class dragon::Coinc ==================== //

dragon::Coinc::Coinc()
{
	reset();
}

dragon::Coinc::Coinc(const dragon::Head& head, const dragon::Tail& tail)
{
	compose_event(head, tail);
}

void dragon::Coinc::reset()
{
	head.reset();
	tail.reset();
	utils::reset_data(xtrig, xtofh, xtoft);
}

void dragon::Coinc::set_variables(const char* odb)
{
	/*!
	 * \param [in] odb_file Name of the ODB file (*.xml or *.mid) from which to read
	 * \note Passing \c "online" looks at the experiment's ODB, if connected
	 */
	head.set_variables(odb);
	tail.set_variables(odb);
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
	xtrig = utils::calculate_tof(tail.io32.tsc4.trig_time, head.io32.tsc4.trig_time);
	xtoft = utils::calculate_tof(tail.tcal0, tail.tcalx);
	xtofh = utils::calculate_tof(head.tcalx, head.tcal0);
}
