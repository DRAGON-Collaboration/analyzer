//! \file vme.hxx
//! \brief Defines classes relevant to unpacking data from VME modules.
#ifndef _VME_
#define _VME_
#include <stdint.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "TMidasEvent.h"

//! Encloses all VME related classes
namespace vme {

//! \brief Generic VME module
class Module {
public:

	 //! "Empty" code used when no data is present in a channel.
#ifdef VME_NO_DATA
	 static const int16_t NONE = VME_NO_DATA;
#else
	 static const int16_t NONE = -1;
#endif

	 //! Map of bank name to Module*
	 typedef std::map<std::string, Module*> Map;

protected:
	 //! MIDAS bank name.
	 std::string bank;

	 //! Total number of channels in the module.
	 int16_t max_ch;

	 //! Number of channels <i>in a given event</i>.
	 int16_t n_ch;

	 //! Array of data words
	 int16_t* data;

	 //! Event counter
	 int32_t count;

	 //! Is any channel under threshold?
	 bool underflow;

	 //! Is any channel an overflow?
	 bool overflow;

public:
	 //! Was memory allocated in the constructor?
	 const bool self_allocated;

	 //! Constructor
	 //! Set the module address, number of channels, reset data, add to map of all modules.
	 //! Allocate memory to the data array.
	 Module(const char* bank_name, uint16_t num_channels);

	 //! Constructor
	 //! Set the module address, number of channels, reset data, add to map of all modules.
	 //! Use externally allocated data arrray.
	 Module(const char* bank_name, uint16_t num_channels, int16_t* data_);

	 //! Destructor
	 //! Remove from map of all modules, deallocate data array memory (if allocated in the constructor).
	 virtual ~Module();

	 //! Reset averything to it's default value.
	 //! False for under/overflow, vme::NONE for data values
	 void reset();

	 //! Check for an under threshold event.
	 bool is_under() { return underflow; }

	 //! Check for an overflow event.
	 bool is_over()  { return overflow; }

	 //! Return number of channels present in a given event.
	 int16_t n_channels() { return n_ch; }

	 //! Return event counter
	 int32_t event_count() { return count; }

	 //! Return data value for a single channel
	 int16_t get_data(int channel);

	 //! Copy data into another array
	 void copy_data(int16_t* destination);

	 //! \brief Unpack an output buffer.
	 //! \details Pure virtual function, must be implemented in deived classes.
	 //! \returns Error code: 0 = success, 1 = error
	 virtual int unpack_buffer(void* addr) = 0;

	 int unpack(const TMidasEvent& event);
    
private:
	 //! Copy constructor
	 Module(const Module& other) : bank(other.bank), self_allocated(other.self_allocated) { };
};


//! Encloses CAEN VME modules.
namespace caen {

//! \brief CAEN ADC modules.
//! \details Implements virtual functions inherited from vme::Module.
//! Implementations are valid for both CAEN v792 QDC and CAEN v785 TDC.
//! Other CAEN ADC modules haven't been checked. If needed, they could inherit
//! from this one since unpack_buffer() is still virtual.
class Adc : public Module {
public:
	 //! Constructor.
	 //! \details Just call the base Module constructor.
	 Adc(const char* bank_name, uint16_t num_channels);
	 //! Constructor.
	 //! \details Just call the base Module constructor.
	 Adc(const char* bank_name, uint16_t num_channels, int16_t* data_);
	 //! Destructor.
	 //! Leave empty, the base class takes care of everything.
	 virtual ~Adc();

	 //! \brief Unpack an output buffer.
	 //! \details Output buffers are 32-bits and can be one of three types:
	 //! Header
	 //! \code
	 //! 31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
	 //! |     GEO     | 0  1  0 |         CRATE       |  0  0  |     COUNT    |                       |
	 //! \endcode
	 //! Data
	 //! \code
	 //! 31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
	 //! |     GEO     | 0  0  0 |        |   CHANNEL   |     |UN|OV|              DATA                |
	 //! \endcode
	 //! or Footer
	 //! \code
	 //! 31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
	 //! |     GEO     | 1  0  0 |                         EVENT COUNTER                               |
	 //! \endcode
	 virtual int unpack_buffer(void* addr);
};
}
}


struct BgoVariables {
	 double slope[32];
	 double offset [32];
	 BgoVariables() {
		 for(int i=0; i<32;++i) {
			 slope[i]=1;offset[i]=0;
		 }
	 }
};

template <int N> struct Test { int i; };
template <int N> void do_something(Test<N>& test) { test.i = 12; }

struct Bgo {
	 Test<3> test;
	 vme::caen::Adc adc; //!
	 int16_t qraw[32];   //#
	 double  qcal[32];   //#
	 int32_t evt_count;  //#
	 BgoVariables v; //!
	 Bgo(): adc("VADC", 32, qraw) {
		 evt_count = 0;
		 reset();
	 };
	 void reset() {
		 for(int i=0; i< 32; ++i) {
			 qraw[i]=-1;qcal[i]=-1;
		 }
	 }
	 void unpack(TMidasEvent& event) {
		 reset();
		 adc.unpack(event);
		 evt_count++;
		 for(int i=0; i< 32; ++i) {
			 qcal[i] = qraw[i]*v.slope[i]+v.offset[i];
		 }
	 }
	 ~Bgo() { }
};

#endif
