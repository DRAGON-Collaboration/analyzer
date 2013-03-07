# Makefile

CXXFLAGS = -g -O2 -Wall -Wuninitialized

# optional ZLIB library

CXXFLAGS += -DHAVE_ZLIB

# ROOT libraries

ifdef ROOTSYS
#ROOTLIBS  = -L$(ROOTSYS)/lib $(shell $(ROOTSYS)/bin/root-config --libs)  -lXMLParser -lThread
#ROOTGLIBS = -L$(ROOTSYS)/lib $(shell $(ROOTSYS)/bin/root-config --glibs) -lXMLParser -lThread
#RPATH    += -Wl,-rpath,$(ROOTSYS)/lib
CXXFLAGS += -DHAVE_ROOT $(shell $(ROOTSYS)/bin/root-config --cflags)
endif

ALL:= libMidasInterface.a

OBJS:=
OBJS += TMidasEvent.o
OBJS += TMidasFile.o

ifdef MIDASSYS
CXXFLAGS += -I$(MIDASSYS)/include
OBJS += TMidasOnline.o
endif

ifdef ROOTSYS
OBJS += XmlOdb.o HttpOdb.o
endif

all: $(ALL)

libMidasInterface.a: $(OBJS)
	-rm -f $@
	ar -rv $@ $^

#%Dict.cxx: %.h
#	LD_LIBRARY_PATH=$(ROOTSYS)/lib $(ROOTSYS)/bin/rootcint -f $@ -c $^

%.o: %.cxx
	$(CXX) $(CXXFLAGS) -c $<

dox:
	doxygen

clean::
	-rm -f *.o *.a *.exe $(ALL)

# end
