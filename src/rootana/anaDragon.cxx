/*!
 * \file anaDragon.cxx
 * \authors K. Olchanski
 * \authors G. Christian
 * \brief Defines main() program for dragon rootana analyzer.
 */
#include <signal.h>
#include "Callbacks.hxx"

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
