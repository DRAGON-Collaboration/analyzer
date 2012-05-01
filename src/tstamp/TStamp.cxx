#include <cassert>
#include <iostream>
#include "TStamp.hxx"
#include "user/User.hxx"
#include "utils/Error.hxx"

namespace { const uint64_t MAX32 = 4294967295; }

// ========= Struct tstamp::Event ========= //
tstamp::Event::Event(uint32_t tstamp_, int type_, TMidasEvent event_):
	type(type_), fMidasEvent(event_)
{
	static uint64_t last = 0;
	if(tstamp_ >= last) {
		tstamp = (uint64_t)tstamp_;
	}
	else { // rollover
		tstamp = tstamp_ + MAX32;
	}
	last = tstamp;
}

tstamp::Event::~Event()
{
	// std::cout << "~tstamp::Event::~Event()\n";
}

void tstamp::Event::Copy(const tstamp::Event& other)
{
	type = other.type;
	tstamp = other.tstamp;
	fMidasEvent = other.fMidasEvent;
}


/// Timestamp coincidence window
/// \todo make this something more settable, not a #define
#define COINC_WINDOW 10

// ========= Struct tstamp::Compare ========= //

bool tstamp::Compare::operator() (const tstamp::Event& lhs, const tstamp::Event& rhs)
{
	// Note: STL search algorithms define 'equivalency' as (!comp(a,b) && !comp(b,a)) == true
	// Thus we should return 'false' if the two events count as a match and otherwise something that 
	// won't always be true under commuation (e.g. lhs.tstamp < rhs.tstamp)
	if(lhs.type == rhs.type) { // Not a match - same types
		return lhs.tstamp != rhs.tstamp ? lhs.tstamp < rhs.tstamp : true;
	}
	return (std::max(lhs.tstamp, rhs.tstamp) - std::min(lhs.tstamp, rhs.tstamp) < COINC_WINDOW) ?
		 false /* Match */ : lhs.tstamp < rhs.tstamp; /* Not a match - outside window */
}


// ========= Class tstamp::Queue ========= //

void tstamp::Queue::HandleCoinc(tstamp::Event& val, tstamp::Queue::Iterator& match)
{ 
	// Set gamma and heavy ion midas buffer pointers
	TMidasEvent *gammaMidasEvent = 0, *hionMidasEvent = 0;
	if(0);
	else if(val.type == tstamp::Event::GAMMA && match->type == tstamp::Event::HION) {
		gammaMidasEvent = &val.fMidasEvent;
		hionMidasEvent  = const_cast<TMidasEvent*>(&match->fMidasEvent);
	}
	else if(val.type == tstamp::Event::HION && match->type == tstamp::Event::GAMMA) {
		gammaMidasEvent = const_cast<TMidasEvent*>(&match->fMidasEvent);
		hionMidasEvent  = &val.fMidasEvent;
	}
	else {
		err::Error("tstamp::Queue::HandleCoinc")
			 << "Invalid coincidence event types: val.type = " << val.type << ", match->type = " << match->type
			 << ". Valid arguments should be either " << tstamp::Event::GAMMA << " (gamma event) or "
			 << tstamp::Event::HION << " (heavy ion event), with val.type != match->type. Skipping the events "
			 << "in question." << ERR_FILE_LINE;
	}

	if(gammaMidasEvent && hionMidasEvent) {
		// Unpack each event into singles tree
		rb::Event* gamma_event = rb::Event::Instance<GammaEvent>();
		gamma_event->Process(gammaMidasEvent, 0);

		rb::Event* hi_event = rb::Event::Instance<HeavyIonEvent>();
		hi_event->Process(hionMidasEvent, 0);
		
		// Unpack into coincidence tree
		rb::Event* coinc_event = rb::Event::Instance<CoincidenceEvent>();
		CoincEventPair_t coinc =
			 std::make_pair(static_cast<GammaEvent*>(gamma_event), static_cast<HeavyIonEvent*>(hi_event));
		coinc_event->Process(&coinc, 0);
	}
	
	// Remove match from the queue
	fContainer.erase(match);
}

void tstamp::Queue::HandleSingle(tstamp::Queue::Iterator& it)
{
	// Figure out event type & unpack
	switch(it->type) {
	case tstamp::Event::GAMMA :
		 {
			 rb::Event* gamma_event = rb::Event::Instance<GammaEvent>();
			 gamma_event->Process(const_cast<TMidasEvent*>(&it->fMidasEvent), 0);
			 break;
		 }
	case tstamp::Event::HION :
		 {
			 rb::Event* hi_event = rb::Event::Instance<HeavyIonEvent>();
			 hi_event->Process(const_cast<TMidasEvent*>(&it->fMidasEvent), 0);
			 break;
		 }
	default:
		 {
			 err::Error("tstamp::Queue::HandleSingle")
					<< "Unrecognized tstamp::Event::type: " << it->type << ". Recognized types are "
					<< tstamp::Event::GAMMA << " (gamma event) and " << tstamp::Event::HION << " (heavy ion event). "
					<< "Skiping the event in question." << ERR_FILE_LINE;
			 break;
		 }
	}
	// Remove the event in question from the queue
	fContainer.erase(it);
}

tstamp::Queue::Queue(int size):
	fMaxSize(size) { }

bool tstamp::Queue::IsFull()
{
	if(fContainer.empty()) return false;
	tstamp::Queue::Iterator first = fContainer.begin(), last = fContainer.end();
	--last;
	return fMaxSize <= last->tstamp - first->tstamp;
}

tstamp::Queue::Iterator tstamp::Queue::CheckMatch(const tstamp::Event& event,
																									tstamp::Queue::Iterator first, tstamp::Queue::Iterator last)
{
	tstamp::Compare comp;
	tstamp::Queue::Iterator it =
		 std::lower_bound<tstamp::Queue::Iterator, tstamp::Event, tstamp::Compare> (first, last, event, comp);
	if(it != last && (!comp(*it, event) && !comp(event, *it))) {
		return it;
	}
	else {
		return last;
	}	
}

void tstamp::Queue::Push(tstamp::Event& event)
{
	tstamp::Queue::Iterator itMatch = CheckMatch(event);
	if(itMatch != fContainer.end()) {
		HandleCoinc(event, itMatch);
	}
	else {
		if(IsFull()) {
			this->Pop();
		}
		std::pair<tstamp::Queue::Iterator, bool> result = fContainer.insert(event);
		if(!result.second) { // Duplicate
			std::cerr << "Warning in <tstamp::Queue::Push>: Tried to insert a duplicate timestamped event; "
								<< "skipping the second one.\n"
								<< "Timestamp value: " << event.tstamp << ", type: " << event.type << "\n";
		}
	}
}

void tstamp::Queue::Pop()
{
 	if(fContainer.empty()) return;
	tstamp::Queue::Iterator itFirst = fContainer.begin(), itSecond = itFirst;
	++itSecond;
	tstamp::Queue::Iterator itMatch = CheckMatch(*itFirst, itSecond, this->Last());
	if(itMatch == this->Last()) {
		HandleSingle(itFirst);
	}
	else {
		std::cerr << "Warning in <tstamp::Queue::Pop()>: Found a match - this shouldn't hapen, no!?\n" 
							<< "Erasing both elements - "
							<< "First.ts, First.type, Match.ts, Match.type: "
							<< itFirst->tstamp << ", " << itFirst->type << ", " << itMatch->tstamp << ", " << itMatch->type << "\n";
		fContainer.erase(itFirst);
		fContainer.erase(itMatch);
	}
}

void tstamp::Queue::Cleanup()
{
	while(!fContainer.empty()) {
		Pop();
	}
}
	

#if defined TEST

int main() {

	TMidasFile fFile;
	TMidasEvent fBuffer;
	assert(fFile.Open("/Users/gchristian/soft/develop/dragon/analyzer/run00068.mid"));
	tstamp::Queue fTSQueue(100);

	while(1) {
		fBuffer.Clear();
		bool result = fFile.Read(&fBuffer);
		if(!result) break;

		Short_t eventId = fBuffer.GetEventId();
		if(eventId == DRAGON_EVENT) {
			
			static uint64_t fakeTS = 1;
			tstamp::Event event(fakeTS++,1);
			event.fMidasEvent = fBuffer;
			
			fTSQueue.Push(event);
		}
		else ;
	}
}

#endif
