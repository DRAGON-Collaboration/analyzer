/// \file Scaler.hxx
/// \author C. Stanford
/// \author G. Christian
/// \brief Defines classes relevant to unpacking data from Scaler modules.
#ifndef DRAGON_SCALER_HXX
#define DRAGON_SCALER_HXX
#include <string>
#include <sstream>
#include "utils/Banks.hxx"

namespace midas { class Event; }

namespace dragon {

/// Generic dragon scaler class
class Scaler {

	// Static const data //
public:
	/// Number of scaler channels
	static const int MAX_CHANNELS = 32; //!

	// Sub classes //
public:
  /// Scalar variables
  class Variables {
		// Class data //
 public:
		/** @cond */
 PRIVATE:
		/** @endcond */
		/// Name of a given channel
		std::string names[MAX_CHANNELS];

		/// Frontend bank names
		utils::ScalerBanks bank_names;

		/// Base odb path
		std::string odb_path; //!

		// Class Methods //
 public:
    /// Constuctor
    Variables(const char* name);
		/// Resets names to default values
		void reset();
		/// Set names from ODB
		void set(const char* odb);
		/// Set bank names
		void set_bank_names(const char* base);
		/// Allow owning class access to internals
		friend class dragon::Scaler;
  };

	/// Variables instance
	Variables variables;

	// Class data //
public:
	/** @cond */
PRIVATE:
	/** @endcond */

	/// Number of counts in a single read period
	uint32_t count[MAX_CHANNELS];    //#

	/// Number of counts over the course of a run
	uint32_t sum[MAX_CHANNELS];      //#

	/// Average count rate over the course of a run
	double rate[MAX_CHANNELS];       //#

public:  
  /// Initialize data
  Scaler(const char* name = "head");
  
  /// Reset all data to zero
  void reset();

  /// Unpack Midas event data into scalar data structiures
  void unpack(const midas::Event& event);

	/// Returns the name of a given scaler channel
	const std::string& channel_name(int ch) const;

	/// Sets variable values
	void set_variables(const char* odb);

	/// Set branch alises in a ROOT TTree.
	template <class T>
	void set_aliases(T* t, const char* branchName) const;
};

} // namespace dragon

template <class T>
void dragon::Scaler::set_aliases(T* t, const char* branchName) const
{
	/*!
	 * Sets tree branch aliases based on variable values - results in easier to use
	 * branch names (e.g. something descriptive instead of 'scaler.sum[0]', etc.)
	 *
	 * \param t Pointer to a class having a "SetAlias(const char*, const char*) method
	 * (e.g. TTree) and for which you want to set aliases.
	 * \param branchName "Base" name of the branch
	 * \tparam T Some class with a "SetAlias" method.
	 *
	 * \note Intended for TTree, but defined as a template
	 * function to maintain compatability with systems that don't use ROOT. In the case that ROOT is
	 * not available and this function is never used, it simply gets ignored by the compiler. Note also,
	 * that for systems with ROOT, a CINT dictionary definition is automoatically provided for TTree
	 * as the template parameter.
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

			t->SetAlias(newName.str().c_str(), oldName.str().c_str());
		}
	}
}


#endif // Include guard
