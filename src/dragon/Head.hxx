/// \file Head.hxx
/// \author G. Christian
/// \brief Defines classes for DRAGON head (gamma-ray) detectors.
#ifndef DRAGON_HEAD_HXX
#define DRAGON_HEAD_HXX
#include "midas/Event.hxx"
#include "vme/V1190.hxx"
#include "vme/V792.hxx"
#include "vme/IO32.hxx"
#include "Bgo.hxx"

namespace dragon {

/// Collection of all head detectors and VME modules
class Head {
// Header //
public:
	/// Midas event header
	midas::Event::Header header; //#

// Class Data //
private:
  /** @cond */
PRIVATE:
	/** @endcond */
#ifdef DISPLAY_MODULES
	/// IO32 FPGA
	vme::IO32 io32;   //#

	/// CAEN V792 QDC
	vme::V792 v792;   //#

	/// CAEN V1190 TDC
	vme::V1190 v1190; //#
#else
	/// IO32 FPGA
	vme::IO32 io32;   //!

	/// CAEN V792 QDC
	vme::V792 v792;   //!

	/// CAEN V1190 TDC
	vme::V1190 v1190; //!
#endif // #ifdef DISPLAY_MODULES

	/// Bgo array
	dragon::Bgo bgo;

// Methods //
public:
	/// Initializes data values
	Head();

	/// Sets all data values to vme::NONE or other defaults
	void reset();

	///  Reads all variable values from an ODB (file or online)
	void set_variables(const char* odb_file);

	/// Unpack raw data into VME modules
	void unpack(const midas::Event& event);

	/// Calculate higher-level data for each detector, or across detectors
	void calculate();
};

} // namespace dragon


#endif
