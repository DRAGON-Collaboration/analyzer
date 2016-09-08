#include <cstdio>
#include <cassert>
#include <TTree.h>
#include <TFile.h>
#include <TH1F.h>
#include <TTreeFormula.h>
#include "Dragon.hxx"

TFile fFile ("test.root", "recreate");
TTree fTree ("t", "TEST");
dragon::Coinc coinc;
void* coincAddr = &coinc;
TH1F hst("hst", "", 100, -100, 4100);
TTreeFormula* fFormula;

void fill1()
{
	hst.Fill (coinc.head.bgo.esort[0]);
}

void fill2()
{
	hst.Fill ( fFormula->EvalInstance() );
}

typedef void (*fill_t)();

int main(int argc , char** argv)
{
	fill_t fillfunc;
	if (argc == 1 || argv[1][0] == '0') {
		fillfunc = fill1;
		printf ("using fill1()...\n");
	}
	else {
		fillfunc = fill2;
		printf ("using fill2()...\n");
	}

	fTree.SetCircular(0);
	fTree.Branch("coinc","dragon::Coinc",&coincAddr);
	fFormula = new TTreeFormula("formula", "coinc.head.bgo.q[0]", &fTree);
	assert (fFormula->GetNdim());

	int nnn = 0;
  do {
		fillfunc();
  } while (++nnn < 10000000);

	printf ( "hst.GetEntries() = %f\n", hst.GetEntries() );
	
	delete fFormula;

}
