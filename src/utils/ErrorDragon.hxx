//! \file ErrorDragon.hxx
//! \author G. Christian
//! \brief Defines C++ style error printing classes.
#ifndef DRAGON_ERROR_HXX
#define DRAGON_ERROR_HXX
#include <stdint.h>
#include <map>
#include <memory>
#include <string>
#include <sstream>
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
	/// Empty destructor
	virtual ~AStrm() {}

private:
	AStrm(const AStrm&) { }
	AStrm& operator= (const AStrm&) { return *this; }
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
	Strm(const char* what, const char* where, const char* file, int line, std::ostream& stream, bool ignore):
		AStrm(&stream), fIgnore(ignore)
		{
			/*!
			 * \param what The type of message (error, warning, etc.)
			 * \param where The location of the message (function name)
			 * \param stream Instance std::ostream to use for streaming
			 */
			std::string strfile = file;
			if(!strfile.empty()) {
				if(strfile.rfind("/") < strfile.size()) {
					strfile = strfile.substr(strfile.rfind("/")+1);
				}
			}
			if(!fIgnore) {
				if(line >= 0)
					*fStream << what << " in <" << strfile << ", " << line << ", " << where << ">: ";
				else
					*fStream << what << " in <" << where << ">: ";
			}
		}

	/// Automatic printing of std::endl
	virtual ~Strm() { if(!fIgnore) std::endl(*fStream); }
private:
	bool fIgnore;
};

} } // namespace utils, namespace dragon

#ifndef MIDASSYS

namespace dragon { namespace utils {

/// Specialized Strm class to print informational messages
struct Info: public Strm { Info(const char* where, const char* file = "", int line = -1, bool = true) :
	Strm("Info", where, file, line, std::cout, gErrorIgnoreLevel > 1000)
		{ if(gErrorIgnoreLevel > 1000) fStream = &NullStream; } };

/// Specialized Strm class to print error messages
struct Error: public Strm { Error(const char* where, const char* file = "", int line = -1, bool = true) :
	Strm("Error", where, file, line, std::cerr, gErrorIgnoreLevel > 3000)
		{ if(gErrorIgnoreLevel > 3000) fStream = &NullStream; } };

/// Specialized Strm class to print warning messages
struct Warning: public Strm { Warning(const char* where, const char* file = "", int line = -1, bool = true) :
	Strm("Warning", where, file, line, std::cerr, gErrorIgnoreLevel > 2000)
		{ if(gErrorIgnoreLevel > 2000) fStream = &NullStream; } };

} } // namespace utils, namespace dragon

#else
#ifndef __MAKECINT__
#include "midas.h"
#include "msystem.h"
#endif


namespace dragon { namespace utils {

/// Specialized Strm class to print informational messages
class Info: public AStrm {
public:
	Info(const char* where, const char* file = "", int line = -1, bool printMidas = true):
		fWhere(""), fFile(file), fLine(line), fUseMidas(printMidas)
		{
			fWhere += where;
			fStream = new std::stringstream();
			if(!fFile.empty()) {
				if(fFile.rfind("/") < fFile.size()) {
					fFile = fFile.substr(fFile.rfind("/")+1);
				}
			}
		}
	~Info()
		{
			int line = fLine >= 0 ? fLine : __LINE__;
			const char* file = fLine >= 0 ? fFile.c_str() : __FILE__;
			if(fUseMidas) {
				if(gErrorIgnoreLevel <= 1000)
					cm_msg(MT_INFO, file, line, fWhere.c_str(), "%s",
								 static_cast<std::stringstream*>(fStream)->str().c_str());
			} else {
				std::ostream& rstrm = gErrorIgnoreLevel > 1000 ? NullStream : std::cout;
				rstrm << "Info in <";
				if(fLine >= 0) rstrm << file << ", " << line << ", ";
				rstrm << fWhere << ">: " << static_cast<std::stringstream*>(fStream)->str() << "\n";
			}
			delete fStream;
		}
private:
	std::string fWhere;
	std::string fFile;
	int fLine;
	bool fUseMidas;
};

/// Specialized Strm class to print error messages
class Error: public AStrm {
public:
	Error(const char* where, const char* file = "", int line = -1, bool printMidas = true):
		fWhere(""), fFile(file), fLine(line), fUseMidas(printMidas)
		{
			fWhere += where;
			fStream = new std::stringstream();
			if(!fFile.empty()) {
				if(fFile.rfind("/") < fFile.size()) {
					fFile = fFile.substr(fFile.rfind("/")+1);
				}
			}
		}
	~Error()
		{
			int line = fLine >= 0 ? fLine : __LINE__;
			const char* file = fLine >= 0 ? fFile.c_str() : __FILE__;
			if(fUseMidas) {
				if(gErrorIgnoreLevel <= 3000)
					cm_msg(MT_ERROR, file, line, fWhere.c_str(), "%s",
								 static_cast<std::stringstream*>(fStream)->str().c_str());
			} else {
				std::ostream& rstrm = gErrorIgnoreLevel > 3000 ? NullStream : std::cerr;
				rstrm << "Error in <";
				if(fLine >= 0) rstrm << file << ", " << line << ", ";
				rstrm << fWhere << ">: " << static_cast<std::stringstream*>(fStream)->str() << "\n";
			}
			delete fStream;
		}
private:
	std::string fWhere;
	std::string fFile;
	int fLine;
	bool fUseMidas;
};

/// Specialized Strm class to print warning messages
class Warning: public AStrm {
public:
	Warning(const char* where, const char* file = "", int line = -1, bool printMidas = true):
		fWhere(""), fFile(file), fLine(line), fUseMidas(printMidas)
		{
			fWhere += where;
			fStream = new std::stringstream();
			if(!fFile.empty()) {
				if(fFile.rfind("/") < fFile.size()) {
					fFile = fFile.substr(fFile.rfind("/")+1);
				}
			}
		}
	~Warning()
		{
			int line = fLine >= 0 ? fLine : __LINE__;
			const char* file = fLine >= 0 ? fFile.c_str() : __FILE__;
			if(fUseMidas) {
				if(gErrorIgnoreLevel <= 2000)
					cm_msg(MT_ERROR, file, line, fWhere.c_str(), "(Warning) %s",
								 static_cast<std::stringstream*>(fStream)->str().c_str());
			} else {
				std::ostream& rstrm = gErrorIgnoreLevel > 2000 ? NullStream : std::cerr;
				rstrm << "Warning in <";
				if(fLine >= 0) rstrm << file << ", " << line << ", ";
				rstrm << fWhere << ">: " << static_cast<std::stringstream*>(fStream)->str() << "\n";
			}
			delete fStream;
		}
private:
	std::string fWhere;
	std::string fFile;
	int fLine;
	bool fUseMidas;
};

} }

#endif // #ifndef MIDASSYS


namespace dragon { namespace utils {

/// Abstract delayed error handler
/*!
 * Intent is for error messages that happen in rapid succession. By sending them
 * to this class, the printing can be delayed to happen only every `n` times, or
 * until explicitly requested by another function.
 */
class ADelayedMessagePrinter {
protected:
	/// See concrete template class constructor
 	ADelayedMessagePrinter(const char* location, Int_t period = 0,
												 const char* file = "", int line = -1,
												 const char* message = 0):
		fPeriod(period), fNumErrors(0), fLocation(location), fFile(file), fLine(line)
		{ if(message) fMessage = message; }
public:
	/// Returns number of stored error messages
	Int_t GetNumErrors() const { return fNumErrors; }
	/// Returns message period, i.e. how many messages in between prints
	Int_t GetPeriod()    const { return fPeriod;    }
	/// Set message period
	void  SetPeriod(Int_t period) { fPeriod = period; }
	/// Reset message counter to zero
	void  ResetCounter() { fNumErrors = 0;  }	
	/// Reset the message to null string
	void  ResetMessage() { fMessage = ""; }
	/// Return reference to message string, can use this to change the message
	std::string& GetMessage() { return fMessage; }
	/// Increment the number of errors
	void Incr()
		{
			++fNumErrors;
			if(fPeriod > 0 && fNumErrors > 0 && fNumErrors%fPeriod == 0)
				Print();
		}
	/// Print the error message
	virtual void  Print() = 0;
	/// Empty
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
	/// Message file location 
	std::string fFile;
	/// Message line location
	int fLine;
};	

/// Class for delayed error printing
/*! Useful, for example if we have lots of repeating error
 *  messages but only want to print some fraction of them.
 */
template <class ErrType_t>
class DelayedMessagePrinter : public ADelayedMessagePrinter {
public:
	/// Construct message printer instance
	/*! \param location Message location, i.e. the function from where it's originating
	 *  \param file File from where the message originates
	 *  \param line Line from where the message originates
	 *  \param period How often to print the message (0 = only when explicitly requested)
	 *  \param message The message, if NULL can be set later using GetMessage()
	 */ 
 	DelayedMessagePrinter(const char* location, Int_t period = 0,
												const char* file = "", int line = -1,
												const char* message = 0):
		ADelayedMessagePrinter(location, period, file, line, message) { }
	/// Print the error message and reset counter to zero
	void Print()
		{
			if(!fNumErrors) return;
			ErrType_t(fLocation.c_str(), fFile.c_str(), fLine)
				<< fMessage << ", number of occurances: " << fNumErrors;
			fNumErrors = 0;
		}
};

#ifndef __MAKECINT__

/// Factory class to create and store DelayedMessagePrinters
class DelayedMessageFactory {
public:
	/// Empty
	DelayedMessageFactory() { }
	/// Free memory allocated to all registered printers
	~DelayedMessageFactory();
	/// Register a new delayed message printer
	template <class T, class U>
	ADelayedMessagePrinter* Register(U base, int code, const char* location, Int_t period,
																	 const char* file = "", int line = -1, const char* message = 0)
		{
			///
			/// \param base Base part of the key, often the address of the class associated w/ the message
			/// \param code Offset part of the key, the final access key is `base + code`
			/// \param location Where the message is originating from
			/// \param file File from where the message originates
			/// \param line Line from where the message originates
			/// \param period The message period
			/// \param message The message to print
			/// \returns Pointer to the newly created message printer, or NULL if one already exists with key `base + code`

			int64_t key = (int64_t)base + code;
			ADelayedMessagePrinter* err = new DelayedMessagePrinter<T>(location, period, file, line, message);
			if(fPrinters.insert(std::make_pair(key, err)).second) return err;
			else delete err;
			Error("DelayedMessageFactory::Register")
				<< "Couldn't register delayed message printer for location \"" << location << "\", period " << period
				<< ", message \"" << message << "\": Key " << key << " already in use";
			return 0;
		}

	/// Retreive a registered message printer
	template <class T>
	ADelayedMessagePrinter* Get(T base, int code)
		{
			///
			/// \param base Base part of the key
			/// \param code Offset part of the key
			/// \returns Pointer to the message printer registered w/ key `base + code` or NULL is it doesn't exist

			int64_t key = (int64_t)base + code;
			std::map<int64_t, ADelayedMessagePrinter*>::iterator i = fPrinters.find(key);
			return i == fPrinters.end() ? 0 : fPrinters.operator[](key);
		}

	/// Remove a registered message printer
	template <class T>
	void Delete(T base, int code)
		{
			///
			/// Frees memory associated to the printer and removes it from the collection
			/// 
			/// \param base Base part of the key
			/// \param code Offset part of the key

			int64_t key = (int64_t)base + code;	
			std::map<int64_t, ADelayedMessagePrinter*>::iterator i = fPrinters.find(key);
			if(i!=fPrinters.end()) { delete i->second; fPrinters.erase(i); }
			else Warning("DeleteDelayedMessagePrinter")
						 << "No delayed error handler with  key " << key << ", doing nothing.";
		}
	
	/// Print messages for all registered printers
	void Flush();

private:
	DelayedMessageFactory(const DelayedMessageFactory&) { }
	DelayedMessageFactory& operator= (const DelayedMessageFactory&) { return *this; }

private:
	std::map<int64_t, ADelayedMessagePrinter*> fPrinters;
};

extern DelayedMessageFactory gDelayedMessageFactory;

#endif // __MAKECINT__

} }  // namespace dragon namespace utils



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

