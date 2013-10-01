//! \file ErrorDragon.hxx
//! \author G. Christian
//! \brief Defines C++ style error printing classes.
#ifndef DRAGON_ERROR_HXX
#define DRAGON_ERROR_HXX
#include <map>
#include <string>
#include <memory>
#include <iostream>
#include <stdexcept>
#include "AutoPtr.hxx"

#ifdef USE_ROOT
#include <TError.h> // for gErrorIgnoreLevel
#else
extern int gErrorIgnoreLevel;
#endif

namespace { std::ostream NullStream(0); }

namespace dragon { namespace utils {

/// Utility to temporarily change the error ignore level
/*! Enclose in `{ ... }` blocks to get desired bahavior */
class ChangeErrorIgnore {
public:
	/// Set gErrorIgnoreLevel to _level_
	ChangeErrorIgnore(int level): fOldLevel(gErrorIgnoreLevel)
		{ gErrorIgnoreLevel = level; }
	/// Set gErrorIgnoreLevel back to previous value
	~ChangeErrorIgnore()
		{ gErrorIgnoreLevel = fOldLevel; }
private:
	int fOldLevel;
};

/// Base stream class
class AStrm {
protected:
#ifndef __MAKECINT__
	typedef std::basic_ostream<char, std::char_traits<char> > CoutType;
	typedef CoutType& (*StandardEndLine)(CoutType&);
#endif
	std::ostream* fStream;
public:
	/// Stream operator
	template <typename T> AStrm& operator<< (T arg)
		{	if(fStream) *fStream << arg; return *this; }

#ifndef __MAKECINT__
	/// Stream operator, for std::endl
	AStrm& operator<<(StandardEndLine)
		{	return operator<< ("\n"); }
#endif
	
	/// Empty constructor
	AStrm(std::ostream* strm = 0): fStream(strm) {}
	
	/// Empry destructor
	virtual ~AStrm() {}
};

/// Class to print messages using C++ stream operators.
/*!
 *  Via the class constructor, we also provide information about the
 *  type of message and the call location (function). Using ERR_FILE_LINE
 *  one can also report the source code location from where the message originates.
 */
class Strm: public AStrm {
public:
	/// Sets the message formatting.
	Strm(const char* what, const char* where, std::ostream& stream):
		AStrm(&stream)
		{
			/*!
			 * \param what The type of message (error, warning, etc.)
			 * \param where The location of the message (function name)
			 * \param stream Instance std::ostream to use for streaming
			 */
			*fStream << what << " in <" << where << ">: ";
		}

	/// Automatic printing of std::endl
	virtual ~Strm() { std::endl(*fStream); }
};

} } // namespace utils, namespace dragon

#ifndef MIDASSYS

namespace dragon { namespace utils {

/// Specialized Strm class to print informational messages
struct Info: public Strm { Info(const char* where, bool = true) :
	Strm("Info", where, std::cout)  { if(gErrorIgnoreLevel > 1000) fStream = &NullStream; } };

/// Specialized Strm class to print error messages
struct Error: public Strm { Error(const char* where, bool = true) :
	Strm("Error", where, std::cerr) { if(gErrorIgnoreLevel > 3000) fStream = &NullStream; } };

/// Specialized Strm class to print warning messages
struct Warning: public Strm { Warning(const char* where, bool = true) :
	Strm("Warning", where, std::cerr) { if(gErrorIgnoreLevel > 2000) fStream = &NullStream; } };

} } // namespace utils, namespace dragon

#else
#include <sstream>
#ifndef __MAKECINT__
#include "midas.h"
#include "msystem.h"
#endif

namespace dragon { namespace utils {

/// Specialized Strm class to print informational messages
class Info: public AStrm {
public:
	Info(const char* where, bool printMidas = true):
		fWhere(""), fUseMidas(printMidas)
		{
			fWhere += where;
			fStream = new std::stringstream();
		}
	~Info()
		{
			if(fUseMidas) {
				if(gErrorIgnoreLevel <= 1000)
					cm_msg(MINFO, fWhere.c_str(), "%s",
								 static_cast<std::stringstream*>(fStream)->str().c_str());
			} else {
				std::ostream& rstrm = gErrorIgnoreLevel > 1000 ? NullStream : std::cout;
				rstrm << "Info in <" << fWhere << ">: " << static_cast<std::stringstream*>(fStream)->str() << "\n";
			}
			delete fStream;
		}
private:
	std::string fWhere;
	bool fUseMidas;
};

/// Specialized Strm class to print error messages
class Error: public AStrm {
public:
	Error(const char* where, bool printMidas = true):
		fWhere(""), fUseMidas(printMidas)
		{
			fWhere += where;
			fStream = new std::stringstream();
		}
	~Error()
		{
			if(fUseMidas) {
				if(gErrorIgnoreLevel <= 3000)
					cm_msg(MERROR, fWhere.c_str(), "%s",
								 static_cast<std::stringstream*>(fStream)->str().c_str());
			} else {
				std::ostream& rstrm = gErrorIgnoreLevel > 3000 ? NullStream : std::cerr;
				rstrm << "Error in <" << fWhere << ">: " << static_cast<std::stringstream*>(fStream)->str() << "\n";
			}
			delete fStream;
		}
private:
	std::string fWhere;
	bool fUseMidas;
};

/// Specialized Strm class to print warning messages
class Warning: public AStrm {
public:
	Warning(const char* where, bool printMidas = true):
		fWhere(""), fUseMidas(printMidas)
		{
			fWhere += where;
			fStream = new std::stringstream();
		}
	~Warning()
		{
			if(fUseMidas) {
				if(gErrorIgnoreLevel <= 2000)
					cm_msg(MERROR, fWhere.c_str(), "(Warning): %s",
								 static_cast<std::stringstream*>(fStream)->str().c_str());
			} else {
				std::ostream& rstrm = gErrorIgnoreLevel > 2000 ? NullStream : std::cerr;
				rstrm << "Warning in <" << fWhere << ">: " << static_cast<std::stringstream*>(fStream)->str() << "\n";
			}
		delete fStream;
		}
private:
	std::string fWhere;
	bool fUseMidas;
};


/// Abstract delayed error handler
class ADelayedMessagePrinter {
protected:
 	ADelayedMessagePrinter(const char* location, Int_t period = 0, const char* message = 0):
		fPeriod(period), fNumErrors(0), fLocation(location)
		{ if(message) fMessage = message; }
public:
	Int_t GetNumErrors() const { return fNumErrors; }
	Int_t GetPeriod()    const { return fPeriod;    }
	void  SetPeriod(Int_t period) { fPeriod = period; }
	void  ResetCounter() { fNumErrors = 0;  }	
	void  ResetMessage() { fMessage = ""; }
	std::string& GetMessage() { return fMessage; }
	void Incr()
		{
			++fNumErrors;
			if(fPeriod > 0 && fNumErrors > 0 && fNumErrors%fPeriod == 0)
				Print();
		}
	virtual void  Print() = 0;
	virtual ~ADelayedMessagePrinter() { }
protected:
	/// How often to print the message
	Int_t fPeriod;
	/// Counter for the number of errors
	Int_t fNumErrors;
	/// The error message
	std::string fMessage;
	/// Message location
	std::string fLocation;
};	

/// Class for delayed error printing
/*! Useful, for example if we have lots of repeating error
 *  messages but only want to print some fraction of them.
 */
template <class ErrType_t>
class DelayedMessagePrinter : public ADelayedMessagePrinter {
public:
 	DelayedMessagePrinter(const char* location, Int_t period = 0, const char* message = 0):
		ADelayedMessagePrinter(location, period, message) { }
	void Print()
		{
			if(!fNumErrors) return;
			ErrType_t(fLocation.c_str())  
				<< fMessage << ", number of occurances: " << fNumErrors;
			fNumErrors = 0;
		}
};

#ifndef __MAKECINT__
class DelayedMessageFactory {
public:
	DelayedMessageFactory() { }

	~DelayedMessageFactory();

	template <class T, class U>
	ADelayedMessagePrinter* Register(U base, int code, const char* location, Int_t period = 0, const char* message = 0)
{
	int64_t key = (int64_t)base + code;
	ADelayedMessagePrinter* err = new DelayedMessagePrinter<T>(location, period, message);
	if(fPrinters.insert(std::make_pair(key, err)).second) return err;
	else delete err;
	Error("DelayedMessageFactory::Register")
		<< "Couldn't register delayed message printer for location \"" << location << "\", period " << period
		<< ", message \"" << message << "\": Key " << key << " already in use";
	return 0;
}

template <class T>
ADelayedMessagePrinter* Get(T base, int code)
{
	int64_t key = (int64_t)base + code;
	std::map<int64_t, ADelayedMessagePrinter*>::iterator i = fPrinters.find(key);
	return i == fPrinters.end() ? 0 : fPrinters.operator[](key);
}

template <class T>
void Delete(T base, int code)
{
	int64_t key = (int64_t)base + code;	
	std::map<int64_t, ADelayedMessagePrinter*>::iterator i = fPrinters.find(key);
	if(i!=fPrinters.end()) { delete i->second; fPrinters.erase(i); }
	else Warning("DeleteDelayedMessagePrinter")
				 << "No delayed error handler with  key " << key << ", doing nothing.";
}

void Flush();

private:
	DelayedMessageFactory(const DelayedMessageFactory&) { }
	DelayedMessageFactory& operator= (const DelayedMessageFactory&) { return *this; }

private:
	std::map<int64_t, ADelayedMessagePrinter*> fPrinters;
};

extern DelayedMessageFactory gDelayedMessageFactory;

#endif


} }  // namespace dragon namespace utils

#endif // #ifndef MIDASSYS


#ifndef __CINT__
/// For printing in-place file & line messages
#define DRAGON_ERR_FILE_LINE "\nFile, line: " << __FILE__ << ", " << __LINE__ << "." << "\n"
#endif


#ifdef __MAKECINT__
#pragma link C++ function dragon::utils::AStrm::operator<< (const char*);
#pragma link C++ function dragon::utils::AStrm::operator<< (const std::string&);
#pragma link C++ function dragon::utils::AStrm::operator<< (const Char_t&);
#pragma link C++ function dragon::utils::AStrm::operator<< (const Short_t&);
#pragma link C++ function dragon::utils::AStrm::operator<< (const Int_t&);
#pragma link C++ function dragon::utils::AStrm::operator<< (const Long64_t&);
#pragma link C++ function dragon::utils::AStrm::operator<< (const UChar_t&);
#pragma link C++ function dragon::utils::AStrm::operator<< (const UShort_t&);
#pragma link C++ function dragon::utils::AStrm::operator<< (const UInt_t&);
#pragma link C++ function dragon::utils::AStrm::operator<< (const ULong64_t&);
#pragma link C++ function dragon::utils::AStrm::operator<< (const Float_t&);
#pragma link C++ function dragon::utils::AStrm::operator<< (const Double_t&);

#pragma link C++ class dragon::utils::DelayedMessagePrinter<dragon::utils::Error>+;
#pragma link C++ class dragon::utils::DelayedMessagePrinter<dragon::utils::Warning>+;
#pragma link C++ class dragon::utils::DelayedMessagePrinter<dragon::utils::Info>+;
#endif



#endif // #ifndef DRAGON_ERROR_HXX

