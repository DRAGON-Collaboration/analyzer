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
	import midas
	import vme
	import dragon
	\endcode

	You can use the python 'help' function (<tt>help dragon</tt>, etc) to see the classes and functions
	available in each module.

	\section using For Users

	\subsection rootana Rootana Online Analyzer

	This package includes a library to interface the DRAGON analysis codes with MIDAS's
	<a href=http://ladd00.triumf.ca/~olchansk/rootana/>rootana</a> system,
	which allows visualization of online and offline data in histograms, in. If you are familiar with the "old"
	DRAGON analyzer, this is essentially the same thing, but with a few added features. To use this analyzer,
	simply compile with the "USE_ROOTANA" option set to "YES" in the Makefile (of course, for this to work,
	you will need to have the corresponding MIDAS packages installed on your system). Then run the 'anaDragon'
	executable, either from a shell or by starting it on the MIDAS status page.  To see a list of command line option
	flags, run with the \c -h flag. To view histograms, you will
	need to run the 'roody' executable, which should be available on your system if you have MIDAS installed.

	As mentioned, there are a couple feature addidions since the previous version of the DRAGON analyzer. The main
	one is the ability to define histograms and cuts at runtime instead of compile time. The hope is that this will
	allow for easy and quick changes to be made to the available visualization, even for users who are completely
	inexperienced with C++.

	Histograms are created at runtime (\c anaDragon program start) by parsing a text file that contains histogram
	definitions. By default, histograms are created from the file <tt>${DR_HOME}/histos.dat</tt>, where \c ${DR_HOME}
	is the location of the DRAGON analyzer package. However, you can alternatively specify a different histogram
	definition file by invoking the \c -histos flag at program start:
	\code
	./anaDragon -histos my_histograms.dat
	\endcode

	Note that the default histogram file (or the one specified with the \c -histos flag, creates histograms that
	are available both online and offline, that is, they are viewable online using the roody interface, and they
	are also saved to a .root file at the end of the run. It is also possible to specify a separate set of histograms
	for online viewing \e only. To do this, invoke the \c -histos0 flag:
	\code
	./anaDragon -histos0 histograms_i_only_care_to_see_online.dat
	\endcode

	Now that you know how to specify what file to read the histogram definitions from, you should probably also
	know how to write said file. The basic way in which the files are parsed is by using a set of "key" codes
	that tell the parser to look in the lines below for the appropriate information for defining the corresponding
	ROOT object. The "keys" available are as follows:
	
	-# DIR: Create a directory.\n\n
	The succeding line should contain the full path of the directory as plain text.\n
	For example:
  \code
  DIR:
  histos/gamma
  \endcode
  This would create a new directory "histos", with "gamma" as a subdirectory. Note that if you later do
  \code
  DIR:
  histos/hion
  \endcode
  the "histos" directory would not be re-created; instead, "hion" would be added as a new subdirectory. When
  you specify a directory, any histogram definitions succeding it will be members of that directory, until another
  directory line is encountered. If any histograms are defined before the first "DIR:" command, they will belong to
  the "top level" directory (e.g. in a TFile, their owning directory will be the TFile itself).
	\n\n
	-# TH1D: Create a 1d histogram.\n\n
	Should be succeded by two lines: 1) the argument to a TH1D constructor, and 2)
	the parameter to display in the histogram.\n
	Example:
	\code
  TH1D:
        ("bgo_e0", "Bgo energy, channel 0 [singles]", 256, 0, 4096)
        rootana::gHead.bgo.ecal[0]
	\endcode
	(note that the indentations are only for readability, not required). This creates a histogram with name "bgo_e0",
	title "Bgo energy, channel 0 [singles]", and ranging from 0 to
	4096 with 256 bins. The parameter to be displayed in the histogram would be rootana::gHead.bgo.ecal[0].
	\n\n
	-# TH2D: Create a 2d histogram.\n\n
	Succeding lines are in the same sprit as the 1d case, except a third line is
	aded to specify the y-axis parameter.\n
	Example:
	\code
  TH2D:
      ("bgo_e1_e0", "Bgo energy 1 vs. 0 [coinc]", 256, 0, 4096, 256, 0, 4096)
      rootana::gCoinc.head.bgo.ecal[1]
      rootana::gCoinc.head.bgo.ecal[0]
	\endcode
	Creates a histogram of rootana::gCoinc.head.bgo.ecal[1] [x-axis] vs. rootana::gCoinc.head.bgo.ecal[0] [y-axis].
	\n\n
	-# TH3D Create a 3d histogram.\n\n
	Following the same pattern as 1d and 2d - add a third line to specify the
	z-axis parameter.
	\n\n
	-# SUMMARY: Create a "summary" histogram, that is a histogram which displays information on multiple channels
	at once, like this:\n\n
	\image html summary.png
	\n\n
	Note that sumary histograms are currently only available for arrays, that is, where each y-axis bin corresponds
	to a different array index. If you need to display summary information for parameters not contained in an array,
	you will have to define the histogram manually in C++. However, in nearly all cases where summary imformation
	might be desired, the parameters are already contained in an array anyway.
	\n\n
	The lines following a "SUMMARY:" command are 1) Constructor for a 1d histogram; this will define the x-axis
	binning and set the histogram name/title; 2) The name of an array that you want to display in the histogram;
	and 3) The number of y-axis bins (should be equal to the length of the array).\n
	Example:
	\code
  SUMMARY:
      ("bgo_q", "Bgo energies [singles]", 256, 0, 4096)
      rootana::gHead.bgo.ecal
      30
	\endcode
	This would create a summary histogram of the 30 BGO detector energies contained in the rootana::gHead.bgo.ecal
	array (bgo.ecal[0] -> bgo.ecal[29]).
	\n\n
	-# CUT: Defines a cut.\n\n
	This defines a "cut" or "gate" condition that will be applied to the histogram defined directly before it.
	The cut condition is specified as a logical condition consisting of rootana::Cut derived classes. For more information
	on how to define a cut, see the code documentation of the Cut.hxx source file and links therein.\n
	As a simple example:
	\code
	TH1D:
      ("bgo_e0", "Bgo energy, ch 0 [singles]", 256, 0, 4096)
      rootana::gHead.bgo.ecal[0]
  CUT:
      Less(gHead.bgo.ecal[0], 2000) && Greater(gHead.bgo.ecal[0], 100)
  \endcode
	will create the "bgo_e0" histogram displaying bgo.ecal[0], with the condition that bgo.ecal[0] be greater-than
	100 and less-than 2000.\n\n
	Note that you can also create/use 2d polygon cuts, either by specifying the parameters and (closed) polygon
	points:
	\code
	CUT:
      Cut2D(gHead.bgo.q[0], gHead.bgo.q[1], -2,-2, -2,2, 2,2, 2,-2, -2,-2)
  \endcode
	This would create the following cut:\n\n
	\image html cut.png
	It is also possible to pre-define and then re-use graphical cut using the "CMD:" option (see next).
	\n\n
	-# CMD: Evaluates a series of commands in CINT.\n\n
	The succeeding lines are a series of C++ statements to be evaluated literally in the CINT interpreter, closed
	by a line containing only "END".
	Any objects created within these statements will become available for use in future commands throughout
	the definition file. Note that each file should only contain one "CMD:" statement (if it contains any), and
	that it should be the first "active" code within the file.\n
	As an example, we could use "CMD:" to define a graphical cut:
	\code
  CMD:
      TCutG cutg("cutTest",7);
      cutg.SetVarX("");
      cutg.SetVarY("");
      cutg.SetTitle("Graph");
      cutg.SetFillColor(1);
      cutg.SetPoint(0,37.8544,70.113);
      cutg.SetPoint(1,25.5939,41.5819);
      cutg.SetPoint(2,54.9042,28.0226);
      cutg.SetPoint(3,83.0651,35.3672);
      cutg.SetPoint(4,81.5326,53.1638);
      cutg.SetPoint(5,59.3103,79.1525);
      cutg.SetPoint(6,37.8544,70.113);
  END
	\endcode
	This cut is then available in future satements; for example to create a Cut2D object to be applied to a histogram
	\code
  SUMMARY:
      ("bgo_ecal", "Bgo energies [singles]", 256, 0, 4096)
       rootana::gHead.bgo.ecal
      30
  CUT:
      Cut2D(gHead.bgo.ecal[0], gHead.bgo.ecal[1], cutg)
  \endcode
  The above example would apply the graphical cut "cutg" defined in the "CMD:" statement to the "bgo_ecal" histogram.

	With combinations of the above commands, you should hopefully be able to define any histogram-related objects you'll
	need in the online analyzer without ever having to touch the source code. A few more notes about histogram definition
	files:

	- Whitespace is ignored, though good indention improves readability immensely.
	\n\n
	- The \c # character denotes a comment, all characters on the same line coming after a \c # are ignored:
	\code
	# IGNORE THIS HEADER STATEMENT #
	TH1D: #Ignore this descriptive comment also...
	\endcode
	\n\n
	- The runtime parsing is done using CINT via the gROOT->ProcessLineFast command. This has very little native
	error handling. Some support has been added in the DRAGON analyzer to "gracefully" handle common errors, for
	example skipping the current definition and moving onto others, but alerting the user. If you find a case where
	you feel the error handling could be improved (in particular, where a mistake in the script file causes a crash
	or un-reported failure), do not hesitate to alert the developers.

	Once started, the online analyzer simply runs in the background to receive, match, and analyze events coming
	from the two separate frontends. The analyzed data are then summarized in the histograms requested by the user
	as outlined in the proceeding paragraphs. As menetioned, roody can be used to visualize the histograms as data
	is coming in. Roody is a graphical program, and as such it's use is fairly intuituve

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
