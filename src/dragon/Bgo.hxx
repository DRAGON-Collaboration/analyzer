/// \file Bgo.hxx
/// \author G. Christian
/// \brief Defines a class for the DRAGON Bgo array
#ifndef DRAGON_BGO_HXX
#define DRAGON_BGO_HXX
#include <stdint.h>

namespace vme {
class V792;
class V1190;
}

namespace dragon {

/// The BGO array
class Bgo {
public:
	/// Number of channels in the BGO array
	static const int MAX_CHANNELS = 30; //!

	/// Number of energy-sorted channels
	static const int MAX_SORTED = 5; //!

public:
	/// Bgo variables
	class Variables {
 private:
		/** @cond */
 PRIVATE:
		/** @endcond */
		/// Maps ADC channel to BGO detector
		/*!
		 * Example: setting ch[0] = 12, means that the 0th detector
		 * in the BGO array reads its charge data from channel 12 of the qdc.
		 */
		int qdc_ch[MAX_CHANNELS];

		/// Maps TDC channel to BGO detector
		/*! Similar to qdc_ch */
		int tdc_ch[MAX_CHANNELS];

		/// \e x position
		/*! \c xpos[n] refers to the \e x position (in cm) of the <I>n<SUP>th</SUP></I> detector */
		double xpos[MAX_CHANNELS];

		/// \e y position
		/*! \c ypos[n] refers to the \e y position (in cm) of the <I>n<SUP>th</SUP></I> detector */
		double ypos[MAX_CHANNELS];

		/// \e z position
		/*! \c zpos[n] refers to the \e z position (in cm) of the <I>n<SUP>th</SUP></I> detector */
		double zpos[MAX_CHANNELS];

 public:
		/// Constructor, sets *_ch[i] to i
		Variables();

		/// \brief Set variable values from an ODB file
		/// \param [in] odb_file Path of the odb file from which you are extracting variable values
		/// \todo Needs to be implemented once ODB is set up
		void set(const char* odb_file);

		/// Allow Bgo class access to internals
		friend class dragon::Bgo;
	};

	/// Instance of Bgo::Variables for mapping digitizer ch -> bgo detector
	Variables variables;        //!
	 
private:
	/** @cond */
PRIVATE:
	/** @endcond */
	/// Raw charge signals, per detector
	int16_t q[MAX_CHANNELS];    //#

	/// Raw timing signals, per detector
	int32_t t[MAX_CHANNELS];    //#

	/// Sorted (high->low) charge signals
	int16_t qsort[MAX_SORTED];  //#

	/// Sum of all \e valid charge signals
	double qsum; //#

	/// \brief \e x position of the qsort[0] hit
	double x0; //#

	/// \brief \e y position of the qsort[0] hit
	double y0; //#

	/// \brief \e z position of the qsort[0] hit
	double z0; //#

public:
	/// Constructor, initializes data values
	Bgo();

	/// Sets all data values to vme::NONE
	void reset();

	/// Read adc & tdc data
	void read_data(const vme::V792& adc, const vme::V1190& tdc);

	/// Do higher-level parameter calculations
	void calculate();
};

} // namespace dragon


#endif
