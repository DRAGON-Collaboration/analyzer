//! \file ErrorDragon.hxx
//! \author G. Christian
//! \brief Defines C++ style error printing classes.
#ifndef __MAKECINT__
#ifndef DRAGON_ERROR_HXX
#define DRAGON_ERROR_HXX
#include <iostream>
#include <string>
#include <stdexcept>


namespace utils {

/// Encloses message reporting classes.
namespace err {

/// Base stream class
class AStrm {
protected:
	typedef std::basic_ostream<char, std::char_traits<char> > CoutType;
	typedef CoutType& (*StandardEndLine)(CoutType&);
	std::ostream* fStream;
public:
	/// Stream operator
	template <typename T> AStrm& operator<< (T arg)
		{	if(fStream) *fStream << arg; return *this; }

	/// Stream operator, for std::endl
	AStrm& operator<<(StandardEndLine manip)
		{	if(fStream) manip(*fStream);	return *this;	}
	
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

} } // namespace utils, namespace err

#ifndef MIDASSYS

namespace utils { namespace err {

/// Specialized err::Strm class to print informational messages
struct Info: public Strm { Info(const char* where, bool = true) : Strm("Info", where, std::cout) {} };

/// Specialized err::Strm class to print error messages
struct Error: public Strm { Error(const char* where, bool = true) : Strm("Error", where, std::cerr) {} };

/// Specialized err::Strm class to print warning messages
struct Warning: public Strm { Warning(const char* where, bool = true) : Strm("Warning", where, std::cerr) {} };

} } // namespace utils, namespace err

#else
#include <sstream>
#include "midas.h"

namespace utils { namespace err {

/// Specialized err::Strm class to print informational messages
class Info: public AStrm {
public:
	Info(const char* where, bool printMidas = true):
		fWhere("anaDragon::"), kMidas(printMidas)
		{
			fWhere += where;
			fStream = new std::stringstream();
		}
	~Info()
		{
			if(kMidas) {
				cm_msg(MINFO, fWhere.c_str(), "%s",
							 static_cast<std::stringstream*>(fStream)->str().c_str());
			} else {
				std::cout << "Info in <" << fWhere << ">: " << static_cast<std::stringstream*>(fStream)->str() << "\n";
			}
			delete fStream;
		}
private:
	std::string fWhere;
	const bool kMidas;
};

/// Specialized err::Strm class to print error messages
class Error: public AStrm {
public:
	Error(const char* where, bool printMidas = true):
		fWhere("anaDragon::"), kMidas(printMidas)
		{
			fWhere += where;
			fStream = new std::stringstream();
		}
	~Error()
		{
			if(kMidas) {
				cm_msg(MERROR, fWhere.c_str(), "%s",
							 static_cast<std::stringstream*>(fStream)->str().c_str());
			} else {
				std::cerr << "Error in <" << fWhere << ">: " << static_cast<std::stringstream*>(fStream)->str() << "\n";
			}
			delete fStream;
		}
private:
	std::string fWhere;
	const bool kMidas;
};

/// Specialized err::Strm class to print warning messages
class Warning: public AStrm {
public:
	Warning(const char* where, bool printMidas = true):
		fWhere("anaDragon::"), kMidas(printMidas)
		{
			fWhere += where;
			fStream = new std::stringstream();
		}
	~Warning()
		{
			if(kMidas) {
				cm_msg(MERROR, fWhere.c_str(), "(Warning): %s",
							 static_cast<std::stringstream*>(fStream)->str().c_str());
			} else {
				std::cerr << "Warning in <" << fWhere << ">: " << static_cast<std::stringstream*>(fStream)->str() << "\n";
			}
		delete fStream;
		}
private:
	std::string fWhere;
	const bool kMidas;
};

} } // namespace utils namespace err

#endif // #ifndef MIDASSYS

/// For printing in-place file & line messages
#define DRAGON_ERR_FILE_LINE "\nFile, line: " << __FILE__ << ", " << __LINE__ << "." << std::endl

#endif // #ifndef DRAGON_ERROR_HXX

#endif // #ifndef __MAKECINT__
