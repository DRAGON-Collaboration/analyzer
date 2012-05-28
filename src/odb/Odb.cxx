//! \file Odb.cxx
//! \brief Allows reading of data from the MIDAS Online DataBase (ODB).
//! \note Originally copied from DRAGON frontend code directory, then edited
//! \todo Figure out what database is being read/written, allow options for offline files (hDB extern variable??),
//! fix messaging scheme.
#ifdef MIDASSYS
#include <cassert>
#include <cstring>
#include <stdint.h>
#include <midas.h>
#include "odb/Odb.hxx"

// extern const char* frontend_name;
// extern HNDLE hDB;
namespace {
const char* frontend_name = "";
HNDLE hDB = 0; // n.b. HNDLE == int
}

int odb::ReadAny(const char*name,int index,int tid,void* value,int valueLength)
{
  int status;
  int size = rpc_tid_size(tid);
  HNDLE hdir = 0;
  HNDLE hkey;

  if (valueLength > 0)
		 size = valueLength;

  status = db_find_key (hDB, hdir, (char*)name, &hkey);
  if (status == SUCCESS)
	{
		status = db_get_data_index(hDB, hkey, value, &size, index, tid);
		if (status != SUCCESS)
		{
			cm_msg(MERROR, frontend_name, "Cannot read \'%s\'[%d] of type %d from odb, db_get_data_index() status %d", name, index, tid, status);
			return -1;
		}

		return 0;
	}
  else if (status == DB_NO_KEY)
	{
		cm_msg(MINFO, frontend_name, "Creating \'%s\'[%d] of type %d", name, index+1, tid);

		status = db_create_key(hDB, hdir, (char*)name, tid);
		if (status != SUCCESS)
		{
			cm_msg (MERROR, frontend_name, "Cannot create \'%s\' of type %d, db_create_key() status %d", name, tid, status);
			return -1;
		}

		status = db_find_key (hDB, hdir, (char*)name, &hkey);
		if (status != SUCCESS)
		{
			cm_msg(MERROR, frontend_name, "Cannot create \'%s\', db_find_key() status %d", name, status);
			return -1;
		}

		status = db_set_data_index(hDB, hkey, value, size, index, tid);
		if (status != SUCCESS)
		{
			cm_msg(MERROR, frontend_name, "Cannot write \'%s\'[%d] of type %d to odb, db_set_data_index() status %d", name, index, tid, status);
			return -1;
		}

		return 0;
	}
  else
	{
		cm_msg(MERROR, frontend_name, "Cannot read \'%s\'[%d] from odb, db_find_key() status %d", name, index, status);
		return -1;
	}
};

int odb::ReadInt(const char*name,int index,int defaultValue)
{
  int value = defaultValue;
  if (odb::ReadAny(name,index,TID_INT,&value) == 0)
		 return value;
  else
		 return defaultValue;
};

uint32_t odb::ReadUint32(const char*name,int index,uint32_t defaultValue)
{
  uint32_t value = defaultValue;
  if (odb::ReadAny(name,index,TID_DWORD,&value) == 0)
		 return value;
  else
		 return defaultValue;
};

double odb::ReadDouble(const char*name,int index,double defaultValue)
{
  double value = defaultValue;
  if (odb::ReadAny(name,index,TID_DOUBLE,&value) == 0)
		 return value;
  else
		 return defaultValue;
};

float odb::ReadFloat(const char*name,int index,double defaultValue)
{
  float value = defaultValue;
  if (odb::ReadAny(name,index,TID_FLOAT,&value) == 0)
		 return value;
  else
		 return defaultValue;
};

bool     odb::ReadBool(const char*name,int index,bool defaultValue)
{
  uint32_t value = defaultValue;
  if (odb::ReadAny(name,index,TID_BOOL,&value) == 0)
		 return value;
  else
		 return defaultValue;
};

const char* odb::ReadString(const char*name,int index,const char* defaultValue,int stringLength)
{
  const int maxStringLength = 256;
  char buf[maxStringLength];
  buf[0] = 0;
  if (defaultValue)
		 strlcpy(buf, defaultValue, maxStringLength);
  assert(stringLength < maxStringLength);
  if (odb::ReadAny(name, index, TID_STRING, buf, stringLength) == 0)
		 return buf;
  else
		 return defaultValue;
};

int odb::ReadArraySize(const char*name)
{
  int status;
  HNDLE hdir = 0;
  HNDLE hkey;
  KEY key;

  status = db_find_key (hDB, hdir, (char*)name, &hkey);
  if (status != SUCCESS)
		 return 0;

  status = db_get_key(hDB, hkey, &key);
  if (status != SUCCESS)
		 return 0;

  return key.num_values;
}

int odb::ResizeArray(const char*name, int tid, int size)
{
	int oldSize = odb::ReadArraySize(name);

	if (oldSize >= size)
		 return oldSize;

	int status;
	HNDLE hkey;
	HNDLE hdir = 0;

	status = db_find_key (hDB, hdir, (char*)name, &hkey);
	if (status != SUCCESS)
	{
		cm_msg(MINFO, frontend_name, "Creating \'%s\'[%d] of type %d", name, size, tid);

		status = db_create_key(hDB, hdir, (char*)name, tid);
		if (status != SUCCESS)
		{
			cm_msg (MERROR, frontend_name, "Cannot create \'%s\' of type %d, db_create_key() status %d", name, tid, status);
			return -1;
		}
         
		status = db_find_key (hDB, hdir, (char*)name, &hkey);
		if (status != SUCCESS)
		{
			cm_msg(MERROR, frontend_name, "Cannot create \'%s\', db_find_key() status %d", name, status);
			return -1;
		}
	}
   
	cm_msg(MINFO, frontend_name, "Resizing \'%s\'[%d] of type %d, old size %d", name, size, tid, oldSize);

	status = db_set_num_values(hDB, hkey, size);
	if (status != SUCCESS)
	{
		cm_msg(MERROR, frontend_name, "Cannot resize \'%s\'[%d] of type %d, db_set_num_values() status %d", name, size, tid, status);
		return -1;
	}
   
	return size;
}

int odb::WriteInt(const char*name, int index, int value)
{
  int status;
  HNDLE hdir = 0;
  HNDLE hkey;

  status = db_find_key (hDB, hdir, name, &hkey);
  if (status == SUCCESS)
	{
		int size = 4;
		status = db_set_data_index(hDB, hkey, &value, size, index, TID_INT);
		if (status == SUCCESS)
			 return 0;
	}

  return -1;
}

int odb::WriteBool(const char*name, int index, bool value)
{
  int status;
  HNDLE hdir = 0;
  HNDLE hkey;

  status = db_find_key (hDB, hdir, (char*)name, &hkey);
  if (status == SUCCESS)
	{
		int size = 4;
		status = db_set_data_index(hDB, hkey, &value, size, index, TID_BOOL);
		if (status == SUCCESS)
			 return 0;
	}

  return -1;
}

int odb::WriteDouble(const char*name, int index, double value)
{
  int status;
  HNDLE hdir = 0;
  HNDLE hkey;

  status = db_find_key (hDB, hdir, name, &hkey);
  if (status == SUCCESS)
	{
		int size = sizeof(double);
		status = db_set_data_index(hDB, hkey, &value, size, index, TID_DOUBLE);
		if (status == SUCCESS)
			 return 0;
	}

  return -1;
}

int odb::WriteString(const char*name, const char* string)
{
  int status;
  HNDLE hdir = 0;
  HNDLE hkey;

  status = db_find_key(hDB, hdir, name, &hkey);
  if (status == SUCCESS)
	{
		int size = strlen(string) + 1;

		status = db_set_data(hDB, hkey, string, size, 1, TID_STRING);
		if (status == SUCCESS)
			 return SUCCESS;
	}

  return status;
}


#else
#include <iostream>
#include "odb/Odb.hxx"

#define ERR_NO_MIDAS														\
	std::cerr << "Error: MIDASSYS not defined. file, line: " << __FILE__ <<  ", " << __LINE__ << "\n"

int odb::ReadAny(const char*name,int index,int tid,void* value,int valueLength) { ERR_NO_MIDAS; return -1; }
int odb::ReadInt(const char*name,int index,int defaultValue) { ERR_NO_MIDAS; return -1; }
uint32_t odb::ReadUint32(const char*name,int index,uint32_t defaultValue) { ERR_NO_MIDAS; return 1; }
double odb::ReadDouble(const char*name,int index,double defaultValue) { ERR_NO_MIDAS; return -1.; }
float odb::ReadFloat(const char*name,int index,double defaultValue) { ERR_NO_MIDAS; return -1.; }
bool odb::ReadBool(const char*name,int index,bool defaultValue) { ERR_NO_MIDAS; return 0; }
const char* odb::ReadString(const char*name,int index,const char* defaultValue,int stringLength) { ERR_NO_MIDAS; return 0; }
int odb::ReadArraySize(const char*name) { ERR_NO_MIDAS; return -1; }
int odb::ResizeArray(const char*name, int tid, int size) { ERR_NO_MIDAS; return -1; }
int odb::WriteInt(const char*name, int index, int value) { ERR_NO_MIDAS; return -1; }
int odb::WriteBool(const char*name, int index, bool value) { ERR_NO_MIDAS; return -1; }
int odb::WriteDouble(const char*name, int index, double value) { ERR_NO_MIDAS; return -1; }
int odb::WriteString(const char*name, const char* string) { ERR_NO_MIDAS; return -1; }

#undef ERR_NO_MIDAS

#endif
