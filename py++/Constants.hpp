///
/// \file Constants.hpp
/// \author G. Christian
/// \brief Copies static const member data into global constants, for use in boost::python.
/// \details boost::python apparently needs 'static const' class members to be declared
/// as extern in the source file producing the python library, even if they are literal
/// constants, that is defined as
/// \code
/// static const int SOME_VALUE = 25;
/// \endcode
/// Here we provide those extern declarations for every 'static const' class member.
///
#ifndef DRAGON_PYPP_CONSTANTS_HPP
#define DRAGON_PYPP_CONSTANTS_HPP


// dragon classes //

extern const int dragon::NaI::MAX_CHANNELS;
extern const int dragon::Bgo::MAX_CHANNELS;
extern const int dragon::DSSSD::MAX_CHANNELS;
extern const int dragon::IonChamber::MAX_CHANNELS;
extern const int dragon::MCP::MAX_CHANNELS;
extern const int dragon::MCP::NUM_DETECTORS;
extern const int dragon::SurfaceBarrier::MAX_CHANNELS;
extern const int dragon::Tail::NUM_ADC;



// vme classes //

extern const uint16_t vme::V1190::TDC_HEADER;
extern const uint16_t vme::V1190::TDC_MEASUREMENT;
extern const uint16_t vme::V1190::TDC_ERROR;
extern const uint16_t vme::V1190::TDC_TRAILER;
extern const uint16_t vme::V1190::GLOBAL_HEADER;
extern const uint16_t vme::V1190::GLOBAL_TRAILER;
extern const uint16_t vme::V1190::EXTENDED_TRIGGER_TIME;
extern const uint16_t vme::V1190::MAX_CHANNELS;
extern const uint16_t vme::V1190::MAX_HITS;

extern const uint16_t vme::V792::DATA_BITS;
extern const uint16_t vme::V792::HEADER_BITS;
extern const uint16_t vme::V792::FOOTER_BITS;
extern const uint16_t vme::V792::INVALID_BITS;
extern const uint16_t vme::V792::MAX_CHANNELS;


#endif
