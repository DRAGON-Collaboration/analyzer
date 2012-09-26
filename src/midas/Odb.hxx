//! \file Odb.hxx
//! \author G. Christian
//! \brief Defines a "static" class with functions to read MIDAS ODB data.
#ifndef DRAGON_UTILS_ODB_HXX
#define DRAGON_UTILS_ODB_HXX
#include <stdint.h>
#include <iostream>
#include <string>

namespace midas {

/// ODB reader class
class Odb {
public:

  /// Get database handle
	static int GetHandle();

  /// Read any value from the odb
	static int ReadAny(const char*name,int index,int tid,void* value,int valueLength = 0);

  /// Read an integer from the odb
	static int ReadInt(const char*name,int index = 0,int defaultValue = 0);

  /// Read a 32-bit integer from the odb
	static uint32_t ReadUint32(const char*name,int index = 0,uint32_t defaultValue = 0);

  /// Read a double-precision floating point from the odb
	static double ReadDouble(const char*name,int index = 0,double defaultValue = 0);

  /// Read a single-precision floating point from the odb
	static float ReadFloat(const char*name,int index = 0,double defaultValue = 0);

  /// Read a boolean from the odb
	static bool ReadBool(const char*name,int index = 0,bool defaultValue = 0);

  /// Read a C string from the odb
	const char* ReadString(const char*name,int index,const char* defaultValue,int stringLength);

  /// Get the size of an odb array
	static int ReadArraySize(const char*name);

  /// Change the size of an odb array
	static int ResizeArray(const char*name, int tid, int size);

  /// Write an integer to the odb
	static int WriteInt(const char*name, int index, int value);

  /// Write a boolean to the odb
	static int WriteBool(const char*name, int index, bool value);

  /// Write a double-precision floating point to the odb
	static int WriteDouble(const char*name, int index, double value);

  /// Write a string to the odb
	static int WriteString(const char*name, const char* string);

	template <typename T>
	static T ReadValue(const char* name, bool* success = 0)
		{
#ifdef MIDASSYS
			T value = 0;
			if(success) *success = true;
			if ( ReadAny(name, 0, GetTID<T>(), &value, sizeof(T)) == 0 ) { }
			else {
				if(success) *success = false;
				value = 0;
			}
			return value;
#else
			std::cerr << "Error: MIDASSYS not defined. file, line: " << __FILE__ <<  ", " << __LINE__ << "\n";
#endif
		}

private:

enum {
	tid_byte   = 1,
	tid_sbyte  = 2,
	tid_char   = 3,
	tid_word   = 4,
	tid_short  = 5,
	tid_dword  = 6,
	tid_int    = 7,
	tid_bool   = 8,
	tid_float  = 9,
	tid_double = 10,
	tid_string = 12
};

	template <typename T> static int GetTID();

};

template<> inline int Odb::GetTID<char>()           { return tid_sbyte;  }
template<> inline int Odb::GetTID<unsigned char>()  { return tid_char;   }
template<> inline int Odb::GetTID<uint16_t>()       { return tid_word;   }
template<> inline int Odb::GetTID<int16_t>()        { return tid_short;  }
template<> inline int Odb::GetTID<uint32_t>()       { return tid_dword;  }
template<> inline int Odb::GetTID<int32_t>()        { return tid_int;    }
template<> inline int Odb::GetTID<bool>()           { return tid_bool;   }
template<> inline int Odb::GetTID<float>()          { return tid_float;  }
template<> inline int Odb::GetTID<double>()         { return tid_double; }
template<> inline int Odb::GetTID<std::string>()    { return tid_string; }

#ifndef __MAKECINT__
#ifdef MIDASSYS
template<> inline std::string Odb::ReadValue<std::string>(const char* name, bool* success)
{
	char buf[256];
	std::string value;
	if(success) *success = true;
	if ( ReadAny(name, 0, GetTID<std::string>(), buf, sizeof(buf)) == 0 ) {
		value = buf;
	}
	else {
		if(success) *success = false;
	}
	return value;
}
#endif
#endif

} // namespace midas

#endif
