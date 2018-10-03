---
title:
---

# __DRAGON Analyzer__

__Authors__
- G. Christian
- C. Stanford
- K. Olchanski
- D. Connolly

## __Introduction__

This software package is developed for the analysis of data collected in [DRAGON](http://dragon.triumf.ca) experiments at [TRIUMF](http://www.triumf.ca). The software is intended for experiments run with the "new" (c. 2013) DRAGON data-acquisition system (VME hardware + timestamp coincidence matching).

At it's core, the package is simply a collection of routines to extract ("unpack") the MIDAS data generated during an experiment into a format that is more suitable for human consumption, as well as to perform some of the more basic and commonly needed analysis tasks. From here, users can further process the data using their own codes, or they can interface with a visualization library to view the data in histograms, either online or offline. The core of the package is written in "plain" C++, that is without any dependence on libraries other than the standard library.

In addition to the "core", this package also includes code to interface with the [rootana](https://bitbucket.org/tmidas/rootana) data analysis package distributed as part of [MIDAS](https://midas.triumf.ca) and the [rootbeer](http://trshare.triumf.ca/~gchristian/rootbeer/doc/html/index.html) program for data visualization. Each of these packages is designed for "user friendly" viewing of data in histograms, either from saved files or real-time online data. Both require that a relatively recent version of [ROOT](https://root.cern.ch) be installed on your system and, additionally, MIDAS must be installed in order to look at online data (offline data viewing should be possible without a MIDAS installation, however).

There is also an optional extension to create python wrappers for the DRAGON classes, allowing data to be analyzed in python independent of ROOT/PyRoot. This package is somewhat in the experimental stage at the moment and requires installation of [`Boost.Python`](http://www.boost.org/doc/libs/1_66_0/libs/python/doc/html/index.html) to compile the libraries.

If you have a specific question about this package, you may want to check out the [FAQ page](/analyzer/docs/html/analyzer/html/faq.html) to see if it's addresed there, or visit the [Main Documentation page](/analyzer/docs/html/analyzer/html/index.html)
