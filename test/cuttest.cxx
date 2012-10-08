#include <cassert>
#include <TROOT.h>
#include <iostream>
#include <fstream>

#include "Base.h"

void f(bool is_new)
{
	if (is_new)
	{
		printf("creating\n");
		std::ofstream ofs ("test.C");
		ofs << "#include \"Base.h\"\n";
		ofs << "struct Derived: public Base { void f() { printf(\"Derived!\\n\"); } };\n";
	}

	gROOT->ProcessLine(".L test.C+");

	Base* b = (Base*)gROOT->ProcessLineFast("new Derived()");

	assert (b);

	b->f();
}
