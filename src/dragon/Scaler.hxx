/// \file Scaler.hxx
/// \author C. Stanford
/// \author G. Christian
/// \brief Defines classes relevant to unpacking data from Scaler modules.
#ifndef DRAGON_SCALER_HXX
#define DRAGON_SCALER_HXX
#include <string>


#ifdef USE_ROOT
class TTree;
#endif

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
 public:
		/** @cond */
 PRIVATE:
		/** @endcond */
		/// Name of a given channel
		std::string names[MAX_CHANNELS];

		/// Scaler bank names
		struct BankNames {
			std::string sum;   ///< Scaler sum bank name
			std::string count; ///< Scaler counts bank name
			std::string rate;  ///< Scaler rate bank name
		};
		/// Frontend Bank names
		BankNames bank_names;

    /// Constuctor
    Variables();
		/// Resets names to default values
		void reset();
		/// Set names from ODB
		void set(const char* odb);
		/// Set bank names
		void set_bank_names(const char* base);
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
  
  /// Initialize data
  Scaler();
  
  /// Reset all data to zero
  void reset();

  /// Unpack Midas event data into scalar data structiures
  void unpack(const midas::Event& event);

	/// Returns the name of a given scaler channel
	const std::string& channel_name(int ch) const;

#ifdef USE_ROOT
	/// Set branch alises in a ROOT TTree.
	void set_aliases(TTree* tree, const char* branchName) const;
#endif
};

} // namespace dragon

#endif
