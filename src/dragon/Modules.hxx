/// \file Modules.hxx
/// \author G. Christian
/// \brief Defines classes that encapsulate the VME modules used to read out data
///  for each DRAGON sub-system (heavy ion and gamma).
#ifndef DRAGON_MODULES_HXX
#define DRAGON_MODULES_HXX
#include "vme/IO32.hxx"
#include "vme/V792.hxx"
#include "vme/V1190.hxx"

namespace dragon {

namespace gamma {

/// Encapsulates all VME modules used to read out gamma-ray data.
class Modules {
private:
	/// CAEN v792 qdc (32 channel, integrating).
	vme::caen::V792 v792;

	/// CAEN v1190 tdc (64 channel).
	vme::caen::V1190 v1190;

	/// I032 FPGA
	vme::IO32 io32;

public:
	/// MIDAS bank names
	struct Banks {
		/// v792 bank name
		char v792[5];
		/// v1190b bank name
		char v1190[5];
		/// io32 bank name
		char io32[5];
	} banks; //!

public:
	/// Initialize all modules
	Modules();

	/// Nothing to do
	~Modules() { }

	/// Copy constructor
	Modules(const Modules& other);

	/// Equivalency operator
	Modules& operator= (const Modules& other);

	/// Reset all modules
	void reset();

	/// Unpack Midas event data into module data structures.
	/// \param [in] event Reference to the MidasEvent object being unpacked
	void unpack(const dragon::MidasEvent& event);

	/// Return data from a v792 channel
	/// \param ch channel numner
	int16_t v792_data(unsigned ch) const;

	/// Return data from a v1190 channel
	/// \param ch channel numne
	int16_t v1190_data(unsigned ch) const;

	/// Return io32 tstamp value
	int32_t tstamp() const;
};

} // namespace gamma

namespace hion {

/// Encapsulates all VME modules used to read out heavy-ion data.
class Modules {
private:
	/// Caen v785 adcs (32 channel, peak-sensing, x2)
	vme::caen::V785 v785[2];

	/// CAEN v1190 TDC
	vme::caen::V1190 v1190;

	/// IO32 FPGA
	vme::IO32 io32;

public:
	/// MIDAS bank names
	struct Banks {
		/// v792 bank name
		char v785[5][2];
		/// v1190b bank name
		char v1190[5];
		/// io32 bank name
		char io32[5];
	} banks; //!


public:
	/// Initialize all modules
	Modules();

	/// Nothing to do
	~Modules() { }

	/// Copy constructor
	Modules(const Modules& other);

	/// Equivalency operator
	Modules& operator= (const Modules& other);

	/// Reset all modules
	void reset();

	/// Unpack Midas event data into module data structures.
	/// \param [in] event Reference to the MidasEvent object being unpacked
	void unpack(const dragon::MidasEvent& event);	 

	/// Return data from a v785 channel
	/// \param which The ADC you want data from
	/// \param ch channel numner
	int16_t v785_data(unsigned which, unsigned ch) const;

	/// Return data from a v1190 channel
	/// \param ch channel numner
	int16_t v1190_data(unsigned ch) const;

	/// Return io32 tstamp value
	int32_t tstamp() const;
};

} // namespace hion

/// Wrapper for gamma and heavy-ion modules
struct Modules {
	/// Gamma
	gamma::Modules gamma;
	/// Heavy ion
	hion::Modules heavy_ion;
};

} // namespace dragon




#endif
