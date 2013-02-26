/// \file Database.hxx
/// \author G. Christian
/// \brief Defines a generic (online or from XML file) database reader
#ifndef DRAGON_MIDAS_DATABASE_HXX
#define DRAGON_MIDAS_DATABASE_HXX
#include <memory>
#include "utils/ErrorDragon.hxx"
#include "Odb.hxx"
#include "Xml.hxx"

namespace midas {

/// Generic database reading class
/*!
 * With this class, users can read data from either the ODB or from
 * a saved XML (or .mid) file using a single structure. Whether to read
 * from the ODB or a file is specified by the constructor argument, which
 * is either the path to a file containing XML data, or "online" to read
 * from the ODB.
 */
class Database {

private:
	/// Pointer to Xml reader (NULL if in 'online mode')
	std::auto_ptr<Xml> fXml;

	/// Flag specifying online mode
	bool fIsOnline;

	/// Flag specifying 'zombie' status
	bool fIsZombie;

public:
	/// Determines online or offline mode
	Database (const char* filename): fXml(0), fIsOnline(false), fIsZombie(false)
		{
			/*!
			 * \param filename Name of the XML (or .mid) file from which to read
			 *  data. \note Specifying "online" reads from the ODB if connected to an
			 *  experiment.
			 */
			if (strcmp(filename, "online")) {
				fXml.reset(new Xml(filename));
				if (fXml->IsZombie()) {
					fXml.reset(0);
					fIsZombie = true;
				}
			}
			else {
				fIsOnline = true;
				if (Odb::GetHandle() == 0)
					fIsZombie = true;
			}
			if (fIsZombie) {
				utils::err::Error("midas::Database::Database")
					<< "Failed opening the database: \"" << filename << "\"";
			}
		}

	/// Tell the public if a zombie or not
	bool IsZombie() const { return fIsZombie; }

	/// Read a single value
	template <typename T> bool ReadValue(const char* path, T& value) const
		{
			/*!
			 * \param [in] path Path ODB directory path of what is to be read
			 * \param [out] array The value you want to read
			 * \tparam T The type of the data to be read
			 * \returns true if read was successful, false otherwise
			 */
			if(fIsZombie) return false;
			if (fIsOnline) return Odb::ReadValue(path, value);
			else if (fXml.get()) {
				bool success;
				fXml->GetValue(path, value, &success);
				return success;
			}
			else return false;
		}

	template <typename T> int ReadArray(const char* path, T* array, int length) const
		{
			/*!
			 * \param [in] path Path ODB directory path of what is to be read
			 * \param [out] array The value you want to read
			 * \param [in] length Length of the array to be read
			 * \tparam T The type of the data in the array to be read
			 * \returns The length of the array that was read (0 if error)
			 */
			if(fIsZombie) return false;
			if (fIsOnline) return Odb::ReadArray(path, array, length);
			else if (fXml.get()) {
				bool success;
				fXml->GetArray(path, length, array, &success);
				return length;
			}
			else return 0;
		}
};

}


#endif
