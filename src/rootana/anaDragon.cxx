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


bool check_public()
{
	std::string priv = DRAGON_UTILS_STRINGIFY(PRIVATE);
	bool ret = true;

	if(priv != "public") {
		std::cerr << "Error: To use the histogram parser, anaDragon must be compiled with -DPRIVATE=public\n"
							<< "Please change the appropriate lines near the top of your Makefile and re-compile.\n\n";
		ret = false;
	}

	return ret;	
}
	

int main(int argc, char *argv[])
{
	if(!check_public())
		return 1;

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
