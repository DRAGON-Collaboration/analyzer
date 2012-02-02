//! \file vme.hxx
//! \brief Defines classes relevant to unpacking data from VME modules.
#ifndef _VME_
#define _VME_
#include <stdint.h>
#include <string>
#include <map>
#include "TMidasEvent.h"

//! Encloses all VME related classes
namespace vme {

  //! \brief Generic VME module
  //! \details Base VME module class (etc.) \todo write description
  class Module {
  public:

    //! "Empty" code used when no data is present in a channel.
    static int16_t NONE;

    //! \brief Module identification.
    //! \details Consists of Midas bank identifier (a string) and module number.
    //! \todo Is the number necessary? I think that each module is given a unique
    //! bank name in general. If the number isn't needed, it's better to just use
    //! the name (much simpler).
    struct Id {
      //! Midas bank name
      std::string bank;
      //! Module number
      int16_t number;
      //! \brief Id comparison class
      //! \details Operator returns true if lhs is less than rhs.
      //! First checks name, then module
      struct Compare {
	inline bool operator() (const Id& lhs, const Id& rhs);
      };
      //! Constructor, set bank and number
      Id(const std::string& bank_, int16_t number_) : bank(bank_), number(number_) { };
      //! Copy constructor, set bank and number from other module
      Id(const Id& other) : bank(other.bank), number(other.number) { };
    };

    typedef std::map<Id, Module*, Id::Compare> Map;

  protected:
    //! Maps Address to a self pointer for all instances of Module and its derivitives.
    static Map all;

    //! Module address
    Id id;

    //! Total number of channels in the module.
    int16_t max_ch;

    //! Number of channels <i>in a given event</i>.
    int16_t n_ch;

    //! Array of data words
    int16_t* data;

    //! Is any channel under threshold?
    bool underflow;

    //! Is any channel an overflow?
    bool overflow;

  public:
    //! Constructor
    //! Set the module address, number of channels, reset data, add to map of all modules.
    //! Allocate memory to the data array.
    Module(uint16_t num_channels, const std::string& bank_, int16_t number_);

    //! Destructor
    //! Remove from map of all modules, deallocate data array memory.
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

    //! Return data value for a single channel
    int16_t get_data(int channel);

    //! \brief Unpack an output buffer.
    //! \details Pure virtual function, must be implemented in deived classes.
    //! \returns Error code: 0 = success, 1 = error
    virtual int unpack_buffer(void* addr) = 0;

    //! Find a module from it's address.
    //! \returns NULL if not found, pointer to the module otherwise.
    static Module* find(const std::string& bank_, int16_t number);

    //! Unpack a midas event.
    //! \details Looks for every instance of vme::Module present in the
    //! event, and if found calls unpack_buffer().
    //! \returns Error code: 0 = success, 1 = error.
    static int unpack_all(const TMidasEvent& event);

  private:
    //! Copy constructor
    Module(const Module& other) : id(other.id) { };
  };


  //! Encloses CAEN VME modules.
  namespace caen {

    //! \brief CAEN ADC modules.
    //! \details Implements virtual functions inherited from vme::Module.
    //! Implementations are valid for both CAEN v792 QDC and CAEN v785 TDC.
    //! Other CAEN ADC modules haven't been checked. If needed, they could inherit
    //! from this one since unpack_buffer() is made virtual.
    class Adc : public Module {
    public:
      //! Constructor.
      //! \details Just call the base Module constructor.
      Adc(uint16_t num_channels, const std::string& bank_, int16_t number_);
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

#endif
