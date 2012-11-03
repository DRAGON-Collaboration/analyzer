// Manual file, py++ output won't compile

#include "boost/python.hpp"

#include "__array_1.pypp.hpp"

#include "/Users/gchristian/soft/develop/dragon/analyzer/src/midas/Event.hxx"

namespace bp = boost::python;


namespace midas { namespace wrappers {

struct File : TMidasFile {

	File (): TMidasFile()
		{  }

	File (const char* filename): TMidasFile()
		{ Open(filename); }

};

struct CoincEvent : midas::CoincEvent, bp::wrapper< midas::CoincEvent > {
    CoincEvent(::midas::Event const & event1, ::midas::Event const & event2 )
    : midas::CoincEvent( boost::ref(event1), boost::ref(event2) )
      , bp::wrapper< midas::CoincEvent >(){
        // constructor
    }
    static ::midas::Event const * get_fGamma(midas::CoincEvent const & inst ){
        return inst.fGamma;
    }
    static void set_fGamma( midas::CoincEvent & inst, ::midas::Event const * new_value ){
        inst.fGamma = new_value;
    }
    static ::midas::Event const * get_fHeavyIon(midas::CoincEvent const & inst ){
        return inst.fHeavyIon;
    }
    static void set_fHeavyIon( midas::CoincEvent & inst, ::midas::Event const * new_value ){
        inst.fHeavyIon = new_value;
    }
};

// midas::Event::GetEventId)
// midas::Event::GetTriggerMask)
// midas::Event::GetSerialNumber)
// midas::Event::GetTimeStamp)
// midas::Event::GetDataSize)
// midas::Event::Print)
// midas::Event::GetBankList)
// midas::Event::FindBank)
// midas::Event::LocateBank)
// midas::Event::IsBank32)
// midas::Event::IterateBank)
// midas::Event::IterateBank32)

void Event_PrintSingle(const midas::Event& self)
{ self.PrintSingle(stdout); }

void Event_PrintCoinc(const midas::Event& self, const midas::Event& other)
{ self.PrintCoinc(other, stdout); }

bool Event_ReadFromFile(midas::Event& self, midas::wrappers::File& file)
{ return self.ReadFromFile(file); } 

} } // namespace midas::wrappers


#include "Headers.hpp"

#include "Constants.hpp"

BOOST_PYTHON_MODULE(midas)
{ 
	bp::class_< midas::wrappers::File >	("File", bp::init < >())
		.def(bp::init < const char* >())
		.def("Open",         &midas::wrappers::File::Open)
		.def("Close",        &midas::wrappers::File::Close)
		.def("GetFilename",  &midas::wrappers::File::GetFilename)
		.def("GetLastErrno", &midas::wrappers::File::GetLastErrno)
		.def("Read",         &midas::wrappers::File::Read)
		;

	typedef ::midas::Event & ( ::midas::Event::*assign_function_type )( ::midas::Event const & ) ;
	bp::class_ < midas::Event > ("Event", bp::init < >())
		.def(bp::init < const char*, char*, int >())
		.def(bp::init < const char*, const void*, const void*, int >())
		.def(bp::init < const midas::Event& >())
		.def("GetEventId",        &midas::Event::GetEventId)
		.def("GetTriggerMask",    &midas::Event::GetTriggerMask)
		.def("GetSerialNumber",   &midas::Event::GetSerialNumber)
		.def("GetTimeStamp",      &midas::Event::GetTimeStamp)
		.def("GetDataSize",       &midas::Event::GetDataSize)
		.def("Print",             &midas::Event::Print)
		.def("GetBankList",     	&midas::Event::GetBankList)
		.def("FindBank",        	&midas::Event::FindBank)
		.def("LocateBank",      	&midas::Event::LocateBank)
		.def("IsBank32",        	&midas::Event::IsBank32)
		.def("IterateBank",     	&midas::Event::IterateBank)
		.def("IterateBank32",   	&midas::Event::IterateBank32)
		.def("CopyHeader",        &midas::Event::CopyHeader)
		.def("ReadFromFile",      &midas::wrappers::Event_ReadFromFile)
		.def("TriggerTime",       &midas::Event::TriggerTime)
		.def("IsCoinc",           &midas::Event::IsCoinc)
		.def("TimeDiff",          &midas::Event::TimeDiff)
		.def("PrintSingle",       &midas::wrappers::Event_PrintSingle)
		.def("PrintCoinc",        &midas::wrappers::Event_PrintCoinc)
		.def("CompareId",         &midas::Event::CompareId::operator())
		.def("CompareSerial",     &midas::Event::CompareSerial::operator())
		.def("CompareTrigger",    &midas::Event::CompareTrigger::operator())
		.def( bp::self < bp::self )
		.def("assign"
			, assign_function_type( &::midas::Event::operator= )
			, ( bp::arg("other") )
			, bp::return_self< >() )
		;

    bp::class_< midas::wrappers::CoincEvent, boost::noncopyable >
			( "CoincEvent", bp::init< midas::Event const &,
																midas::Event const & >
				(( bp::arg("event1"), bp::arg("event2") )) )
			.add_property( "fGamma"
										 , bp::make_function(
											 (::midas::Event const * (*)( ::midas::CoincEvent const & ))
											 (&midas::wrappers::CoincEvent::get_fGamma), bp::return_internal_reference< >() )
										 , bp::make_function( (void (*)( ::midas::CoincEvent &,::midas::Event const * ))
																					(&midas::wrappers::CoincEvent::set_fGamma),
																					bp::with_custodian_and_ward_postcall< 1, 2 >() ) )
			.add_property( "fHeavyIon"
										 , bp::make_function( (::midas::Event const * (*)( ::midas::CoincEvent const & ))
																					(&midas::wrappers::CoincEvent::get_fHeavyIon), bp::return_internal_reference< >() )
										 , bp::make_function( (void (*)( ::midas::CoincEvent &,::midas::Event const * ))
																					(&midas::wrappers::CoincEvent::set_fHeavyIon),
																					bp::with_custodian_and_ward_postcall< 1, 2 >() ) );

} // BOOST_PYTHON_MODULE(midas)
