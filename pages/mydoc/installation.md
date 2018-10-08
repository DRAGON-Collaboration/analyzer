---
title: Installation Instructions
keywords: installation
sidebar: mydoc_sidebar
permalink: installation.html
---

## __Dependencies__

For the "core" functionality, all you will need is a working C++ compiler; there is no dependence on any third-party libraries. However, you will likely want to make use of the analysis package in ROOT, in which case you will need a fairly recent installation of ROOT on your system. For instructions on installing ROOT, go [here](https://root.cern.ch/building-root). It is suggested that you install a relatively recent version of ROOT (say, ≥ 5.30) to ensure combatibilty between the ROOT release and the package code. You will also need to be sure that the `$ROOTSYS` environment variable is set and that `$ROOTSYS/bin` is in your search path. This is typically accomplished by sourcing the `thisroot.sh` or `thisroot.csh` script in `$ROOTSYS/bin` from your startup script (e.g. - `~/.bashrc`). It is also required that ROOT be compiled with the `minuit2` and `xml` options set to `ON`.

{% include warning.html content="Ubuntu users: compiliation of the DRAGON analyzer package on ubuntu evidently requires clang: <br/><br/>
`sudo apt-get install clang clang++`" %}

{% include note.html content="The DRAGON analyzer package is now compatible with ROOT6, but ROOT6 functionality should be considered beta until it has been tested more rigorously. Please report bugs to [dconnolly@triumf.ca](mailto:dconnolly@triumf.ca) or [file an issue here](https://github.com/DRAGON-Collaboration/analyzer/issues)." %}

The optional [rootbeer](https://github.com/gchristian/rootbeer) or [rootana](https://bitbucket.org/tmidas/rootana) extensions each require ROOT to be installed (and, of course, the rootana and/or rootbeer packages themselves). To look at online data, you will need MIDAS installed, and, if using the rootana system, roody is required for online histogram viewing.

## __Download and Compile__

One may obtain the analysis package from the git repository as follows:

``` sh
git clone https://github.com/DRAGON-Collaboration/analyzer
```

If you don't have git installed or prefer to download a tarball, then visit https://github.com/DRAGON-Collaboration/analyzer, click on the [releases](https://github.com/DRAGON-Collaboration/analyzer/releases) link and select the latest release (or an earlier one if you have reason to do so). Click on either the `.zip` or the `.tar.gz` link to download a zip file or a tarball containing the code.

Once the package is obtained, it is necessary to run a configure script in order to set the appropriate environment variables before compiling. To see a list of options, type:

``` sh
./configure --help
```

In most cases, one may simply run

``` sh
./configure
```

At this point, compilation should be as simple as typing make. If not, please [let me know](mailto:dconnolly@triumf.ca) or [file an issue](https://github.com/DRAGON-Collaboration/analyzer/issues), and we can try to fix the problem permanently so others don't run into it.

Compilation creates a shared library `lib/libDragon.so` (and `lib/libDragon.so.dSYM` if installed on a Mac) as well as an executable `bin/mid2root`. The executable converts MIDAS files to ROOT trees. 

## __Environment Setup__

The library can be loaded into an interactive ROOT session by issuing the following commands:

``` sh
root[] .include /path/to/your/dragon/analyzer/src
root[] gSystem->Load("/path/to/your/dragon/analyzer/lib/libDragon.so");
```

It is strongly suggested that you add the appropriate lines to your root logon script if you have one. If you don't, [the proper way to set up your root environment](https://root.cern.ch/root/htmldoc/guides/users-guide/ROOTUsersGuide.html#environment-setup) is to create a file entitled `rootlogon.C` in a sensible place (such as `~/packages/root/macros`) and include the following lines in it:

``` c++
gInterpreter->AddIncludePath(gSystem->ExpandPathName("${HOME}/path/to/your/dragon/analyzer/src"));
gSystem->Load("/path/to/your/dragon/analyzer/lib/libDragon.so");
```

Then create the file `${HOME}/.rootrc` and include the following line in it:

``` sh
Rint.Logon: /path/to/your/rootlogon.C
```

This will give you access to all of the dragon classes and functions in the software package from within a ROOT session or macro. Examples of these scripts are provided: [.rootrc](/analyzer/script/.rootrc)   [rootlogon.C](/analyzer/script/rootlogon.C)

If you are using git and want to stay on top of new releases, just do:

``` sh
git pull https://github.com/DRAGON-Collaboration/analyzer master --tags
```

(or just `git pull`) from within the repository directory (`~/packages/dragon/analyzer`). This gets you all of the new code since your last pull (or the initial clone). Note that we've started using a versioning system than goes as such: `vMAJOR.MINOR.PATCH`, where a MAJOR version change indicates a new set of non backwards compatible changes. MINOR indicates a feature addition that is still backwards compatible, and PATCH indicates a bugfix or other small change that is backwards compatible.
