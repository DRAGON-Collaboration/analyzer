//! \file Error.hxx
//! \brief Defines C++ style error printing classes.
#ifndef __MAKECINT__
#ifndef ERROR_HXX
#define ERROR_HXX
#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>

namespace err
{

struct Strm
{
protected:
	 typedef std::basic_ostream<char, std::char_traits<char> > CoutType;
	 typedef CoutType& (*StandardEndLine)(CoutType&);
public:
	 Strm(const char* what, const char* where) {
		 std::cerr << what << " in <" << where << ">: ";
	 }
	 template <typename T> Strm& operator<< (T arg) {
		 std::cerr << arg; return *this; 
	 }
	 Strm& operator<<(StandardEndLine manip) {
		 manip(std::cerr);
		 return *this;
	 }
	 virtual ~Strm() { std::endl(std::cerr); }
};
struct Info: public Strm { Info(const char* where) : Strm("Info", where) {} };
struct Error: public Strm { Error(const char* where) : Strm("Error", where) {} };
struct Warning: public Strm { Warning(const char* where) : Strm("Warning", where) {} };
/*
struct Throw
{
protected:
	 typedef std::basic_ostream<char, std::char_traits<char> > CoutType;
	 typedef CoutType& (*StandardEndLine)(CoutType&);
	 std::stringstream fStrm;
public:
	 Throw() { }
	 Throw(const char* where) {
		 fStrm << "Error in <" << where << ">: ";
	 }
	 template <typename T> Strm& operator<< (T& arg) {
		 std::cerr << arg; return *this; 
	 }
	 Throw& operator<<(StandardEndLine manip) {
		 manip(fStrm);
		 return *this;
	 }
	 virtual ~Throw() {
		 throw(std::invalid_argument(fStrm.str().c_str()));
	 }
};
*/

}

#define ERR_FILE_LINE "\nFile, line: " << __FILE__ << ", " << __LINE__ << "." << std::endl


#endif
#endif
