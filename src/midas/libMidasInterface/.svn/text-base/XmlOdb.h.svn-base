//
// Access ODB from the XML ODB dump file
//
// Name: XmlOdb.h
// Author: K.Olchanski, 11-July-2006
//
// $Id$
//

#ifndef INCLUDE_XmlOdb_H
#define INCLUDE_XmlOdb_H

#include <stdlib.h>
#include <stdint.h>
#include "TXMLNode.h"
#include "TXMLAttr.h"
#include "TDOMParser.h"

#include "VirtualOdb.h"

/// Access to ODB saved in XML format inside midas .mid files

struct XmlOdb : VirtualOdb
{
  TDOMParser* fParser; ///< XML parser for the XML-encoded ODB data
  TXMLNode*   fOdb;    ///< Pointer to the root of the ODB tree

  /// Contructor from a string
  XmlOdb(const char*buf,int bufLength);

  /// Contructor from a file
  XmlOdb(const char* filename);

  /// Destructor
  virtual ~XmlOdb();

  /// Find node with given name anywhere inside the given ODB tree
  TXMLNode* FindNode(TXMLNode*node,const char*name);

  /// Print out the contents of the ODB tree
  void DumpTree(TXMLNode*node = NULL, int level = 0);

  /// Print out the directory structure of the ODB tree
  void DumpDirTree(TXMLNode*node = NULL, int level = 0);

  /// Return the value of the named attribute
  const char* GetAttrValue(TXMLNode*node,const char*attrName);

  /// Follow the ODB path through the XML DOM tree
  TXMLNode* FindPath(TXMLNode*node,const char* path);

  /// Same as FindPath(), but also index into an array
  TXMLNode* FindArrayPath(TXMLNode*node,const char* path,const char* type,int index);

  int      odbReadAny(   const char*name, int index, int tid,void* buf, int bufsize=0);
  int      odbReadInt(   const char*name, int index, int      defaultValue);
  uint32_t odbReadUint32(const char*name, int index, uint32_t defaultValue);
  bool     odbReadBool(  const char*name, int index, bool     defaultValue);
  double   odbReadDouble(const char*name, int index, double   defaultValue);
  float    odbReadFloat(const char*name, int index, float  defaultValue);
  const char* odbReadString(const char*name, int index, const char* defaultValue);
  int      odbReadArraySize(const char*name);
};

#endif
//end
