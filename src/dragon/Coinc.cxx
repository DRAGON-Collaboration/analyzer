/// \file Coinc.cxx
/// \author G. Christian
/// \brief Implements Coinc.hxx
#include "utils/Valid.hxx"
#include "Coinc.hxx"


// ========== Class dragon::Coinc ========== //

dragon::Coinc::Coinc():
	head(), tail()
{
	reset();
}

dragon::Coinc::Coinc(const dragon::Head& head, const dragon::Tail& tail)
{
	compose_event(head, tail);
}

void dragon::Coinc::reset()
{
	head.reset();
	tail.reset();
}

void dragon::Coinc::set_variables(const char* odb)
{
	/*!
	 * \param [in] odb_file Name of the ODB file (*.xml or *.mid) from which to read
	 * \note Passing \c "online" looks at the experiment's ODB, if connected
	 */
	head.set_variables(odb);
	tail.set_variables(odb);
}

void dragon::Coinc::compose_event(const dragon::Head& head_, const dragon::Tail& tail_)
{
	/*!
	 * \param [in] head_ Head (gamma-ray) event
	 * \param [in] tail_ Tail (heavy-ion) event
	 */
	head = head_;
	tail = tail_;
}

void dragon::Coinc::unpack(const midas::CoincEvent& coincEvent)
{
	/*!
	 * Calls the respective unpack() functions on the head and tail part
	 * of the coincidence events.
	 */
	head.unpack( *(coincEvent.fGamma) );
	tail.unpack( *(coincEvent.fHeavyIon) );
}

void dragon::Coinc::calculate()
{
	/*!
	 * Does head and tail calculations.
	 */
	head.calculate();
	tail.calculate();
}































// DOXYGEN MAINPAGE //

/*!
	\mainpage
	\authors G. Christian
	\authors C. Stanford
	\authors K. Olchanski

	\section intro Introduction

	This software package is developed for the analysis of data collected in DRAGON experiments
	at TRIUMF. The software is intended for experiments run with the "new" (c. 2013) DRAGON data-acquisition
	system (VME hardware + timestamp coincidence matching).

	At it's core, the package is simply a collection of routines to extract ("unpack") the
	<a href=http://midas.psi.ch>MIDAS</a> data
	generated during an experiment into a format that is more suitable for human consumption, as well as to
	perform	some of the more basic and commonly needed analysis tasks. From here, users can further process the
	data using their own codes, or they can interface with a vistualzation library to view the data in histograms,
	either online or offline. The core of the package is written in "plain" C++, that is without any dependence on
	libraries other than the standard library.

	In addition to the "core", this package also includes code to interface with the
	<a href=http://ladd00.triumf.ca/~olchansk/rootana/>rootana</a> data analysis package distributed as part of
	<a href=http://midas.psi.ch>MIDAS</a> and the
	<a href=http://trshare.triumf.ca/~gchristian/rootbeer/doc/html/index.html>rootbeer</a> program for data
	vistualzation. Each of these packages is designed for "user friendly" viewing of data in histograms, either
	from saved files or real-time online data. Both require that a relatively recent
	version of <a href=http://root.cern.ch>ROOT</a> be installed on your system, and, additionally,
	<a href=http://midas.psi.ch> MIDAS</a> must be installed in order to look at online data (offline
	data viewing should be possible without a MIDAS installation, however).
	
	There is also an optional extension to create python wrappers for the DRAGON classes, allowing data
	to be analyzed in python independent of ROOT/PyRoot. This package is somewhat in the experimental stage at
	the moment and requires installation of <a href=http://www.boost.org/doc/libs/1_51_0/libs/python/doc/>
	boost::python</a> to compile the libraries.


	\section installation Installation
	
	\subsection dependencies Dependencies
	For the "core" functionality, all that you will need is a working C++ compiler; there is no dependence on
	any third-party libraries. The optional rootbeer or rootana extensions each require ROOT to be installed
	(and, of course, the rootana and/or rootbeer packages themselves). To look at online data, you will
	need MIDAS installed, and, if using the rootana system,
	<a href=https://daq-plone.triumf.ca/SR/ROODY>roody</a> is required for online histogram viewing.

	The optional python wrapper extension requires <a href=http://boost.org>boost</a>, in particular the
	<a href=http://www.boost.org/doc/libs/1_51_0/libs/python/doc/>boost::python</a> library.

	\subsection download Download and Compile
	The latest version of the code can be downloaded from github:
	<a href=https://github.com/gchristian321/dragon_analyzer/tree/dev>
	https://github.com/gchristian321/dragon_analyzer/tree/dev</a>. You can either download the current version
	of the code as a .zip file, or if you are familiar with
	git, you can clone it into a local repository (note that you want to be on the 'dev' branch):
	\code
	git clone git@github.com:gchristian321/dragon_analyzer.git
	\endcode

	Before compiling, you may need to customize the Makefile to work on your system. If you open the file,
	you will see a section at the top where you can customize things. There are comments explaining what each of
	the optional features are and how you can change them.

	One you have set up the makefile as appropriate for your system, compiling should be as simple as typing
	\code
	make
	\endcode
	at the command line.  This will install the generated binaries in your local directory (shared libraries in
	$PWD/lib, binaries in $PWD). There is no "install" option, so you should either copy/link the binaries and
	libraries into a system-wide directory or update the necessary environment variables to allow your shell
	to find what it needs.

	If you run into any problems compiling this package, do not hesitate to contact the package maintainer
	for help. If do you find a solution to your problems on your own, please alert the package maintainer
	and describe 1) your problem; 2) what you did to fix it. The intent is for installion/compilation to be
	a very quick and painless process for everyone using this package, so any problems and solutions that you
	discover should be reflected in the standard version of the codes.

	\subsection pyext Python Extension
	If you wish to compile or use the optional (and somewhat experimental) python extension, you will first
	need to install the boost::python library. For information on this, see:
	<a href=http://www.boost.org/doc/libs/1_51_0/libs/python/doc/>http://www.boost.org/doc/libs/1_51_0/libs/python/doc/</a>.
	The python libraries rely on code generated by <a href=http://sourceforge.net/scm/?type=svn&group_id=118209>Py++</a>,
	which parses the source files and generates code to be compiled into the python libraries. It should not be necessary
	to generate the source code yourself using Py++, as long as the repository is kept up-to-date; however, if you make
	changes to the dragon analyzer sources, re-generation of the Py++ sources will be necessary in order to safely use
	the python libraries.

	To compile the python extension, simply cd into the py++ directory and type 'make', \e after having compiled
	the core dragon library. To be able to use the extensions from any directory, add the following to your bash
	startup script:
	\code
	export PYTHONPATH=${PYTHONPATH}:"/path/to/your/dragon/analyzer/py++/"
	export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:"/path/to/your/dragon/analyzer/py++/"
	\endcode
	and re-start your bash session or re-source the startup script. Now from a python session or script, you
	can load the generated modules:
	\code
	import vme
	import dragon
	\endcode

	\section using For Users



	\section developers For Developers

	This section just gives a brief overview behind the package's design and some suggestions for future
	maintainers/developers. For detailed documentation, see the related Doxygen documentation linked from the top
	of this page.
	
	\subsection design Design


	\subsection doc Documentation

	Given the fast turnover rate of developers and the common requirement for "everyday" users (whose level of expertise
	in the source language ranges from beginner to expert) to understand and modify source code,
	maintaining quality code documentation is essential for a physics software package to stay maintainable.

	As you may have noticed, the present source code has been extensively documented using <a href=http://doxygen.org>
	Doxygen</a>.  I don't claim that the present documentation is perfect by any means, but I hope it is at least a
	large step in the right direction. If you happen to extend or modify the code base, \e please carefully document
	anything that you do. If you are not familiar with Doxygen, it's quite intuitive and quick to learn - just follow
	the example of existing	code and/or refer to the manual at the <a href=http://doxygen.org>Doxygen website</a>.


 */
