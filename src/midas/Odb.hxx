//! \file Odb.hxx
//! \author G. Christian
//! \brief Defines a "static" class with functions to read MIDAS ODB data.
#ifndef DRAGON_UTILS_ODB_HXX
#define DRAGON_UTILS_ODB_HXX

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

};

}

#endif
