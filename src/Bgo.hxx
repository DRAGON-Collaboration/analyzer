#ifndef DRAGON_BGO_HXX
#define DRAGON_BGO_HXX
#include <vector>
#include "Modules.hxx"

namespace dragon {

/// The BGO array
class Bgo {
public:
// ==== Const Data ==== //
	 /// Number of channels in the BGO array
	 const int nch;

// ==== Classes ==== //	 
	 /// Bgo variables
	 class Variables {
	 private:
			/// Number of channels in the BGO array
			const int nch;
	 public:
      /// \brief Maps ADC channel to BGO detector
			/// \details Example: setting ch[0] = 12, means that the 0th detector
			/// in the BGO array reads its charge data from channel 12 of the qdc.
			std::vector<int> qdc_ch;

      /// \brief Maps TDC channel to BGO detector
			/// \details Similar to qdc_ch
			std::vector<int> tdc_ch;

			/// Constructor, sets nch to ch_, *_ch[i] to i
			Variables(int nch_);

			/// Destructor, nothing to do
			~Variables() { }

      /// Set variable values from an ODB file
			/// \todo Needs to be implemented once ODB is set up
			void set(const char* odb_file);
	 };

// ==== Data ==== //
	 /// Instance of Bgo::Variables for mapping digitizer ch -> bgo detector
	 Variables variables; //!
	 
	 /// Event counter
	 int32_t evt_count; //#

	 /// Raw charge signals, per detector
	 std::vector<int16_t> q; //#

   /// Raw timing signals, per detector
	 std::vector<int16_t> t; //#

// ==== Methods ==== //	 
	 /// Constructor, initializes data values
	 Bgo();

	 /// Destructor, nothing to do
	 ~Bgo() { }

   /// Sets all data values to vme::NONE
	 void reset();

	 /// Read data from a midas event.
	 /// \param modules Electronic modules structure from which the data are read
	 void read_data(const dragon::gamma::Modules& modules);
};
	 

} // namespace dragon


#endif
