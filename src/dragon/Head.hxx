/// \file Head.hxx
/// \author G. Christian
/// \brief Defines classes for DRAGON head (gamma-ray) detectors.
#ifndef DRAGON_HEAD_HXX
#define DRAGON_HEAD_HXX
#include "utils/VariableStructs.hxx"
#include "utils/Banks.hxx"
#include "midas/Event.hxx"
#include "vme/V1190.hxx"
#include "vme/V792.hxx"
#include "vme/Io32.hxx"
#include "Bgo.hxx"

namespace dragon {

/// Collection of all head detectors and VME modules
class Head {
// Header //
public:
	/// Midas event header
	midas::Event::Header header; //#

	// Subclasses //
public:
	/// Times-of-flight measured by the Head ADC.
	class Tof {
 public:
		// Subclasses //
		class Variables {
	 private:
			/** @cond */
	 PRIVATE:
			/** @endcond */
			/// Crossover TDC channel variables
			utils::TdcVariables<1> xtdc;

			// Methods //
	 public:
			/// Sets data to defaults
			Variables();
			/// Sets data to defaults
			void reset();
			/// Sets data from ODB
			void set(const char* odb);

			/// Give parent Tof class internal access
			friend class dragon::Head::Tof;
		};
		/// Variables instance
		Variables variables; //!

		// Class data //
 private:
		/// "Parent" instance of dragon::Head
		const dragon::Head* fParent; //!

		/** @cond */
 PRIVATE:
		/** @endcond */
		/// Crossover tcal value [tail trigger]
		double tcalx;
		/// GAMMA_TRIGGER->TAIL_TRIGGER TOF
		double gamma_tail;

		// Class methods //
 public:
		/// Sets data to defaults
		Tof(const dragon::Head* parent);
		/// Sets data to dragon::NO_DATA
		void reset();
		/// Reads data from raw VME modules
		void read_data(const vme::V785&, const vme::V1190& v1190);
		/// Performs calibration of xover tdc and TOF calculations
		void calculate();
	};

// Class Data //
public:
	/// Bank names
	utils::EventBanks<1, 1> banks; //!

private:
  /** @cond */
PRIVATE:
	/** @endcond */
#ifdef DISPLAY_MODULES
	/// IO32 FPGA
	vme::Io32 io32;   //#

	/// CAEN V792 QDC
	vme::V792 v792;   //#

	/// CAEN V1190 TDC
	vme::V1190 v1190; //#
#else
	/// IO32 FPGA
	vme::Io32 io32;   //!

	/// CAEN V792 QDC
	vme::V792 v792;   //!

	/// CAEN V1190 TDC
	vme::V1190 v1190; //!
#endif // #ifdef DISPLAY_MODULES

	/// Bgo array
	dragon::Bgo bgo;

	/// Flight times
	dragon::Head::Tof tof;

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

	/// Allow Tof subclass internal access
	friend class dragon::Head::Tof;
};

} // namespace dragon


#endif
