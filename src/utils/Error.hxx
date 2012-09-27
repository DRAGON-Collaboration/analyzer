//! \file Error.hxx
//! \author G. Christian
//! \brief Defines C++ style error printing classes.
#ifndef __MAKECINT__
#ifndef DRAGON_ERROR_HXX
#define DRAGON_ERROR_HXX
#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>

/// Encloses message reporting classes.
namespace err {

/// Class to print messages using C++ stream operators.
/*!
 *  Via the class constructor, we also provide information about the
 *  type of message and the call location (function). Using ERR_FILE_LINE
 *  one can also report the source code location from where the message originates.
 */
class Strm {

protected:
	 typedef std::basic_ostream<char, std::char_traits<char> > CoutType;
	 typedef CoutType& (*StandardEndLine)(CoutType&);

public:
	/// Sets the message formatting.
	Strm(const char* what, const char* where)
		{
			/*!
			 * \param what The type of message (error, warning, etc.)
			 * \param where The location of the message (function name)
			 */
			std::cerr << what << " in <" << where << ">: ";
		}

	/// Stream operator
	template <typename T> Strm& operator<< (T arg)
		{	std::cerr << arg; return *this; }

	/// Stream operator, for std::endl
	Strm& operator<<(StandardEndLine manip)
		{	manip(std::cerr);	return *this;	}

	/// Automatic printing of std::endl
	virtual ~Strm() { std::endl(std::cerr); }
};

/// Specialized err::Strm class to print informational messages
struct Info: public Strm { Info(const char* where) : Strm("Info", where) {} };

/// Specialized err::Strm class to print error messages
struct Error: public Strm { Error(const char* where) : Strm("Error", where) {} };

/// Specialized err::Strm class to print warning messages
struct Warning: public Strm { Warning(const char* where) : Strm("Warning", where) {} };

} /// namespace err

#define ERR_FILE_LINE "\nFile, line: " << __FILE__ << ", " << __LINE__ << "." << std::endl


#endif // #ifndef DRAGON_ERROR_HXX

#endif // #ifndef __MAKECINT__
