///\file Banks.hxx
///\author G. Christian
///\brief Defines structs wrapping bank-name variables.
#ifndef DRAGON_BANKS_HXX
#define DRAGON_BANKS_HXX


namespace dragon {

/// Helper class to manage bank name setting (guarantee right length, etc.)
class Banks {
public:
	/// MIDAS bank names are strings of length 4
	typedef char Name_t[5];

#ifndef __MAKECINT__
	/// Set a bank (from string literal)
	static void Set(Name_t bkName, const char* from);
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

} // namespace dragon



#ifndef __MAKECINT__

// Inline implementation //
#include <string.h>
#include "utils/Error.hxx"

inline void dragon::Banks::Set(dragon::Banks::Name_t bkName, const char* from)
{
	/*!
	 * Sets a new bank name, checking to make sure the length is correct.
	 * \param [out] bkName Bank name to set
	 * \param [in] from Desired new value of \e bkName
	 */
	int fromLen = strlen(from);
	if (fromLen >= 4) {
		strncpy(bkName, from, 4);
		bkName[4] = '\0';
		if (fromLen > 4) {
			utils::err::Warning("dragon::Banks::Set")
				<< "Source string longer than 4: truncating bank name to " << bkName
				<< DRAGON_ERR_FILE_LINE;
		}
	}
	else {
		strncpy(bkName, from, fromLen);
		for(int i= fromLen; i < 4; ++i) bkName[i] = '0';
		bkName[4] = '\0';
			utils::err::Warning("dragon::Banks::Set")
				<< "Source string shorter than 4: extending bank name to " << bkName
				<< DRAGON_ERR_FILE_LINE;
	}
}

#else  // __MAKECINT__ defined

#pragma link C++ class dragon::EventBanks<2, 1>+;
#pragma link C++ class dragon::EventBanks<1, 1>+;

#endif // #ifndef __MAKECINT__

#endif
