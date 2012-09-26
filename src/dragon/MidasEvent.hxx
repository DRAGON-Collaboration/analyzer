/// \file MidasEvent.hxx
/// \author G. Christian
/// \brief Defines an inherited class of TMidasEvent that includes
/// specific functionality for timestamp coincidence matching.
#ifndef DRAGON_MIDAS_EVENT_HXX
#define DRAGON_MIDAS_EVENT_HXX
#include <map>
#include <set>
#include <cmath>
#include <cstdio>
#include <vector>
#include <cassert>
#include <typeinfo>
#include "midas/TMidasFile.h"
#include "midas/TMidasEvent.h"
#include "utils/Error.hxx"

namespace dragon {

/// Derived class of TMidasEvent for timestamped dragon events
/*!
 * Stores timestamp values as fields for easy access. Also provides
 * constructors to set an event from the addresses returned by polling.
 *
 * Inherits privately from TMidasEvent and reproduces much of its public
 * interface (private inheritance due to the non-virtual destructor of
 * TMidasEvent).
 */
class MidasEvent: private TMidasEvent {

public:
	// Reproduce parts of TMidasEvent interface
  using TMidasEvent::GetEventId;
  using TMidasEvent::GetTriggerMask;
  using TMidasEvent::GetSerialNumber;
  using TMidasEvent::GetTimeStamp;
  using TMidasEvent::GetDataSize;
  using TMidasEvent::Print;
	using TMidasEvent::GetBankList;
	using TMidasEvent::FindBank;
	using TMidasEvent::LocateBank;
	using TMidasEvent::IsBank32;
	using TMidasEvent::IterateBank;
	using TMidasEvent::IterateBank32;

private:
	/// Default ('master') clock frequency in MHz
	double kDefaultFreq;

	/// Coincidence window (in us)
	double fCoincWindow;

	/// Timestamp value in clock cycles since BOR
	uint64_t fClock;

	/// Crossed timestamp clock value(s)
	std::vector<uint64_t> fCrossClock;

	/// Timestamp value in uSec
	double fTriggerTime;

	/// Clock frequency
	double  fFreq;

public:
	/// Construct from event callback parameters
	MidasEvent(const char* tsbank, const void* header, const void* data, int size);

	/// Construct from direct polling parameters
	MidasEvent(const char* tsbank, char* buf, int size);

	/// Copy constructor
	MidasEvent(const MidasEvent& other) { CopyDerived(other); }

	/// Assignment operator
	MidasEvent& operator= (const MidasEvent& other)
		{ CopyDerived(other); return *this; }

	/// Copies event header information into another one
	void CopyHeader(EventHeader_t& destination) const
		{	memcpy (&destination, &fEventHeader, sizeof(EventHeader_t)); }

	/// Read an event from a TMidasFile
	bool ReadFromFile(TMidasFile& file)
		{	return file.Read(this);	}

	/// Returns trigger time in uSec
	double TriggerTime() const { return fTriggerTime; }

	/// Equivalency operator
	bool operator== (const MidasEvent& rhs) const
		{ return IsCoinc (rhs); }

	/// Less than operator
	/*! \returns false if the two event's trigger times are within
	 *  the coincidence window; otherwise returns true if the trigger
	 *  time of 'this' is less than the trigger time of 'other' */
	bool operator< (const MidasEvent& rhs) const
		{
			if (IsCoinc(rhs)) return false;
			return fTriggerTime < rhs.fTriggerTime;
		}

	/// Prints timestamp information for a singles event
	void PrintSingle(FILE* where = stdout) const;

	/// Prints timestamp information for coincidence events
	void PrintCoinc(const MidasEvent& other, FILE* where = stdout) const;

	/// Calculates difference of timestamps
	/*! \note 'this' - 'other' */
	double TimeDiff(const MidasEvent& other) const
		{ return fTriggerTime - other.fTriggerTime; }

	/// Destructor, empty
	virtual ~MidasEvent() { }

	/// Bank finding routine (templated)
	template <typename T>
	T* GetBankPointer(const char* name, int* length, bool reportMissing = false, bool checkType = false) const
		{
			/*!
			 * \param [in] name Name of the bank to search for
			 * \param [out] Length of the returned bank
			 * \param [in] reportMissing True means a warning message is printed if the bank is absent
			 * \param [in] checkType Specifies whether or not to check that the template parameter
			 *  matches the TID of the bank. If this parameter is set to true and the types do
			 *  not match, the error is fatal.
			 * \returns Pointer to the beginning of the bank
			 */
			void *pbk;
			int type;
			int bkfound = FindBank(name, length, &type, &pbk);
			if(!bkfound && reportMissing) {
				err::Warning("dragon::MidasEvent::GetBankPointer<T>")
					<< "Couldn't find the MIDAS bank \"" << name  << "\". Skipping...\n";
			}
			if (bkfound && checkType) {
				switch (type) {
				case 1:  assert (typeid(T) == typeid(unsigned char)); break;
				case 2:  assert (typeid(T) == typeid(char));          break;
				case 3:  assert (typeid(T) == typeid(unsigned char)); break;
				case 4:  assert (typeid(T) == typeid(uint16_t));      break;
				case 5:  assert (typeid(T) == typeid(int16_t));       break;
				case 6:  assert (typeid(T) == typeid(uint32_t));      break;
				case 7:  assert (typeid(T) == typeid(int32_t));       break;
				case 8:  assert (typeid(T) == typeid(bool));          break;
				case 9:  assert (typeid(T) == typeid(float));         break;
				case 10: assert (typeid(T) == typeid(double));        break;
				default:
					fprintf(stderr, "Unknown type id: %i\n", type);
					assert(false); break;
				}
			}
			return bkfound ? reinterpret_cast<T*>(pbk) : 0;
		}

private:
	/// Helper function for copy constructor / assignment operator
	void CopyDerived(const dragon::MidasEvent& other);

	/// Checks if two events are coincident
	bool IsCoinc(const MidasEvent& other) const
		{ return fabs(TimeDiff(other)) < fCoincWindow; }

	/// Helper function for constructors
	void Init(const char* tsbank, const void* header, const void* addr, int size);

};

  /// Struct to hold two coincident midas events
struct CoincMidasEvent {
	const dragon::MidasEvent* fGamma;
	const dragon::MidasEvent* fHeavyIon;
	CoincMidasEvent (const MidasEvent& event1, const MidasEvent& event2);
	~CoincMidasEvent() { }
};


} // namespace dragon

#endif // #ifndef DRAGON_MIDAS_EVENT_HXX

/*
	TID_BYTE   1
	TID_SBYTE  2
	TID_CHAR   3
	TID_WORD   4
	TID_SHORT  5
	TID_DWORD  6
	TID_INT    7
	TID_BOOL   8
	TID_FLOAT  9
	TID_DOUBLE 10
*/
