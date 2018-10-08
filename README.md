# __DRAGON Analyzer__

#### Authors
- G. Christian
- C. Stanford
- K. Olchanski

## __Introduction__

This software package is developed for the analysis of data collected in [DRAGON](http://dragon.triumf.ca) experiments at [TRIUMF](http://www.triumf.ca). The software is intended for experiments run with the "new" (c. 2013) DRAGON data-acquisition system (VME hardware + timestamp coincidence matching).

At it's core, the package is simply a collection of routines to extract ("unpack") the MIDAS data generated during an experiment into a format that is more suitable for human consumption, as well as to perform some of the more basic and commonly needed analysis tasks. From here, users can further process the data using their own codes, or they can interface with a visualization library to view the data in histograms, either online or offline. The core of the package is written in "plain" C++, that is without any dependence on libraries other than the standard library.

In addition to the "core", this package also includes code to interface with the [rootana](https://bitbucket.org/tmidas/rootana) data analysis package distributed as part of [MIDAS](https://midas.triumf.ca) and the [rootbeer](http://trshare.triumf.ca/~gchristian/rootbeer/doc/html/index.html) program for data visualization. Each of these packages is designed for "user friendly" viewing of data in histograms, either from saved files or real-time online data. Both require that a relatively recent version of [ROOT](https://root.cern.ch) be installed on your system and, additionally, MIDAS must be installed in order to look at online data (offline data viewing should be possible without a MIDAS installation, however).

There is also an optional extension to create python wrappers for the DRAGON classes, allowing data to be analyzed in python independent of ROOT/PyRoot. This package is somewhat in the experimental stage at the moment and requires installation of [`Boost.Python`](http://www.boost.org/doc/libs/1_66_0/libs/python/doc/html/index.html) to compile the libraries.

If you have a specific question about this package, you may want to check out the [FAQ page](http://dragon-collaboration.github.io/analyzer/html/analyzer/html/faq.html) to see if it's addresed there, or visit the [Main Documentation page](http://dragon-collaboration.github.io/analyzer/html/analyzer/html/index.html)

## __Quick Start Guide__

***
```diff
++ Note: These instructions assume that you will install the dragon analyzer package in ~/packages/dragon
```
***

There are two methods for obtaining and installing the code:

1. Clone the git repository:
```
mkdir -p ~/packages/dragon
cd ~/packages/dragon
git clone https://github.com/DRAGON-Collaboration/analyzer
cd analyzer
./configure (--with-options if desired)
make
```

2. Download a [release](https://github.com/DRAGON-Collaboration/analyzer/releases)
```
tar -xzvf analyzer-1.x.x.tar.gz  --or-- unzip analyzer-1.x.x.zip
cd analyzer
./configure (--with-options if desired)
make
```

The library can be loaded into an interactive ROOT session by issuing the following commands:
```
root[] .include /path/to/your/dragon/analyzer/src
root[] gSystem->Load("/path/to/your/dragon/analyzer/lib/libDragon.so");
```

To convert a MIDAS file to a ROOT file (assuming `~/packages/dragon/analyzer/bin` is in your search path), issue the command

```
mid2root runxxxx.mid
```

For installation troublshooting, please visit the [faq page](http://dragon-collaboration.github.io/analyzer/html/analyzer/html/faq.html)


## __Installation__

### __Dependencies__

For the "core" functionality, all you will need is a working C++ compiler; there is no dependence on any third-party libraries. However, you will likely want to make use of the analysis package in ROOT, in which case you will need a fairly recent installation of ROOT on your system. For instructions on installing ROOT, go [here](https://root.cern.ch/building-root). It is suggested that you install a relatively recent version of ROOT (say, ≥ 5.30) to ensure combatibilty between the ROOT release and the package code. You will also need to be sure that the `$ROOTSYS` environment variable is set and that `$ROOTSYS/bin` is in your search path. This is typically accomplished by sourcing the `thisroot.sh` or `thisroot.csh` script in `$ROOTSYS/bin` from your startup script (e.g. - `~/.bashrc`). It is also required that ROOT be compiled with the `minuit2` and `xml` options set to `ON`.

***
```diff
-- Warning: Ubuntu users: compiliation of the DRAGON analyzer package on ubuntu evidently requires clang
```
```
sudo apt-get install clang clang++
```
***

***
```diff 
++ Note: 
```
> The DRAGON analyzer package is now compatible with ROOT6, but ROOT6 functionality should be considered beta until it has been tested more rigorously. Please report bugs to [dconnolly@triumf.ca](mailto:dconnolly@triumf.ca) or [file an issue here](https://github.com/DRAGON-Collaboration/analyzer/issues).
***

The optional rootbeer or rootana extensions each require ROOT to be installed (and, of course, the rootana and/or rootbeer packages themselves). To look at online data, you will need MIDAS installed, and, if using the rootana system, roody is required for online histogram viewing.

### __Download and Compile__

One may obtain the analysis package from the git repository as follows:
```
git clone https://github.com/DRAGON-Collaboration/analyzer
```
If you don't have git installed or prefer to download a tarball, then visit https://github.com/DRAGON-Collaboration/analyzer, click on the [releases](https://github.com/DRAGON-Collaboration/analyzer/releases) link and select the latest release (or an earlier one if you have reason to do so). Click on either the `.zip` or the `.tar.gz` link to download a zip file or a tarball containing the code.

Once the package is obtained, it is necessary to run a configure script in order to set the appropriate environment variables before compiling. To see a list of options, type:
```
./configure --help
```

In most cases, one may simply run
```
./configure
```

At this point, compilation should be as simple as typing make. If not, please [let me know](mailto:dconnolly@triumf.ca) or [file an issue](https://github.com/DRAGON-Collaboration/analyzer/issues), and we can try to fix the problem permanently so others don't run into it.

Compilation creates a shared library `lib/libDragon.so` (and `lib/libDragon.so.dSYM` if installed on a Mac) as well as an executable `bin/mid2root`. The executable converts MIDAS files to ROOT trees. The library can be loaded into an interactive ROOT session by issuing the following commands:
```
root[] .include /path/to/your/dragon/analyzer/src
root[] gSystem->Load("/path/to/your/dragon/analyzer/lib/libDragon.so");
```

It is strongly suggested that you add the appropriate lines to your root logon script if you have one. If you don't, [the proper way to set up your root environment](https://root.cern.ch/root/htmldoc/guides/users-guide/ROOTUsersGuide.html#environment-setup) is to create a file entitled `rootlogon.C` in a sensible place (such as `~/packages/root/macros`) and include the following lines in it:
```cpp
gInterpreter->AddIncludePath(gSystem->ExpandPathName("${HOME}/path/to/your/dragon/analyzer/src"));
gSystem->Load("/path/to/your/dragon/analyzer/lib/libDragon.so");
```

Then create the file `${HOME}/.rootrc` and include the following line in it:
```
Rint.Logon: /path/to/your/rootlogon.C
```

This will give you access to all of the dragon classes and functions in the software package from within a ROOT session or macro. Examples of these scripts are provided: [.rootrc](script/.rootrc)   [rootlogon.C](script/rootlogon.C)

If you are using git and want to stay on top of new releases, just do:
```
git pull https://github.com/DRAGON-Collaboration/analyzer master --tags
```

(or just `git pull`) from within the repository directory (`~/packages/dragon/analyzer`). This gets you all of the new code since your last pull (or the initial clone). Note that we've started using a versioning system than goes as such: `vMAJOR.MINOR.PATCH`, where a MAJOR version change indicates a new set of non backwards compatible changes. MINOR indicates a feature addition that is still backwards compatible, and PATCH indicates a bugfix or other small change that is backwards compatible.
