//
// ROOT analyzer
//
// K.Olchanski
//
// $Id: analyzer.cxx 64 2008-12-22 03:14:11Z olchansk $
//
#include <signal.h>
#include "RootanaDragon.hxx"

// Main function call

int main(int argc, char *argv[])
{
	setbuf(stdout,NULL);
	setbuf(stderr,NULL);
 
	signal(SIGILL,  SIG_DFL);
	signal(SIGBUS,  SIG_DFL);
	signal(SIGSEGV, SIG_DFL);

	rootana::App* app = new rootana::App("rootana", &argc, argv);
	app->Run();
	int ret = app->ReturnVal();
	app->Terminate(0);
	delete app;
	return ret;
}

//end
