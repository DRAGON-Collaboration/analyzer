///\file Banks.hxx
///\author G. Christian
///\brief Defines structs wrapping bank-name variables.
#ifndef DRAGON_UTILS_BANKS_HXX
#define DRAGON_UTILS_BANKS_HXX


namespace midas { class Database; }

namespace dragon { namespace utils {

/// Helper class to manage bank name setting (guarantee right length, etc.)
class Banks {
public:
	/// MIDAS bank names are strings of length 4
	typedef char Name_t[5];

#ifndef __MAKECINT__
	/// Set a bank name (from string literal)
	static void Set(Name_t bkName, const char* from);
	/// Set a bank name (from ODB)
	static void OdbSet(Name_t bkName, midas::Database& odb, const char* path);
	/// Set an array of bank names (from ODB)
	static void OdbSetArray(utils::Banks::Name_t* bkName, int length, midas::Database& odb, const char* path);
#endif

};

/// Bank names for a scaler event
struct ScalerBanks {
	/// Count bank name
	Banks::Name_t count;
	/// Sum bank name
	Banks::Name_t sum;
	/// Rate bank name
	Banks::Name_t rate;
};

/// Bank names for a "detector" event
/*!
 * \tparam NADC Number of ADCs.
 * \tparam NTDC Number of TDCs.
 */
template <size_t NADC, size_t NTDC>
struct EventBanks {
	/// IO32 bank name
	Banks::Name_t io32;
	/// Timestamp counter (TSC) bank name
	Banks::Name_t tsc;
	/// ADC bank names
	Banks::Name_t adc[NADC];
	/// TDC bank names
	Banks::Name_t tdc[NTDC];
};

/// EventBanks<> specialization for 1 adc
template <size_t NTDC>
struct EventBanks<1, NTDC> {
	/// IO32 bank name
	Banks::Name_t io32;
	/// Timestamp counter (TSC) bank name
	Banks::Name_t tsc;
	/// ADC bank names (not an array)
	Banks::Name_t adc;
	/// TDC bank names
	Banks::Name_t tdc[NTDC];
};

/// EventBanks<> specialization for 1 tdc
template <size_t NADC>
struct EventBanks<NADC, 1> {
	/// IO32 bank name
	Banks::Name_t io32;
	/// Timestamp counter (TSC) bank name
	Banks::Name_t tsc;
	/// ADC bank names
	Banks::Name_t adc[NADC];
	/// TDC bank names (not an array)
	Banks::Name_t tdc;
};

/// EventBanks<> specialization for 1 adc & 1 tdc
template <>
struct EventBanks<1, 1> {
	/// IO32 bank name
	Banks::Name_t io32;
	/// Timestamp counter (TSC) bank name
	Banks::Name_t tsc;
	/// ADC bank names
	Banks::Name_t adc;
	/// TDC bank names (not an array)
	Banks::Name_t tdc;
};

} } // namespace dragon, namespace utils



#ifdef __MAKECINT__
#pragma link C++ class dragon::utils::EventBanks<2, 1>+;
#pragma link C++ class dragon::utils::EventBanks<1, 1>+;
#endif

#endif
