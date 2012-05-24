//! \file Odb.hxx
//! \brief Forward declaraion of functions to read data from MIDAS Online DataBase (ODB).
#ifndef DRAGON_UTILS_ODB_HXX
#define DRAGON_UTILS_ODB_HXX

/// 
int odbReadAny(const char*name,int index,int tid,void* value,int valueLength = 0);

int odbReadInt(const char*name,int index = 0,int defaultValue = 0);

uint32_t odbReadUint32(const char*name,int index = 0,uint32_t defaultValue = 0);

double odbReadDouble(const char*name,int index = 0,double defaultValue = 0);

float odbReadFloat(const char*name,int index = 0,double defaultValue = 0);

bool odbReadBool(const char*name,int index = 0,bool defaultValue = 0);

const char* odbReadString(const char*name,int index,const char* defaultValue,int stringLength);

int odbReadArraySize(const char*name);

int odbResizeArray(const char*name, int tid, int size);

int odbWriteInt(const char*name, int index, int value);

int odbWriteBool(const char*name, int index, bool value);

int odbWriteDouble(const char*name, int index, double value);

int odbWriteString(const char*name, const char* string);


#endif
