/*!
 * \file anaDragon.cxx
 * \authors K. Olchanski
 * \authors G. Christian
 * \brief Defines main() program for dragon rootana analyzer.
 */
#include <signal.h>
#include <memory>
#include <string>
#include <iostream>
#include "utils/Functions.hxx"
#include "Callbacks.hxx"


int main(int argc, char *argv[])
{
	setbuf(stdout,NULL);
	setbuf(stderr,NULL);
 
	signal(SIGILL,  SIG_DFL);
	signal(SIGBUS,  SIG_DFL);
	signal(SIGSEGV, SIG_DFL);

	std::auto_ptr<rootana::App> app (new rootana::App("rootana", &argc, argv));
	app->Run();
	int ret = app->ReturnVal();
	return ret;
}
