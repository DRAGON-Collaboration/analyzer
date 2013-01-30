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
	std::ostream& fStream;

public:
	/// Sets the message formatting.
	Strm(const char* what, const char* where, std::ostream& stream):
		fStream (stream)
		{
			/*!
			 * \param what The type of message (error, warning, etc.)
			 * \param where The location of the message (function name)
			 * \param stream Instance std::ostream to use for streaming
			 */
			fStream << what << " in <" << where << ">: ";
		}

	/// Stream operator
	template <typename T> Strm& operator<< (T arg)
		{	fStream << arg; return *this; }

	/// Stream operator, for std::endl
	Strm& operator<<(StandardEndLine manip)
		{	manip(fStream);	return *this;	}

	/// Automatic printing of std::endl
	virtual ~Strm() { std::endl(fStream); }
};

/// Template specialization for std::cout

/// Specialized err::Strm class to print informational messages
struct Info: public Strm { Info(const char* where) : Strm("Info", where, std::cout) {} };

/// Specialized err::Strm class to print error messages
struct Error: public Strm { Error(const char* where) : Strm("Error", where, std::cerr) {} };

/// Specialized err::Strm class to print warning messages
struct Warning: public Strm { Warning(const char* where) : Strm("Warning", where, std::cerr) {} };

} // namespace err

} // namespace dragon

/// For printing in-place file & line messages
#define DRAGON_ERR_FILE_LINE "\nFile, line: " << __FILE__ << ", " << __LINE__ << "." << std::endl

#endif // #ifndef DRAGON_ERROR_HXX

#endif // #ifndef __MAKECINT__
