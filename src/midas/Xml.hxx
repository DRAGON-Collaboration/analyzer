//! \file Xml.hxx
//! \author G. Christian
//! \brief Defines a class to parse MIDAS XML files from the ODB.
#ifndef MIDAS_XML_HXX
#define MIDAS_XML_HXX
#include "utils/IntTypes.h"
#include <cstdlib>
#include <cstring>
#include <vector>
#include <sstream>
#include <typeinfo>
#ifndef __MAKECINT__
#include <iostream>
#endif
#include "mxml.h"
#include "strlcpy.h"

#ifdef USE_ROOT
#include <TObject.h>
#endif

#include "utils/ErrorDragon.hxx"


namespace midas {

/// Class to parse MIDAS ODB XML files.
/*!
 * \attention If you want to be able to cleanly read from either the
 * ODB or an offline file, it is suggested to use the class midas::Database
 * instead of this one, as that class can read from either. In midas::Database
 * All of the "work" to read from an XML (or .mid) file is handled by the
 * template functions of this class.
 */
class Xml
#ifdef USE_ROOT
	: public TObject
#endif
{
public:
	/// Pointer to an XML node.
	typedef PMXML_NODE Node;

public:// private:
	/// Pointer to the entire XML tree contained within a file
	Node fTree; //!
	/// Pointer to the ODB portion of fTree
	Node fOdb;  //!
	/// Flag specifying if the file was invalid
	bool fIsZombie;
	/// Length of fBuffer
	uint32_t fLength;
	/// Buffer containing all of the XML data
	char* fBuffer; //[fLength]

public:
	/// \brief Default constructor for ROOTCINT
	Xml();

	/// \brief Read data from an XML file
	/// \details Parses a file containing XML data to fill fTree and fOdb.
	/// Can handle either a dedicated \c .xml file or a \c .mid (or any other) file containing
	/// the XML data as a subset.
	/// \note Memory is allocated only to fTree, fOdb and any other subnodes used later
	/// simply refer to memory locations relative to fTree.
	Xml(const char* filename);

	/// \brief Read data from a buffer w/ XML data
	Xml(char* buf, int length);

	/// Frees resources allocated to fTree
	~Xml();

	/// Returns fIsZombie
	bool IsZombie() { return fIsZombie; }

	/// Dump buffer to an output stream
	void Dump(std::ostream& strm) const;

	/// TObject has a virtual function Dump() as well, so implement it here
	void Dump() const { Dump(std::cout); }

	/// \brief Find the node location of a specific key element within the xml file
	/// \param [in] path String specifying the "directory" path of the element, e.g.
	/// "Equipment/gTrigger/Variables/Pedestals".
	Node FindKey(const char* path, bool silent = false);

	/// \brief Find the node location of a specific keyarray element within the xml file
	/// \param [in] path String specifying the "directory" path of the element, e.g.
	/// "Equipment/gTrigger/Variables/Pedestals".
	Node FindKeyArray(const char* path, bool silent = false);

	/// Get the value of key element
	template <typename T> T GetValue(const char* path, bool* success = 0)
		{
			/*!
			 * \tparam The type of the key element (e.g. int, float, string, ...)
			 * \param [in] path String specifying the "directory" path of the element, e.g.
			 * "Equipment/gTrigger/Variables/Pedestals"
			 * \param [out] success Sets the value to \c true if \e path was valid, \c false otherwise.
			 *  Passing a NULL value to the function disables the error reporting
			 * \returns The value of the element specified by \e path
			 */
			T out;
			GetValue(path, out, success);
			return out;
		}

  /// Get the value of key element
	template <typename T> void GetValue(const char* path, T& value, bool* success = 0)
		{
			/*!
			 * \tparam The type of the key element (e.g. int, float, string, ...)
			 * \param [in] path String specifying the "directory" path of the element, e.g.
			 * "Equipment/gTrigger/Variables/Pedestals"
			 * \param [out] value Set to the value of the element specified by \e path
			 * \param [in] success Sets the value to \c true if \e path was valid, \c false otherwise.
			 *  Passing a NULL value to the function disables the error reporting
			 */
			if(success) *success = true;
			Node node = FindKey(path);
			if(!node) {
				if(success) *success = false;
				return;
			}
			value = ConvertNode<T>(node);
		}

public:
	///  Get the values of an array of key elements
	template <typename T> std::vector<T> GetArray(const char* path, bool* success = 0)
		{
			/*!
			 * \tparam The type of the key element (e.g. int, float, string, ...)
			 * \param [out] path String specifying the "directory" path of the element, e.g.
			 * "Equipment/gTrigger/Variables/Pedestals"
			 * \param [out] success Sets the value to \c true if \e path was valid, \c false otherwise.
			 *  Passing a NULL value to the function disables the error reporting
			 * \returns A vector of all array values
			 */
			std::vector<T> out;
			GetArray<T>(path, out, success);
			return out;
		}

	/// Get the values of an array of key elements
	template <typename T> void GetArray(const char* path, std::vector<T>& array, bool* success = 0)
		{
			/*!
			 * \tparam The type of the key element (e.g. int, float, string, ...)
			 * \param [in] path String specifying the "directory" path of the element, e.g.
			 * "Equipment/gTrigger/Variables/Pedestals"
			 * \param [out] array Set to a vector of all array values
			 * \param [out] success Sets the value to \c true if \e path was valid, \c false otherwise.
			 *  Passing a NULL value to the function disables the error reporting
			 */
			if(success) *success = true;
			Node node = FindKeyArray(path);
			if(!node) {
				if(success) *success = false;
				return;
			}
			array.clear();
			char* pAttribute = mxml_get_attribute(node, "num_values");
			if(!pAttribute) {
				dragon::utils::Error("midas::Xml::GetArray", __FILE__, __LINE__)
					<< "\"num_values\" attribute not found for array: " << path;
				if(success) *success = false;
				return;
			}
			int size = atoi(pAttribute);
			for(int i=0; i< size; ++i) {
				std::stringstream valPath;
				valPath << "/value[" << i+1 << "]";
				Node valNode = mxml_find_node(node, valPath.str().c_str());
				if(!valNode) {
					dragon::utils::Error("midas::Xml::GetArray", __FILE__, __LINE__)
						<< "Unable to find value node for array index " << i;
					continue;
				}

				T value;
				if(typeid(T) != typeid(bool)) {
					std::stringstream val;
					val << valNode->value;
					val >> value;
				}
				else {
					value = (!strcmp("y", valNode->value)) ? true : false;
				}
				array.push_back(value);

			}
		}

	/// Read the length of an array
	int GetArrayLength(const char* path)
		{
			/*!
			 * \param [in] path String specifying the "directory" path of the element, e.g.
			 * "Equipment/gTrigger/Variables/Pedestals"
			 * \returns Length of the array if valid, -1 if error.
			 */
			Node node = FindKeyArray(path);
			if(!node) return -1;
			char* pAttribute = mxml_get_attribute(node, "num_values");
			if(!pAttribute) {
				dragon::utils::Error("midas::Xml::GetArrayLength", __FILE__, __LINE__)
					<< "\"num_values\" attribute not found for array: " << path;
				return -1;
			}
			int size = atoi(pAttribute);
			return size;
		}

	/// Get the values of an array of key elements
	template <typename T> void GetArray(const char* path, int length, T* array, bool* success = 0)
		{
			/*!
			 * \tparam The type of the key element (e.g. int, float, string, ...)
			 * \param [in] path String specifying the "directory" path of the element, e.g.
			 * "Equipment/gTrigger/Variables/Pedestals"
			 * \param [in] length Length of the array to fill
			 * \param [out] array Array to fill with values from the ODB
			 * \param [out] success Sets the value to \c true if \e path was valid, \c false otherwise.
			 *  Passing a NULL value to the function disables the error reporting
			 */
			if(success) *success = true;
			Node node = FindKeyArray(path);
			if(!node) {
				if(success) *success = false;
				return;
			}
			char* pAttribute = mxml_get_attribute(node, "num_values");
			if(!pAttribute) {
				dragon::utils::Error("midas::Xml::GetArray", __FILE__, __LINE__)
					<< "\"num_values\" attribute not found for array: " << path;
				if(success) *success = false;
				return;
			}
			int size = atoi(pAttribute);
			if(size != length) {
				dragon::utils::Error("midas::Xml::GetArray", __FILE__, __LINE__)
					<< "size of the ODB array " << path << ": " << size
					<< " is not equal to the size of the array to fill: " << length;
				if(success) *success = false;
				return;
			}

			for(int i=0; i< size; ++i) {
				std::stringstream valPath;
				valPath << "/value[" << i+1 << "]";
				Node valNode = mxml_find_node(node, valPath.str().c_str());
				if(!valNode) {
					dragon::utils::Error("midas::Xml::GetArray", __FILE__, __LINE__)
						<< "Unable to find value node for array index " << i;
					continue;
				}

				T value;
				if(typeid(T) != typeid(bool)) {
					std::stringstream val;
					val << valNode->value;
					val >> value;
				}
				else {
					value = (!strcmp("y", valNode->value)) ? true : false;
				}
				array[i] = value;

			}
		}

	/// Print an array
	bool PrintArray(const char* path)
		{
			Node node = FindKeyArray(path, true);
			if(!node) {
				return false;
			}
			char* pAttribute = mxml_get_attribute(node, "num_values");
			if(!pAttribute) {
				dragon::utils::Error("midas::Xml::GetArray", __FILE__, __LINE__)
					<< "\"num_values\" attribute not found for array: " << path;
				return false;
			}
			int size = atoi(pAttribute);
			for(int i=0; i< size; ++i) {
				std::stringstream valPath;
				valPath << "/value[" << i+1 << "]";
				Node valNode = mxml_find_node(node, valPath.str().c_str());
				if(!valNode) {
					dragon::utils::Error("midas::Xml::GetArray", __FILE__, __LINE__)
						<< "Unable to find value node for array index " << i;
					continue;
				}
				std::cout << path << "[" << i << "] = " << valNode->value << "\n";
			}
			return true;
		}

	/// Print a value
	bool PrintValue(const char* path)
		{
			Node node = FindKey(path, true);
			if(!node) {
				return false;
			}
			std::cout << path << " = " << node->value << "\n";
			return true;
		}

private:
	/// \brief Helper function to parse a file containing XML data and set fTree and fObd
	/// \note Most of the implementation was a paraphrase of mxml_parse_file() in midas.c,
	/// extended to handle files that contain the XML data only as a subset (i.e. MIDAS files).
	Node ParseFile(const char* file_name, char *error, int error_size, int *error_line);

	/// \brief Helper function to parse a buffer containing XML data and set fTree and fObd
	/// \note Most of the implementation was a paraphrase of mxml_parse_file() in midas.c,
	/// extended to handle files that contain the XML data only as a subset (i.e. MIDAS files).
	Node ParseBuffer(char* buf, int length, char *error, int error_size, int *error_line);

	/// \brief Parse XML tree stored in fBuffer
	/// \details This is to be used for when we write a class instance to a ROOT file.
	/// When reading it back, the contents of fBuffer are reproduced, but not until
	/// after the default constructor is done. So we have to call a separate function
	/// that would otherwise take on the role of a constructor. This is that function,
	/// and it's purpose is to parse the contents of fBuffer into fTree.
	///
	/// Note that it gets automatically called from the read functions and does some
	/// checks to make sure that fBuffer is valid and fTree is not already parsed. The
	/// result is that a user of ROOT does not have to worry at all about calling this:
	/// the class takes care of it on its own.
	void InitFromStreamer();

	/// \brief Check if fTree and fOdb are non-null
	bool Check();

	/// Disable copy
	Xml(const Xml&) { }

	/// Disable assign
	Xml& operator= (const Xml&) { return *this; }

	/// Convert node->value into template class
	template <typename T> T ConvertNode(Node& node)
		{
			T value;
			std::stringstream val;
			val << node->value;
			val >> value;
			return value;
		}

public:
#ifdef USE_ROOT
	ClassDef(midas::Xml, 1);
#endif
};

#ifndef __MAKECINT__

/// Template specialization for bool
template <>
inline bool midas::Xml::ConvertNode<bool>(Node& node)
{
	return (!strcmp("y", node->value)) ? true : false;
}

/// Template specialization for std::string
template <>
inline std::string midas::Xml::ConvertNode<std::string>(Node& node)
{
	return node->value;
}

#endif

} // namespace midas


#ifdef __MAKECINT__
#pragma link C++ function midas::Xml::GetValue(const char*, Bool_t&,    bool*);
#pragma link C++ function midas::Xml::GetValue(const char*, Char_t&,    bool*);
#pragma link C++ function midas::Xml::GetValue(const char*, Short_t&,   bool*);
#pragma link C++ function midas::Xml::GetValue(const char*, Int_t&,     bool*);
#pragma link C++ function midas::Xml::GetValue(const char*, Long64_t&,  bool*);
#pragma link C++ function midas::Xml::GetValue(const char*, UChar_t&,   bool*);
#pragma link C++ function midas::Xml::GetValue(const char*, UShort_t&,  bool*);
#pragma link C++ function midas::Xml::GetValue(const char*, UInt_t&,    bool*);
#pragma link C++ function midas::Xml::GetValue(const char*, ULong64_t&, bool*);
#pragma link C++ function midas::Xml::GetValue(const char*, Float_t&,   bool*);
#pragma link C++ function midas::Xml::GetValue(const char*, Double_t&,  bool*);
#pragma link C++ function midas::Xml::GetValue(const char*, std::string&, bool*);

#pragma link C++ function midas::Xml::GetArray(const char*, int, Bool_t*,    bool*);
#pragma link C++ function midas::Xml::GetArray(const char*, int, Char_t*,    bool*);
#pragma link C++ function midas::Xml::GetArray(const char*, int, Short_t*,   bool*);
#pragma link C++ function midas::Xml::GetArray(const char*, int, Int_t*,     bool*);
#pragma link C++ function midas::Xml::GetArray(const char*, int, Long64_t*,  bool*);
#pragma link C++ function midas::Xml::GetArray(const char*, int, UChar_t*,   bool*);
#pragma link C++ function midas::Xml::GetArray(const char*, int, UShort_t*,  bool*);
#pragma link C++ function midas::Xml::GetArray(const char*, int, UInt_t*,    bool*);
#pragma link C++ function midas::Xml::GetArray(const char*, int, ULong64_t*, bool*);
#pragma link C++ function midas::Xml::GetArray(const char*, int, Float_t*,   bool*);
#pragma link C++ function midas::Xml::GetArray(const char*, int, Double_t*,  bool*);
#pragma link C++ function midas::Xml::GetArray(const char*, int, std::string*, bool*);

#endif


#endif
