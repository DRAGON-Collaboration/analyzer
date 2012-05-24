//! \file Odb.hxx
//! \brief Forward declaraion of functions to read data from MIDAS Online DataBase (ODB).
#ifndef DRAGON_UTILS_ODB_HXX
#define DRAGON_UTILS_ODB_HXX

/// Encloses ODB related functions
namespace odb {

/// Read any value from the odb
int ReadAny(const char*name,int index,int tid,void* value,int valueLength = 0);

/// Read an integer from the odb
int ReadInt(const char*name,int index = 0,int defaultValue = 0);

/// Read a 32-bit integer from the odb
uint32_t ReadUint32(const char*name,int index = 0,uint32_t defaultValue = 0);

/// Read a double-precision floating point from the odb
double ReadDouble(const char*name,int index = 0,double defaultValue = 0);

/// Read a single-precision floating point from the odb
float ReadFloat(const char*name,int index = 0,double defaultValue = 0);

/// Read a boolean from the odb
bool ReadBool(const char*name,int index = 0,bool defaultValue = 0);

/// Read a C string from the odb
const char* ReadString(const char*name,int index,const char* defaultValue,int stringLength);

/// Get the size of an odb array
int ReadArraySize(const char*name);

/// Change the size of an odb array
int ResizeArray(const char*name, int tid, int size);

/// Write an integer to the odb
int WriteInt(const char*name, int index, int value);

/// Write a boolean to the odb
int WriteBool(const char*name, int index, bool value);

/// Write a double-precision floating point to the odb
int WriteDouble(const char*name, int index, double value);

/// Write a string to the odb
int WriteString(const char*name, const char* string);

} // namespace odb

#endif
