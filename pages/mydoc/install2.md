<!-- # __Installation__ -->

<!-- This section gives an overwiew of how to obtain and install the latest version of the DRAGON data analysis package. -->

<!-- --- -->

<!-- ```diff -->
<!-- ++note: -->
<!-- ``` -->
<!-- These instructions assume that you will install the dragon analyzer package in `~/packages/dragon`. If not, it will be necessary to alter the paths given below accordingly.  -->

<!-- --- -->

<!-- ## __Dependencies__ -->

<!-- For the "core" functionality, all that you will need is a working C++ compiler; there is no dependence on any third-party libraries. However, you will likely want to make use of the analysis package in ROOT, in which case you will need a fairly recent installation of ROOT on your system. For instructions on installing ROOT, go [here](http://root.cern.ch/drupal/content/downloading-root). It is suggested that you install a relatively recent version of ROOT (say, &ge; 5.30) to ensure combatibilty between the ROOT release and the package code. You will also need to be sure that the `$ROOTSYS` environment variable is set and that `$ROOTSYS/bin` is in your search path. This is typically accomplished by sourcing the `thisroot.sh` or `thisroot.csh` script in `$ROOTSYS/bin` from your startup script (e.g. - `~/.bashrc`). -->

<!-- --- -->

<!-- ```diff  -->
<!-- --Attention:  -->
<!-- ``` -->
<!-- __Ubuntu users:__ compiling the DRAGON analyzer package evidently requires `clang` (`sudo apt-get install clang clang++`). -->

<!-- --- -->

<!-- ```diff -->
<!-- ++note: -->
<!-- ``` -->
<!-- The DRAGON analyzer package is now compatible with ROOT6, but ROOT6 functionality should be considered beta until it has been tested more rigorously. Please report bugs to [dconnolly@triumf.ca](mailto:dconnolly@triumf.ca) or [file an issue here](https://github.com/DRAGON-Collaboration/analyzer/issues). -->

<!-- --- -->

<!-- The optional rootbeer or rootana extensions each require ROOT to be installed (and, of course, the rootana and/or rootbeer packages themselves). To look at _online_ data, you will need MIDAS installed, and, if using the rootana system, [roody](https://bitbucket.org/tmidas/roody/src/master/) is required for online histogram viewing. -->

<!-- ## __Download and Compile__ -->

<!-- One may obtain the analysis package from the [git repository](https://github.com/DRAGON-Collaboration/analyzer) as follows: -->

<!-- ``` -->
<!-- mkdir -p ~/packages/dragon -->
<!-- cd ~/packages/dragon -->
<!-- git clone https://github.com/DRAGON-Collaboration/analyzer -->
<!-- cd analyzer -->
<!-- ``` -->

<!-- If you do not have git installed or prefer to download a tarball, then visit https://github.com/DRAGON-Collaboration/analyzer, click on the [releases link](https://github.com/DRAGON-Collaboration/analyzer/releases) and select the latest release (or an earlier one if you have reason to do so). Click on either the "zip" or the "tar.gz" link to download a zip file or a tarball containing the code. -->

<!-- Once the package is obtained, it is necessary to run a configure script in order to set the appropriate environment variables before compiling. To see a list of options, type: -->

<!-- ``` -->
<!-- ./configure --help -->
<!-- ``` -->

<!-- In most cases, one may simply run -->

<!-- ``` -->
<!-- ./configure -->
<!-- ``` -->

<!-- At this point, compilation _should_ be as simple as typing `make`. If not, [please let me know](mailto:dconnolly@triumf.ca), and we can try to fix the problem permanently so others do not run into it. -->

<!-- Compilation creates a shared library `lib/libDragon.so` (and `lib/libDragon.so.dSYM` if installed on a Mac) as well as an executable `bin/mid2root`. The executable converts MIDAS files to ROOT trees. The library can be loaded into an interactive ROOT session by issuing the following commands: -->

<!-- ``` -->
<!-- root[] .include ~/packages/dragon/analyzer/src -->
<!-- root[] gSystem->Load("~/packages/dragon/analyzer/lib/libDragon.so"); -->
<!-- ``` -->

<!-- It is strongly suggested that you add the appropriate lines to your root logon script if you have one. If you do not, the [proper way to set up your root environment](https://root.cern.ch/root/htmldoc/guides/users-guide/ROOTUsersGuide.html#environment-setup) is to create a file entitled `rootlogon.C` in a sensible place (such as `~/packages/root/macros`) and include the following lines in it: -->

<!-- ```c++ -->
<!-- gInterpreter->AddIncludePath(gSystem->ExpandPathName("${HOME}/packages/dragon/analyzer/src")); -->
<!-- gSystem->Load("${HOME}/packages/dragon/analyzer/lib/libDragon.so"); -->
<!-- ``` -->

<!-- Then create the file `${HOME}/.rootrc` (if it doesn't already exist) and include the following line in it: -->

<!-- ``` -->
<!-- Rint.Logon: /path/to/your/rootlogon.C -->
<!-- ``` -->

<!-- Examples of `rootlogon.C` and [.rootrc](https://github.com/DRAGON-Collaboration/analyzer/blob/master/script/.rootrc) are given in the [script](https://github.com/DRAGON-Collaboration/analyzer/tree/master/script) directory. This will give you access to all of the dragon classes and functions in the software package from within a ROOT session or macro. -->

<!-- If you are using git and want to stay on top of new releases, just do: -->

<!-- ``` -->
<!-- git pull master --tags -->
<!-- ``` -->

<!-- from within the repository directory (`~/packages/dragon/analyzer`). This gets you all of the new code since your last pull (or the initial clone). Note that we have started using a versioning system than goes as such: vMAJOR.MINOR.PATCH, where a MAJOR version change indicates a new set of _non backwards compatible_ changes. MINOR indicates a feature addition that is still backwards compatible, and PATCH indicates a bugfix or other small change that is backwards compatible. -->

<!-- ## __Python Extension__ -->

<!-- --- -->

<!-- ```diff -->
<!-- --note: -->
<!-- ```  -->
<!-- No one has kept up with maintaining the python extension in quite a while and it is not a priority. Really, it was just a "fun" project to play around with using `Py++` and `Boost.Python`, but it should not take too much to get it running again if anyone is interested... -->

<!-- If you wish to compile or use the optional (and somewhat experimental) python extension, you will first need to install the `Boost.Python` library. For information on this, see: http://www.boost.org/doc/libs/1_66_0/libs/python/doc/html/index.html -->

<!-- The python libraries rely on code generated by [`Py++`](http://pyplusplus.readthedocs.io/en/latest/index.html), which parses the source files and generates code to be compiled into the python libraries. It should not be necessary  to generate the source code yourself using Py++, as long as the repository is kept up-to-date; however,if you make changes to the dragon analyzer sources, re-generation of the Py++ sources will be necessary in order to safely use the python libraries. -->

<!-- To compile the python extension, simply `cd` into the py++ directory and type `make`, _after_ having compiled the core dragon library. To be able to use the extensions from any directory, add the following to your bash startup script (e.g. - `~/.bashrc`): -->

<!-- ``` -->
<!-- export PYTHONPATH=${PYTHONPATH}:"$HOME/packages/dragon/analyzer/py++/" -->
<!-- export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:"$HOME/packages/dragon/analyzer/py++/" -->
<!-- ``` -->

<!-- and re-start your bash session or re-source the startup script. Now from a python session or script, you can load the generated modules: -->

<!-- ``` -->
<!-- import midas -->
<!-- import vme -->
<!-- import dragon -->
<!-- ``` -->

<!-- You can use the python 'help' function (`help dragon`, etc) to see the classes and functions available in each module. -->
