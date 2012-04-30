#include <cassert>
#include <iostream>
#include "TStamp.hxx"
#include "user/User.hxx"

namespace { const uint64_t MAX32 = 4294967295; }

// ========= Struct tstamp::Event ========= //
tstamp::Event::Event(uint32_t tstamp_, int type_):
	type(type_)
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
	

/// Timestamp coincidence window
/// \todo make this something more settable, not a #define
#define COINC_WINDOW 10

// ========= Struct tstamp::Compare ========= //
bool tstamp::Compare::operator() (const tstamp::Event& lhs, const tstamp::Event& rhs)
{
	// Note: STL search algorithms define 'equivalency' as (!comp(a,b) && !comp(b,a)) == true
	// Thus we should return 'false' if the two events count as a match and otherwise something that 
	// won't always be true under commuation (e.g. lhs.tstamp < rhs.tstamp)
	if(lhs.type == rhs.type) { // Not a match - different types
		return lhs.tstamp < rhs.tstamp;
	}
	return (std::max(lhs.tstamp, rhs.tstamp) - std::min(lhs.tstamp, rhs.tstamp) < COINC_WINDOW) ?
		 false /* Match */ : lhs.tstamp < rhs.tstamp; /* Not a match - outside window */
}


// ========= Class tstamp::Queue ========= //

void tstamp::Queue::HandleCoinc(tstamp::Event& val, tstamp::Queue::Iterator& match)
{
	// Set gamma and heavy ion midas buffer pointers
	TMidasEvent *gammaMidasEvent = 0, *hionMidasEvent = 0;
	if(val.type == tstamp::Event::GAMMA) {
		gammaMidasEvent = &val.fMidasEvent;
		hionMidasEvent  = const_cast<TMidasEvent*>(&match->fMidasEvent);
	} else {
		gammaMidasEvent = const_cast<TMidasEvent*>(&match->fMidasEvent);
		hionMidasEvent  = &val.fMidasEvent;
	}

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
	
	// Remove match from the queue
	fContainer.erase(match);
}

void tstamp::Queue::HandleSingle(tstamp::Queue::Iterator& it)
{
	if(it->type == tstamp::Event::GAMMA) {
		rb::Event* gamma_event = rb::Event::Instance<GammaEvent>();
		gamma_event->Process(const_cast<TMidasEvent*>(&it->fMidasEvent), 0);
	}
	else if(it->type == tstamp::Event::HION) {
		rb::Event* hi_event = rb::Event::Instance<HeavyIonEvent>();
		hi_event->Process(const_cast<TMidasEvent*>(&it->fMidasEvent), 0);
	}
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
			std::cerr << "Warning in <tstamp::Queue::Push>: Tried to insert a duplicate timestampped event; "
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

	tstamp::Event event(1,1);
	TMidasEvent midasEvent;
	event.fMidasEvent = midasEvent;
	tstamp::Queue queue(100);

	queue.Push(event);

}

#elif 0
// ========= FOR TESTING ========= //

class EventGenerator {
public:
	 enum { GAMMA, HION, COINC };
private:
	 TRandom3* rng;
	 TFile* file;
	 TTree *tcoinc, *thion, *tgamma;
	 HiEvent fHion;
	 GammaEvent fGamma;
	 int type;
	 int64_t tsg, tsh;
public:
	 void Coinc();
	 void HeavyIon();
	 void Gamma();
	 void Choose();
	 void TSGen();
	 void Generate(HiEvent*&, GammaEvent*&);
	 EventGenerator(const char* filename, int seed = 12345);
	 ~EventGenerator();
};

EventGenerator::EventGenerator(const char* filename, int seed):
	rng(new TRandom3(seed)),
	file(new TFile(filename, "recreate")),
	tcoinc(new TTree("tgcoinc", "Generated Coincidence Tree")),
	thion( new TTree("tghion",  "Generated Heavy Ion Singles Tree")),
	tgamma(new TTree("tggamma", "Generated Gamma Singles Tree"))
{
	// Timestamp branches
	thion-> Branch("ts", &fHion.tstamp.tstamp, "ts/l");
	tgamma->Branch("ts", &fGamma.tstamp.tstamp, "ts/l");
	tcoinc->Branch("tsg", &fGamma.tstamp.tstamp, "tsg/l");
	tcoinc->Branch("tsh", &fHion.tstamp.tstamp, "tsh/l");

	// Parameter branches
	thion-> Branch("par", &fHion.par, "par/D");
	tgamma->Branch("par", &fGamma.par, "par/D");
	tcoinc->Branch("parg", &fGamma.par, "parg/D");
	tcoinc->Branch("parh", &fHion.par, "parh/D");
}

EventGenerator::~EventGenerator()
{
	thion->Write();
	tgamma->Write();
	tcoinc->Write();
	delete rng;
	delete file;
}

void EventGenerator::Choose()
{
	int n = rng->Integer(100);
	     if(n < 30)  type = COINC;
	else if(n < 80)  type = GAMMA;
	else             type = HION;
}

void EventGenerator::Gamma()
{
	type = GAMMA;
	fGamma.par = rng->Gaus(20, 5);
	tgamma->Fill();
}

void EventGenerator::HeavyIon()
{
	type = HION;
	GEN:
	fHion.par = rng->Landau(10,5);
	if(fHion.par > 200) goto GEN;
	thion->Fill();
}

void EventGenerator::Coinc()
{
	type = COINC;
	Gamma();
	HeavyIon();
	tcoinc->Fill();
}

void EventGenerator::TSGen()
{
	static int last = 1;
	switch(type) {
	case GAMMA:
		fGamma.tstamp.tstamp = last + gRandom->Integer(30);
		last = fGamma.tstamp.tstamp;
		break;
	case HION:
		fHion.tstamp.tstamp = last + gRandom->Integer(100);
		last = fHion.tstamp.tstamp;
		break;
	case COINC:
		fGamma.tstamp.tstamp = last + gRandom->Integer(30);
		fHion.tstamp.tstamp = fGamma.tstamp.tstamp + 3 + gRandom->Integer(10);
		last = fHion.tstamp.tstamp;
		break;
	default:
		assert(0); break;
	}		
}

void EventGenerator::Generate(HiEvent*& h, GammaEvent*& g)
{
	h = 0; g = 0;
	Choose();
	switch(type) {
	case COINC: Coinc(); h = &fHion; g = &fGamma; break;
	case GAMMA: Gamma(); g = &fGamma; break;
	case HION: HeavyIon(); h = &fHion; break;
	default: assert(0); break;
	}
	if(h) h->tstamp.type = tstamp::Event::HION;
	if(g) g->tstamp.type = tstamp::Event::GAMMA;
	TSGen();
}

int main() {

	/*
	EventGenerator evg("test.root");
	tstamp::Queue events(100);
	
	for(int i=0; i< 100000; ++i) {
		evg.Generate(gHionEvent, gGammaEvent);
		if(gGammaEvent) events.Push(gGammaEvent->tstamp);
		if(gHionEvent)  events.Push(gHionEvent->tstamp);
	}

	gCoinc->Write();
	gGamma->Write();
	gHion->Write();
*/

	dragon::Dragon fDragon;

}

/*
int main() {
	TRandom3 rng(100);

	tgen.Branch("coinc",&Coincg,"coinc/i");
	tgen.Branch("val",&Valg,"val/i");
	t.Branch("coinc",&Coinc,"coinc/i");
	t.Branch("val",&Val,"val/i");

	{
		tstamp::Queue events(100);
		int last = 1;
		for(int i=0; i< 100000; ++i) {
			bool coinc = rng.Uniform(0,100) < 33;
			Coincg = coinc;
			int num = rng.Uniform(last+1, last + 100);
			Valg = num;
			tgen.Fill();
			last = num;
			if(coinc) {
				events.Push( tstamp::Event(num, tstamp::Event::GAMMA) );
				events.Push( tstamp::Event(num, tstamp::Event::HION) );
			}
			else {
				bool which = gRandom->Uniform(0,10) < 5;
				if(which) events.Push( tstamp::Event (num, tstamp::Event::GAMMA) );
				else      events.Push( tstamp::Event (num, tstamp::Event::HION) );
			}
		}
	}

	tgen.Write();
	t.Write();
}
*/
#endif
