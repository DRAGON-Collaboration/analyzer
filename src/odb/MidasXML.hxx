//! \file MidasXML.hxx
//! \brief Defines a class to parse MIDAS XML files from the ODB.
#ifndef MIDAS_XML_HXX
#define MIDAS_XML_HXX
#include <vector>
#include "mxml.h"

/// \brief Class to parse MIDAS ODB XML files.
class MidasXML {
public:
	 /// Pointer to an XML node.
	 typedef PMXML_NODE Node;

private:
	 /// Pointer to the entire XML tree contained within a file
	 Node fTree;
	 /// Pointer to the ODB portion of fTree
	 Node fOdb;

public:
	 /// \brief Read data from an XML file
	 /// \details Parses a file containing XML data to fill fTree and fOdb.
	 /// Can handle either a dedicated \c .xml file or a \c .mid (or any other) file containing
	 /// the XML data as a subset.
	 /// \note Memory is allocated only to fTree, fOdb and any other subnodes used later
	 /// simply refer to memory locations relative to fTree.
	 MidasXML(const char* filename);

	 /// Frees resources allocated to fTree
	 ~MidasXML();

	 /// \brief Find the node location of a specific key element within the xml file
	 /// \param [in] path String specifying the "directory" path of the element, e.g.
	 /// "Equipment/gTrigger/Variables/Pedestals".
	 Node FindKey(const char* path);

	 /// \brief Find the node location of a specific keyarray element within the xml file
	 /// \param [in] path String specifying the "directory" path of the element, e.g.
	 /// "Equipment/gTrigger/Variables/Pedestals".
	 Node FindKeyArray(const char* path);

	 /// \brief Get the value of key element
	 /// \tparam The type of the key element (e.g. int, float, string, ...)
	 /// \param [in] path String specifying the "directory" path of the element, e.g.
	 /// "Equipment/gTrigger/Variables/Pedestals"
	 /// \param [out] success Sets the value to \c true if \e path was valid, \c false otherwise.
	 ///  Passing a NULL value to the function disables the error reporting
	 /// \returns The value of the element specified by \e path
	 template <typename T> T GetValue(const char* path, bool* success = 0);

	 /// \brief Get the value of key element
	 /// \tparam The type of the key element (e.g. int, float, string, ...)
	 /// \param [in] path String specifying the "directory" path of the element, e.g.
	 /// "Equipment/gTrigger/Variables/Pedestals"
	 /// \param [out] value Set to the value of the element specified by \e path
	 /// \param [in] success Sets the value to \c true if \e path was valid, \c false otherwise.
	 ///  Passing a NULL value to the function disables the error reporting
	 template <typename T> void GetValue(const char* path, T& value, bool* success = 0);

	 /// \brief Get the values of an array of key elements
	 /// \tparam The type of the key element (e.g. int, float, string, ...)
	 /// \param [out] path String specifying the "directory" path of the element, e.g.
	 /// "Equipment/gTrigger/Variables/Pedestals"
	 /// \param [out] success Sets the value to \c true if \e path was valid, \c false otherwise.
	 ///  Passing a NULL value to the function disables the error reporting
	 /// \returns A vector of all array values
	 template <typename T> std::vector<T> GetArray(const char* path, bool* success = 0);

	 /// \brief Get the values of an array of key elements
	 /// \tparam The type of the key element (e.g. int, float, string, ...)
	 /// \param [in] path String specifying the "directory" path of the element, e.g.
	 /// "Equipment/gTrigger/Variables/Pedestals"
	 /// \param [out] array Set to a vector of all array values
	 /// \param [out] success Sets the value to \c true if \e path was valid, \c false otherwise.
	 ///  Passing a NULL value to the function disables the error reporting
	 template <typename T> void GetArray(const char* path, std::vector<T>& array, bool* success = 0);

	 /// \brief Get the values of an array of key elements
	 /// \tparam The type of the key element (e.g. int, float, string, ...)
	 /// \param [in] path String specifying the "directory" path of the element, e.g.
	 /// "Equipment/gTrigger/Variables/Pedestals"
	 /// \param [in] length Length of the array to fill
	 /// \param [out] array Array to fill with values from the ODB
	 /// \param [out] success Sets the value to \c true if \e path was valid, \c false otherwise.
	 ///  Passing a NULL value to the function disables the error reporting
	 template <typename T> void GetArray(const char* path, int length, T* array, bool* success = 0);

private:
	 /// \brief Helper initialization function called by the constructor.
	 void Init(const char* filename);
	 /// \brief Helper function to parse a file containing XML data and set fTree and fObd
	 /// \note Most of the implementation was a paraphrase of mxml_parse_file() in midas.c,
	 /// extended to handle files that contain the XML data only as a subset (i.e. MIDAS files).
	 Node ParseFile(const char* file_name, char *error, int error_size, int *error_line);
	 /// \brief Check if fTree and fOdb are non-null
	 bool Check();
};


#endif
