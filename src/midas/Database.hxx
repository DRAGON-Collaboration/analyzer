/// \file Database.hxx
/// \author G. Christian
/// \brief Defines a generic (online or from XML file) database reader
#ifndef DRAGON_MIDAS_DATABASE_HXX
#define DRAGON_MIDAS_DATABASE_HXX
#include <memory>
#include "utils/ErrorDragon.hxx"
#include "Odb.hxx"
#include "Xml.hxx"

#ifdef USE_ROOT
#define DRAGON_DATABASE Database: public TObject
#else
#define DRAGON_DATABASE Database
#endif


namespace midas {

/// Generic database reading class
/*!
 * With this class, users can read data from either the ODB or from
 * a saved XML (or .mid) file using a single structure. Whether to read
 * from the ODB or a file is specified by the constructor argument, which
 * is either the path to a file containing XML data, or "online" to read
 * from the ODB.
 */
class DRAGON_DATABASE {

private:
	/// Pointer to Xml reader (NULL if in 'online mode')
	std::auto_ptr<Xml> fXml;

	/// Flag specifying online mode
	bool fIsOnline;

	/// Flag specifying 'zombie' status
	bool fIsZombie;

public:
	/// Default constructor for ROOT I/O
	Database (): fXml(0), fIsOnline(false), fIsZombie(false)
		{ }

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
				dragon::utils::Error("midas::Database::Database")
					<< "Failed opening the database: \"" << filename << "\"";
			}
		}

	/// Read from buffered XML data
	Database (char* buf, int length): fXml(0), fIsOnline(false), fIsZombie(false)
		{
			/*!
			 * \param buf Buffer containing xml data.
			 * \param length Size of the buffer in bytes
			 */
			fXml.reset(new Xml(buf, length));
			if (fXml->IsZombie()) {
				fXml.reset(0);
				fIsZombie = true;
			}
			if (fIsZombie) {
				dragon::utils::Error("midas::Database::Database")
					<< "Failed parsing the XML data.";
			}
		}

	/// Tell the public if a zombie or not
	bool IsZombie() const { return fIsZombie; }

	/// Dump odb contents to an output stream
	void Dump(std::ostream& strm = std::cout)
		{
			if(fIsZombie) return;
			if(fIsOnline) {
				dragon::utils::Error("Database::Dump")
					<< "Currently not supported for online database";
				return;
			}
			if(fXml.get()) fXml->Dump(strm);
		}

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

	/// Read the length of an array
	int ReadArrayLength(const char* path) const
		{
			/*!
			 * \param [in] path Path ODB directory path of what is to be read
			 * \returns The length of the array upon success, -1 upon failure.
			 */
			if      (fIsZombie)  return -1;
			else if (fIsOnline)  return Odb::ReadArraySize(path);
			else if (fXml.get()) return fXml->GetArrayLength(path);
			else                 return -1;
		}

	/// Read an array
	template <typename T> int ReadArray(const char* path, T* array, int length) const
		{
			/*!
			 * \param [in] path Path ODB directory path of what is to be read
			 * \param [out] array The value you want to read
			 * \param [in] length Length of the array to be read
			 * \tparam T The type of the data in the array to be read
			 * \returns The length of the array that was read (0 if error)
			 */
			if(fIsZombie) return 0;
			if(fIsOnline) return Odb::ReadArray(path, array, length);
			else if (fXml.get()) {
				bool success;
				fXml->GetArray(path, length, array, &success);
				return length;
			}
			else return 0;
		}

#ifdef USE_ROOT
	ClassDef(midas::Database, 1);
#endif
};

}

#ifdef __MAKECINT__
#pragma link C++ function midas::Database::ReadValue (const char*, Char_t&);
#pragma link C++ function midas::Database::ReadValue (const char*, Short_t&);
#pragma link C++ function midas::Database::ReadValue (const char*, Int_t&);
#pragma link C++ function midas::Database::ReadValue (const char*, Long64_t&);
#pragma link C++ function midas::Database::ReadValue (const char*, UChar_t&);
#pragma link C++ function midas::Database::ReadValue (const char*, UShort_t&);
#pragma link C++ function midas::Database::ReadValue (const char*, UInt_t&);
#pragma link C++ function midas::Database::ReadValue (const char*, ULong64_t&);
#pragma link C++ function midas::Database::ReadValue (const char*, Float_t);
#pragma link C++ function midas::Database::ReadValue (const char*, Double_t&);

#pragma link C++ function midas::Database::ReadArray (const char*, Char_t*, int);
#pragma link C++ function midas::Database::ReadArray (const char*, Short_t*, int);
#pragma link C++ function midas::Database::ReadArray (const char*, Int_t*, int);
#pragma link C++ function midas::Database::ReadArray (const char*, Long64_t*, int);
#pragma link C++ function midas::Database::ReadArray (const char*, UChar_t*, int);
#pragma link C++ function midas::Database::ReadArray (const char*, UShort_t*, int);
#pragma link C++ function midas::Database::ReadArray (const char*, UInt_t*, int);
#pragma link C++ function midas::Database::ReadArray (const char*, ULong64_t*, int);
#pragma link C++ function midas::Database::ReadArray (const char*, Float_t*, int);
#pragma link C++ function midas::Database::ReadArray (const char*, Double_t*, int);

#endif


#endif
