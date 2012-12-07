///\file Banks.cxx
///\author G. Christian
///\brief Implements Banks.hxx
#include <cstring>
#include <string>
#include "midas/Database.hxx"
#include "Error.hxx"
#include "Banks.hxx"


void utils::Banks::Set(utils::Banks::Name_t bkName, const char* from)
{
	/*!
	 * Sets a new bank name, checking to make sure the length is correct.
	 * \param [out] bkName Bank name to set
	 * \param [in] from Desired new value of \e bkName
	 */
	int fromLen = strlen(from);
	if (fromLen >= 4) {
		if (fromLen > 4) {
			utils::err::Warning("utils::Banks::Set")
				<< "Source string longer than 4: truncating bank name to " << bkName
				<< DRAGON_ERR_FILE_LINE;
		}
		strncpy(bkName, from, 4);
		bkName[4] = '\0';
	}
	else {
		utils::err::Warning("utils::Banks::Set")
			<< "Source string shorter than 4: extending bank name to " << bkName
			<< DRAGON_ERR_FILE_LINE;
		strncpy(bkName, from, fromLen);
		for(int i= fromLen; i < 4; ++i) bkName[i] = '0';
		bkName[4] = '\0';
	}
}

void utils::Banks::OdbSet(utils::Banks::Name_t bkName, midas::Database& odb, const char* path)
{
	std::string temp;
	odb.ReadValue(path, temp);
	utils::Banks::Set(bkName, temp.c_str());
}

		
