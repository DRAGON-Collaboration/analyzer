#include <cassert>
#include <iostream>
#include "TStamp.hxx"


// TTree *gCoinc = 0, *gGamma = 0, *gHion = 0;
// uint64_t gGammaTS, gHionTS;
// double gParG, gParH;
// GammaEvent* gGammaEvent;
// HiEvent* gHionEvent;

// void initTree()
// {
// 	static int ncalls(0); assert(ncalls++ == 0);
// 	gCoinc = new TTree("tcoinc", "Reconstructed Coincidence Tree");
// 	gGamma = new TTree("tgamma", "Reconstructed Gamma Singles Tree");
// 	gHion  = new TTree("thion",  "Reconstructed Heavy Ion Singles Tree");

// 	// Timestamp branches
// 	gHion-> Branch("ts",  &gHionTS,  "ts/l");
// 	gGamma->Branch("ts",  &gGammaTS, "ts/l");
// 	gCoinc->Branch("tsg", &gGammaTS, "tsg/l");
// 	gCoinc->Branch("tsh", &gHionTS,  "tsh/l");

// 	// Parameter branches
// 	gHion-> Branch("par",  &gParH, "par/D");
// 	gGamma->Branch("par",  &gParG, "par/D");
// 	gCoinc->Branch("parg", &gParG, "parg/D");
// 	gCoinc->Branch("parh", &gParH, "parh/D");
// }

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

bool tstamp::Event::operator== (const tstamp::Event& other)
{
	if(other.type == this->type) return false;

	if(other.tstamp > this->tstamp) {
		return other.tstamp - this->tstamp > 10;
	}
	else {
		return this->tstamp - other.tstamp > 10;
	}
}


// ========= Class tstamp::Queue ========= //

void tstamp::Queue::HandleCoinc(tstamp::Event val, tstamp::Queue::Iterator& match)
{
//	if(!gCoinc) initTree();
	assert(val.tstamp == match->tstamp);

//	HandleCoincidence(gHionEvent, gGammaEvent);

	fContainer.erase(match);
}

void tstamp::Queue::HandleSingle(tstamp::Queue::Iterator& it)
{
//	if(!gCoinc) initTree();

	if(it->type == tstamp::Event::GAMMA) {
//		assert(gGammaEvent);
//		HandleGamma(gGammaEvent);
	}
	else if(it->type == tstamp::Event::HION) {
//		assert(gHionEvent);
//		HandleHion(gHionEvent);
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

void tstamp::Queue::Push(tstamp::Event tstamp)
{
	tstamp::Queue::Iterator itMatch = fContainer.find(tstamp);
	if(itMatch != fContainer.end()) {
		HandleCoinc(tstamp, itMatch);
	}
	else {
		if(IsFull()) this->Pop();
		fContainer.insert(tstamp);
	}
}

void tstamp::Queue::Pop()
{
 	if(fContainer.empty()) return;
	tstamp::Queue::Iterator it = fContainer.begin();
	HandleSingle(it);
}

void tstamp::Queue::Cleanup()
{
	while(!fContainer.empty()) {
		Pop();
	}
}
	

#if 1

int main() { return 0; }

#else
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
