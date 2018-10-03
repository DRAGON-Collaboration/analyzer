---
title: ""
---

# __Quick Start Guide__

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

For installation troublshooting, please visit the [faq page](/analyzer/docs/html/analyzer/html/faq.html)
