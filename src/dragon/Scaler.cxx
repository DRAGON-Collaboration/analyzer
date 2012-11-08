/// \file Scaler.cxx
/// \author C. Stanford
/// \author G. Christian
/// \brief Implements Scaler.hxx
#include <sstream>
#include <iostream>
#ifdef USE_ROOT
#include <TTree.h>
#endif
#include "utils/Error.hxx"
#include "midas/Database.hxx"
#include "midas/Event.hxx"
#include "Scaler.hxx"


// ========== Class dragon::Scaler ========== //

dragon::Scaler::Scaler()
{
	/*! Calls reset() */
	reset();
}

void dragon::Scaler::reset()
{
	/*! Sets all count, rate, and sum to zero */
  std::fill(count, count + MAX_CHANNELS, 0);
	std::fill(sum, sum + MAX_CHANNELS, 0);
	std::fill(rate, rate + MAX_CHANNELS, 0.);
}

namespace {
inline bool check_bank_len(int expected, int gotten, const char* bkname)
{
	bool retval = true;
	if (expected != gotten) {
		utils::err::Error("dragon::Scaler::unpack")
			<< "Enexpected length of bank \"" << bkname << "\": expected " << expected
			<< ", got " << gotten << DRAGON_ERR_FILE_LINE;
		retval = false;
	}
	return retval;
} }

void dragon::Scaler::unpack(const midas::Event& event)
{
	/*!
	 * Unpacks scaler data directly into the various array structures.
	 */
	int bank_len;
	uint32_t* pcount = event.GetBankPointer<uint32_t>(variables.bank_names.count, &bank_len, false, true);
	if ( check_bank_len(MAX_CHANNELS, bank_len, variables.bank_names.count) )
		std::copy(pcount, pcount + bank_len, count);

	uint32_t* psum = event.GetBankPointer<uint32_t>(variables.bank_names.sum, &bank_len, false, true);
	if ( check_bank_len(MAX_CHANNELS, bank_len, variables.bank_names.sum) )
		std::copy(psum, psum + bank_len, sum);

	double* prate = event.GetBankPointer<double>(variables.bank_names.rate, &bank_len, false, true);
	if ( check_bank_len(MAX_CHANNELS, bank_len, variables.bank_names.rate) )
		std::copy(rate, rate + bank_len, rate);
}

const std::string& dragon::Scaler::channel_name(int ch) const
{
	/*!
	 * \param ch Channel number
	 */
	if (ch >= 0 && ch < MAX_CHANNELS) return variables.names[ch];
	utils::err::Error("dragon::Scaler::channel_name")
		<< "Invalid channel number: " << ch << ". Valid arguments are 0 <= ch < " << MAX_CHANNELS;
	static std::string junk = "";
	return junk;
}

#ifdef USE_ROOT
void dragon::Scaler::set_aliases(TTree* tree, const char* branchName) const
{
	/*!
	 * Sets tree branch aliases based on variable values - results in easier to use
	 * branch names (e.g. something descriptive instead of 'scaler.sum[0]', etc.)
	 * \param tree Pointer to the TTree for which you want to set aliases
	 * \param branchName "Base" name of the branch
	 *
	 * Example:
	 * \code
	 * TTree t("t","");
	 * dragon::Scaler scaler;
	 * scaler.variables.names[0] = "bgo_triggers_presented";
	 * void* pScaler = &scaler;
	 * t.Branch("scaler", "dragon::Scaler", &pScaler);
	 * scaler.set_aliases(&t, "scaler");
	 * t.Fill(); // adds a events worth of data
	 * t.Draw("scaler_count_bgo_triggers_presented"); // same as doing t.Draw("scaler.count[0]");
	 * \endcode
	 */
	const std::string chNames[3] = { "count", "sum", "rate" };
	for(int i=0; i< MAX_CHANNELS; ++i) {
		for(int j=0; j< 3; ++j) {
			std::stringstream oldName, newName;
			oldName << branchName << "." << chNames[j] << "[" << i << "]";
			newName << branchName << "_" << chNames[j] << "_" << variables.names[i];

			tree->SetAlias(newName.str().c_str(), oldName.str().c_str());
		}
	}
}
#endif


// ========== Class Scaler::Variables ========== //

dragon::Scaler::Variables::Variables()
{
	/*! Calls reset(); */
	reset();
}

void dragon::Scaler::Variables::reset()
{
	/*! Resets every channel to a default name: \c channel_n */
	for (int i=0; i< MAX_CHANNELS; ++i) {
		std::stringstream strm_name;
		strm_name << "channel_" << i;
		names[i] = strm_name.str();
	}
	set_bank_names("SCH");
}

void dragon::Scaler::Variables::set(const char* odb)
{
	midas::Database database(odb);
	if(database.IsZombie()) return;
		
	database.ReadArray("/dragon/scaler/head/names", names, MAX_CHANNELS);

	std::string sCount, sSum, sRate;
	database.ReadValue("/dragon/scaler/head/bank_names/count", sCount);
	database.ReadValue("/dragon/scaler/head/bank_names/rate", sRate);
	database.ReadValue("/dragon/scaler/head/bank_names/sum", sSum);
	utils::Banks::Set(bank_names.count, sCount.c_str());
	utils::Banks::Set(bank_names.rate,  sRate.c_str());
	utils::Banks::Set(bank_names.sum ,  sSum.c_str());
}

void dragon::Scaler::Variables::set_bank_names(const char* base)
{
	std::string strbase(base);
	if(strbase.size() != 3) {
		utils::err::Warning("dragon::Scaler::Variables::set_bank_names")
			<< "Length of base: \"" << base << "\" != 3. Truncating...";
		if(strbase.size() > 3) strbase = strbase.substr(0, 3);
		else while (strbase.size() != 3) strbase.push_back('0');
		std::cerr << "Base = " << strbase << "\n";
	}
	utils::Banks::Set(bank_names.count, (strbase + "D").c_str());
	utils::Banks::Set(bank_names.rate,  (strbase + "R").c_str());
	utils::Banks::Set(bank_names.sum ,  (strbase + "S").c_str());
}
