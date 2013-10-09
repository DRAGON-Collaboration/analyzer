///
/// \file ErrorDragon.cxx
/// \brief Implements parts of ErrorDragon.hxx
///
#include "ErrorDragon.hxx"


namespace dutils = dragon::utils;

namespace dragon { namespace utils {

/// Global DelayedMessageFactory
DelayedMessageFactory gDelayedMessageFactory;


} }

#ifndef USE_ROOT
int gErrorIgnoreLevel; // Global error ignore
#endif


namespace {
struct msgPrint {
	void operator() (std::pair<const int64_t, dutils::ADelayedMessagePrinter*>& element) { element.second->Print(); }
};
struct msgDelete {
	void operator() (std::pair<const int64_t, dutils::ADelayedMessagePrinter*>& element) { delete element.second;   }
}; 
}

void dutils::DelayedMessageFactory::Flush()
{
	std::for_each(fPrinters.begin(), fPrinters.end(), msgPrint());
}

dutils::DelayedMessageFactory::~DelayedMessageFactory()
{
	std::for_each(fPrinters.begin(), fPrinters.end(), msgDelete());
}
