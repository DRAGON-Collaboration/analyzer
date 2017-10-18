///
/// \file Unpack.hxx
/// \author G. Christian
/// \brief Defines generic unpacking functions for different types of DRAGON events.
///
#ifndef DRAGON_UNPACK_HXX
#define DRAGON_UNPACK_HXX
#include <vector>
#include <memory>
#include "midas/Event.hxx"
#include "TStamp.hxx"


namespace dragon {
class Head;
class Tail;
class Coinc;
class Epics;
class Scaler;
class RunParameters;
}

namespace dragon {

///
/// Handles unpacking event data
class Unpacker {
public:
	///
	/// Sets pointers to container classes, initializes fQueue
	Unpacker(dragon::Head* head,
					 dragon::Tail* tail,
					 dragon::Coinc* coinc,
					 dragon::Epics* epics,
					 dragon::Scaler* schead,
					 dragon::Scaler* sctail,
					 dragon::Scaler* scaux,
					 dragon::RunParameters* runpar,
					 tstamp::Diagnostics* tsdiag,
					 bool singlesMode = false);
	///
	/// Nothing to do, all cleanup is automatic
	~Unpacker();
	///
	/// Clear vector of unpacked codes
	void ClearUnpackedCodes();
	///
	/// Flush the timestamp queue
	void FlushQueue(int flushTime = -1);
	///
	/// Flush the timestamp queue, for only a single event
	size_t FlushQueueIterative();
	///
	/// Return a pointer to fQueue
	tstamp::Queue* GetQueue() const;
	///
	/// Returns true if we are in singles mode
	bool IsSinglesMode() const;
	///
	/// Returns the length of the coincidence window in microseconds
	double GetCoincWindow() const;
	///
	/// Returns the queue buffering time in seconds
	double GetQueueTime() const;
	///
	///  Returns the event codes of unpacked events
	std::vector<int32_t> GetUnpackedCodes() const;
	///
	/// Perform actions at the beginning of a run
	void HandleBor(const char* dbname);
	///
	/// Process function to handle singles events popped from the queue
	void Process(const midas::Event& event);
	///
	/// Process function to handle coincidence events popped from the queue
	void Process(const midas::Event& event1, const midas::Event& event2);
	///
	/// Process function for timestamp diagnostics [empty]
	void Process(tstamp::Diagnostics*);
	///
	/// Switch over to coincidence mode
	void SetCoincMode();
	///
	/// Set the coincidence matching window
	void SetCoincWindow(double window);
	///
	/// Put the unpacker in singles mode
	void SetSinglesMode(int qFlush = -1);
	///
	/// Set the queue buffering time
	void SetQueueTime(double t);
	///
	/// Unpack a head event into fHead
	void UnpackHead(const midas::Event& event);
	///
	/// Unpack a tail event into fTail
	void UnpackTail(const midas::Event& event);
	///
	/// Unpack a coincidence event into fCoinc
	void UnpackCoinc(const midas::CoincEvent& event);
	///
	/// Unpack a head scaler event into fHeadScaler;
	void UnpackHeadScaler(const midas::Event& event);
	///
	/// Unpack a tail scaler event into fTailScaler
	void UnpackTailScaler(const midas::Event& event);
	///
	/// Unpack a aux scaler event into fAuxScaler;
	void UnpackAuxScaler(const midas::Event& event);
	///
	/// Unpack an EPICS event into fEpics
	void UnpackEpics(const midas::Event& event);
	///
	/// Unpack run parameters into fRunpar
	void UnpackRunParameters(const midas::Database& db);
	///
	/// Unpack a generic midas event (from full data buffer)
	std::vector<int32_t> UnpackMidasEvent(char* databuf);
	///
	/// Unpack a generic midas event (from header + data)
	std::vector<int32_t> UnpackMidasEvent(void* header, char* data);

private:
	/// Default queue time in seconds
	static const int kQueueTimeDefault = 4;
	/// Default coincidence window in usec
	static const int kCoincWindowDefault = 10;
	/// Size of the coincidence window in microseconds (defaults to 10)
	double fCoincWindow;
	///	Timestamp queue for coincidence matching
	std::auto_ptr<tstamp::Queue> fQueue;
	/// Container of event codes of unpacked events
	std::vector<int32_t> fUnpacked;
	/// Pointer to _external_ head class
	dragon::Head* fHead;
	/// Pointer to _external_ tail class
	dragon::Tail* fTail;
	/// Pointer to _external_ coinc class
	dragon::Coinc* fCoinc;
	/// Pointer to _external_ epics class
	dragon::Epics* fEpics;
	/// Pointer to _external_ scaler class (for head data)
	dragon::Scaler* fHeadScaler;
	/// Pointer to _external_ scaler class (for tail data)
	dragon::Scaler* fTailScaler;
	/// Pointer to _external_ scaler class (for aux data)
	dragon::Scaler* fAuxScaler;
	/// Pointer to _external_ run parameters class
	dragon::RunParameters* fRunpar;
	/// Pointer to _external_ timestamp diagnostics class
	tstamp::Diagnostics* fDiag;
};

} // namespace dragon


inline bool dragon::Unpacker::IsSinglesMode() const
{
	return !(fQueue.get());
}

inline double dragon::Unpacker::GetCoincWindow() const
{
	return fCoincWindow;
}

inline double dragon::Unpacker::GetQueueTime() const
{
	return fQueue->GetMaxDelta() / 1e6;
}

inline void dragon::Unpacker::SetCoincWindow(double window)
{
	/// \note Default value at construction is 10 usec.
	fCoincWindow = window;
}

inline void dragon::Unpacker::SetQueueTime(double t)
{
	/// \note Default value at construction is 4 sec.
	fQueue->SetMaxDelta(t*1e6);
}

inline void dragon::Unpacker::SetSinglesMode(int qFlush)
{
	/// Any incoming events after this call are processed as singles only.
	/// If there are events in the queue, they are first flushed.
	/// \param qFlush Maximum flush time for events in the queue. Specifying a
	///  0 skips flushing entirely and throws away any events in the queue. Negative
	///  numbers cause the entire queue to be flushed.
	if(qFlush) FlushQueue(qFlush);
	fQueue.reset(0);
}

inline void dragon::Unpacker::SetCoincMode()
{
	/// If in singles mode when called, it switches over to coincidence
	/// mode with default coincidence window and queue time. To change
	/// these values, use SetCoincWindow() and SetQueueTime().
	///
	/// If we are already in coincidence mode, this has no effect.
	if(!IsSinglesMode())
		fQueue.reset(new tstamp::OwnedQueue<Unpacker>(kQueueTimeDefault*1e6, this));
}

inline std::vector<int32_t> dragon::Unpacker::UnpackMidasEvent(char* databuf)
{
	/// Forward all work to UnpackMidasEvent(void*, char*)
	return UnpackMidasEvent(databuf, databuf + sizeof(midas::Event::Header));
}

inline void
dragon::Unpacker::ClearUnpackedCodes()
{
	/// Needed when the unpack routines are called implicitly, for example
	/// during a queue flush which calls Process(), not UnpackEvent()

	fUnpacked.clear();
}

inline std::vector<int32_t>
dragon::Unpacker::GetUnpackedCodes() const
{
	/// Whenever a call to UnpackMidasEvent() is made, an
	/// internal vector<int32_t> is filled with the event codes
	/// corresponding to those event types which were unpacked into
	/// a class structure. This function allows the caller to see what
	/// those event codes are and, e.g. fill ROOT trees, ntuples, histograms,
	/// etc. as appripriate.

	return fUnpacked;
}

inline tstamp::Queue*
dragon::Unpacker::GetQueue() const
{
	return fQueue.get();
}

#endif
