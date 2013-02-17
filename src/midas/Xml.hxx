//! \file Xml.hxx
//! \author G. Christian
//! \brief Defines a class to parse MIDAS XML files from the ODB.
#ifndef MIDAS_XML_HXX
#define MIDAS_XML_HXX
#include <cstdlib>
#include <cstring>
#include <vector>
#include <sstream>
#include <typeinfo>
#include <iostream>
#include "internal/mxml.h"
#include "internal/strlcpy.h"

namespace midas {

/// Class to parse MIDAS ODB XML files.
/*!
 * \attention If you want to be able to cleanly read from either the
 * ODB or an offline file, it is suggested to use the class midas::Database
 * instead of this one, as that class can read from either. In midas::Database
 * All of the "work" to read from an XML (or .mid) file is handled by the
 * template functions of this class.
 */
class Xml {
public:
	/// Pointer to an XML node.
	typedef PMXML_NODE Node;

private:
	/// Pointer to the entire XML tree contained within a file
	Node fTree;
	/// Pointer to the ODB portion of fTree
	Node fOdb;
	/// Flag specifying if the file was invalid
	bool fIsZombie;

public:
	/// \brief Read data from an XML file
	/// \details Parses a file containing XML data to fill fTree and fOdb.
	/// Can handle either a dedicated \c .xml file or a \c .mid (or any other) file containing
	/// the XML data as a subset.
	/// \note Memory is allocated only to fTree, fOdb and any other subnodes used later
	/// simply refer to memory locations relative to fTree.
	Xml(const char* filename);

	/// Frees resources allocated to fTree
	~Xml();

	/// Returns fIsZombie
	bool IsZombie() { return fIsZombie; }

	/// \brief Find the node location of a specific key element within the xml file
	/// \param [in] path String specifying the "directory" path of the element, e.g.
	/// "Equipment/gTrigger/Variables/Pedestals".
	Node FindKey(const char* path);

	/// \brief Find the node location of a specific keyarray element within the xml file
	/// \param [in] path String specifying the "directory" path of the element, e.g.
	/// "Equipment/gTrigger/Variables/Pedestals".
	Node FindKeyArray(const char* path);

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
				std::cerr << "Error: \"num_values\" attribute not found for array: " << path << "\n";
				if(success) *success = false;
				return;
			}
			int size = atoi(pAttribute);
			for(int i=0; i< size; ++i) {
				std::stringstream valPath;
				valPath << "/value[" << i+1 << "]";
				Node valNode = mxml_find_node(node, valPath.str().c_str());
				if(!valNode) {
					std::cerr << "Error: Unable to find value node for array index " << i << "\n";
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
				std::cerr << "Error: \"num_values\" attribute not found for array: " << path << "\n";
				if(success) *success = false;
				return;
			}
			int size = atoi(pAttribute);
			if(size != length) {
				std::cerr << "Error: size of the ODB array " << path << ": " << size
									<< " is not equal to the size of the array to fill: " << length << "\n";
				if(success) *success = false;
				return;
			}

			for(int i=0; i< size; ++i) {
				std::stringstream valPath;
				valPath << "/value[" << i+1 << "]";
				Node valNode = mxml_find_node(node, valPath.str().c_str());
				if(!valNode) {
					std::cerr << "Error: Unable to find value node for array index " << i << "\n";
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


private:
	/// \brief Helper initialization function called by the constructor.
	bool Init(const char* filename);

	/// \brief Helper function to parse a file containing XML data and set fTree and fObd
	/// \note Most of the implementation was a paraphrase of mxml_parse_file() in midas.c,
	/// extended to handle files that contain the XML data only as a subset (i.e. MIDAS files).
	Node ParseFile(const char* file_name, char *error, int error_size, int *error_line);

	/// \brief Check if fTree and fOdb are non-null
	bool Check();

	/// Disable copy
	Xml(const Xml& other) { }

	/// Disable assign
	Xml& operator= (const Xml& other) { return *this; }

	/// Convert node->value into template class
	template <typename T> T ConvertNode(Node& node)
		{	
			T value;
			std::stringstream val;
			val << node->value;
			val >> value;
			return value;
		}
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

#endif
