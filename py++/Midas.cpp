// Manual file, py++ output won't compile

#include <stdint.h>

#include "boost/python.hpp"

#include "__array_1.pypp.hpp"

#include "/Users/gchristian/soft/develop/dragon/analyzer/src/midas/Event.hxx"

namespace bp = boost::python;


namespace midas { namespace wrappers {

struct File : TMidasFile {

	File(): TMidasFile()
		{  }

	File(const char* filename): TMidasFile()
		{ Open(filename); }

};

struct Event : midas::Event {
	
	Event():
		midas::Event() {  }
	Event(const char* tsbank, const void* header, const void* data, int size):
		midas::Event(tsbank, header, data, size) {  }
	Event(const char* tsbank, char* buf, int size):
		midas::Event(tsbank, buf, size) {  }
	Event(const Event& other):
		midas::Event(other) {  }
	void PrintCoinc(const midas::Event& other) const
		{ midas::Event::PrintCoinc(other, stdout); }
	bool ReadFromFile(midas::wrappers::File& file)
		{ return midas::Event::ReadFromFile(file); } 
  uint16_t GetEventId() const
		{ return	midas::Event::GetEventId(); }
  uint16_t GetTriggerMask() const
		{ return midas::Event::GetTriggerMask(); }
  uint32_t GetSerialNumber() const
		{ return midas::Event::GetSerialNumber(); }
  uint32_t GetTimeStamp() const
		{ return midas::Event::GetTimeStamp(); }
  uint32_t GetDataSize() const
		{ return midas::Event::GetDataSize(); }
  void Print(const char* option ) const
		{ midas::Event::Print(option); }
  const char* GetBankList() const
		{ return midas::Event::GetBankList(); }
  int FindBank(const char* bankName, int* bankLength, int* bankType, void **bankPtr) const
		{ return midas::Event::FindBank(bankName, bankLength, bankType, bankPtr); }
  int LocateBank(const void *unused, const char* bankName, void **bankPtr) const
		{ return midas::Event::LocateBank(unused, bankName, bankPtr); }
  bool IsBank32() const
		{ return midas::Event::IsBank32(); }
  int IterateBank(Bank_t ** b, char **pdata) const
		{ return midas::Event::IterateBank(b, pdata); }
  int IterateBank32(Bank32_t ** b, char **pdata) const
		{ return midas::Event::IterateBank32(b, pdata); }

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

	typedef ::midas::wrappers::Event & ( ::midas::wrappers::Event::*assign_function_type )( ::midas::wrappers::Event const & ) ;
	bp::class_ < midas::wrappers::Event > ("Event", bp::init < >())
		.def(bp::init < const char*, char*, int >())
		.def(bp::init < const char*, const void*, const void*, int >())
		.def(bp::init < const midas::wrappers::Event& >())
		.def("GetEventId",        &midas::wrappers::Event::GetEventId)
		.def("GetTriggerMask",    &midas::wrappers::Event::GetTriggerMask)
		.def("GetSerialNumber",   &midas::wrappers::Event::GetSerialNumber)
		.def("GetTimeStamp",      &midas::wrappers::Event::GetTimeStamp)
		.def("GetDataSize",       &midas::wrappers::Event::GetDataSize)
		.def("Print",             &midas::wrappers::Event::Print)
		.def("GetBankList",     	&midas::wrappers::Event::GetBankList)
		.def("FindBank",        	&midas::wrappers::Event::FindBank)
		.def("LocateBank",      	&midas::wrappers::Event::LocateBank)
		.def("IsBank32",        	&midas::wrappers::Event::IsBank32)
		.def("IterateBank",     	&midas::wrappers::Event::IterateBank)
		.def("IterateBank32",   	&midas::wrappers::Event::IterateBank32)
		.def("CopyHeader",        &midas::wrappers::Event::CopyHeader)
		.def("ReadFromFile",      &midas::wrappers::Event::ReadFromFile)
		.def("TriggerTime",       &midas::wrappers::Event::TriggerTime)
		.def("IsCoinc",           &midas::wrappers::Event::IsCoinc)
		.def("TimeDiff",          &midas::wrappers::Event::TimeDiff)
		.def("PrintSingle",       &midas::wrappers::Event::PrintSingle)
		.def("PrintCoinc",        &midas::wrappers::Event::PrintCoinc)
		.def("CompareId",         &midas::wrappers::Event::CompareId::operator())
		.def("CompareSerial",     &midas::wrappers::Event::CompareSerial::operator())
		.def("CompareTrigger",    &midas::wrappers::Event::CompareTrigger::operator())
		.def( bp::self < bp::self )
		.def("assign"
			, assign_function_type( &::midas::wrappers::Event::operator= )
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
