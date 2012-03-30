#ifndef DRAGON_BGO_HXX
#define DRAGON_BGO_HXX
#include "vme/vme.hxx"

namespace dragon {

/// The BGO array
class Bgo {
private:
// ==== Private Data ==== //	 
	 /// CAEN v792 qdc module from which BGO energy data is obtained
	 vme::caen::V792 qdc; //!

	 // /// CAEN v1190b tdc module from which BGO timing data is obtained
	 // vme::caen::V1190b;   //!

public:
// ==== Classes ==== //	 
	 /// Bgo variables
	 class Variables {
	 public:
      /// \brief Maps ADC channel to BGO detector
			/// \details Example: setting ch[0] = 12, means that the 0th detector
			/// in the BGO array reads its charge data from challel 12 of the qdc.
			int qdc_ch[30];

      /// \brief Maps TDC channel to BGO detector
			/// \details Similar to qdc_ch
			int tdc_ch[30];

			/// Calibration slope
			double slope [30];

      /// Calibration offset
			double offset[30];

			/// Constructor, sets slope/offset to 1/0, *_ch[i] to i
			Variables();

			/// Destructor, nothing to do
			~Variables() { }

      /// Set variable values from an ODB file
			/// \todo Needs to be implemented once ODB is set up
			void set(const char* odb_file);
	 };

// ==== Data ==== //
	 /// Instance of Bgo::Variables for calibrating class data
	 Variables variables; //!
	 
	 /// Event counter
	 int32_t evt_count; //#

	 /// Raw charge signals, per detector
	 int16_t qraw[30];  //#

   /// Raw timing signale, per detector
	 int16_t traw[30];  //#

	 /// Calibrated charge signals, per detector
	 double  qcal[30];  //#

// ==== Methods ==== //	 
	 /// Constructor, initializes data values
	 Bgo();

	 /// Destructor, nothing to do
	 ~Bgo() { }

   /// Sets all data values to vme::NONE
	 void reset();

	 /// Unpack data from a midas event.
	 void unpack(TMidasEvent& event);

	 /// Calibrate raw data
	 void calibrate();
};
	 

} // namespace dragon


#endif
