//! \file Odb.hxx
//! \author G. Christian
//! \brief Defines a "static" class with functions to read MIDAS ODB data.
#ifndef DRAGON_UTILS_ODB_HXX
#define DRAGON_UTILS_ODB_HXX
#include <stdint.h>
#include <iostream>
#include <string>
#include "utils/Error.hxx"

#ifdef MIDASSYS
#ifdef __MAKECINT__
typedef int32_t HNDLE;
#else
#include <midas.h>
#endif
#else
typedef int32_t HNDLE;
#endif

namespace midas {

/// ODB reader class
class Odb {
public:

  /// Get database handle
	static HNDLE GetHandle();

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
	static const char* ReadString(const char*name,int index,const char* defaultValue,int stringLength);

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
	static bool ReadValue(const char* path, T& value)
		{
#ifdef MIDASSYS
			if ( ReadArray(path, &value, sizeof(value)) != 0 )
				return true;
			else
				return false;
#else
			err::Error("midas::Odb::ReadValue") << "MIDASSYS not defined." << ERR_FILE_LINE;
			return false;
#endif
		}

	/// Get the values of an array of key elements
	template <typename T>
	static int ReadArray(const char* path, T* array, int length)
		{
#ifdef MIDASSYS
			int status;
			int size = length * sizeof(T);
			HNDLE hdir = 0, hkey;
			HNDLE hdb = GetHandle();
			if (hdb == 0) return false;

			status = db_find_key (hdb, hdir, path, &hkey);
			if (status == DB_NO_KEY) {
				err::Error("midas::Odb::ReadArray") <<
					"Couldn't get array key for path \"" << path << "\". status = " << status
																							 << ERR_FILE_LINE;
				return 0;
			}

			if (status == SUCCESS) {
				status = db_get_data(hdb, hkey, array, &size, GetTID<T>());
				if (status == SUCCESS)
					return size / sizeof (T);
			}

			err::Error("midas::Odb::ReadArray") << ERR_FILE_LINE << "Cannot read \"" <<
				path << "\" from odb, status = " << status << "\n";
			return 0;
#else
			err::Error("midas::Odb::ReadArray") << ERR_FILE_LINE << "MIDASSYS not defined.\n";
			return 0;
#endif
		}

private:

	/// MIDAS TID codes
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

	/// Returns the TID code for type \e T
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
template<> inline bool Odb::ReadValue<std::string>(const char* path, std::string& value)
{
#ifdef MIDASSYS
	int status;
	char buf[256];
	int size = sizeof(buf);
	HNDLE hdir = 0, hkey;
	HNDLE hdb = GetHandle();
	if (hdb == 0) return false;

	status = db_find_key (hdb, hdir, path, &hkey);
	if (status == DB_NO_KEY) {
		err::Error("midas::Odb::ReadValue<std::string>") <<
			"Couldn't get array key for path \"" << path << "\". status = " << status
																					 << ERR_FILE_LINE;
		return 0;
	}

	if (status == SUCCESS) {
		status = db_get_data(hdb, hkey, buf, &size, tid_string);
		if (status == SUCCESS) {
			value = buf;
			return true;
		}
	}

	err::Error("midas::Odb::ReadValue<std::string>") << "Cannot read \"" <<
		path << "\" from odb, status = " << status << ERR_FILE_LINE;
	return false;

#else

	err::Error("midas::Odb::ReadValue<std::string>") << "MIDASSYS not defined." << ERR_FILE_LINE;
	return 0;

#endif
}

template<> inline int Odb::ReadArray<std::string>(const char* path, std::string* array, int length)
{
#ifdef MIDASSYS
	int status;
	char buf[256];

	int size = sizeof(buf);
	HNDLE hdir = 0, hkey;
	HNDLE hdb = GetHandle();
	if (hdb == 0) return false;

	status = db_find_key (hdb, hdir, path, &hkey);
	if (status == DB_NO_KEY) {
		err::Error("midas::Odb::ReadArray<std::string>") <<
			"Couldn't get array key for path \"" << path << "\". status = " << status
																					 << ERR_FILE_LINE;
		return 0;
	}

	if (status == SUCCESS) {
		for(int i=0; i< length; ++i) {
			status = db_get_data_index(hdb, hkey, buf, &size, i, tid_string);
			if (status != SUCCESS) break;
			array[i] = buf;
		}
		
		if (status == SUCCESS) return true;
	}

	err::Error("midas::Odb::ReadArray<std::string>")
		<< "Cannot read \"" << path << "\" from odb, status = " << status
		<< ERR_FILE_LINE;
	return false;
#else
	err::Error("midas::Odb::ReadArray<std::string>") << "MIDASSYS not defined." << ERR_FILE_LINE;
	return 0;
#endif
}


#endif
#endif

} // namespace midas





#endif
