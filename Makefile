### SET OPTIONAL #defines HERE ###
DEFINITIONS=-DDISPLAY_MODULES

### Variable definitions
SRC=$(PWD)/src
OBJ=$(PWD)/obj
CINT=$(PWD)/cint
DRLIB=$(PWD)/lib

ROOTGLIBS = $(shell root-config --glibs) -lXMLParser -lThread -lTreePlayer
RPATH    += -Wl,-rpath,$(ROOTSYS)/lib -Wl,-rpath,$(PWD)/lib
DYLIB=-shared
FPIC=-fPIC
INCFLAGS=-I$(SRC) -I$(CINT) -I$(USER) $(USER_INCLUDES)
DEBUG=-ggdb -O0 -DDEBUG
#-DDEBUG
CXXFLAGS=$(DEBUG) $(INCFLAGS) -L$(PWD)/lib $(STOCK_BUFFERS) -DBUFFER_TYPE=$(USER_BUFFER_TYPE) $(DEFINITIONS)


ifdef ROOTSYS
ROOTGLIBS = -L$(ROOTSYS)/lib $(shell $(ROOTSYS)/bin/root-config --glibs) -lXMLParser -lThread -lTreePlayer
CXXFLAGS += -L$(ROOTSYS)/lib $(shell $(ROOTSYS)/bin/root-config --cflags) -I$(ROOTSYS)/include
else
ROOTGLIBS = $(shell root-config --glibs --cflags) -lXMLParser -lThread -lTreePlayer
endif

MXML=
UNAME=$(shell uname)
ifeq ($(UNAME),Darwin)
CXXFLAGS += -DOS_LINUX -DOS_DARWIN
ifdef MIDASSYS
CXXFLAGS += -DMIDASSYS
MIDASLIBS = -L$(MIDASSYS)/darwin/lib -lmidas
INCFLAGS += -I$(MIDASSYS)/include -I$(HOME)/packages/mxml
else
INCFLAGS += -I$(SRC)/odb/mxml
MXML=libmxml.a
endif
DYLIB=-dynamiclib -single_module -undefined dynamic_lookup
FPIC=
RPATH=
endif

COMPILER=g++ -Wall
#COMPILER=clang++ -I/opt/local/include/ -I/opt/local/include/root

DEFAULTS=$(DEF_FILE_DIR) $(DEF_SAVE_DIR) $(DEF_CONFIG_DIR)

COMPILE=$(COMPILER) $(CXXFLAGS) $(RPATH) $(DEF_EXT) $(DEFAULTS) -I/user/gchristian/soft/develop/rootbeer/src -DMIDAS_BUFFERS
LINK=$(COMPILER) $(CXXFLAGS) $(ROOTGLIBS) $(RPATH) $(DEFAULTS) -I/user/gchristian/soft/develop/rootbeer/src -DMIDAS_BUFFERS
ROOTCINT=rootcint

#### DRAGON LIBRARY ####
HEADERS=$(SRC)/utils/Bits.hxx $(SRC)/vme/Vme.hxx \
$(SRC)/vme/V1190.hxx $(SRC)/vme/V792.hxx $(SRC)/vme/IO32.hxx \
$(SRC)/dragon/Modules.hxx $(SRC)/dragon/Dragon.hxx \
$(SRC)/dragon/heavy_ion/HeavyIon.hxx \
$(SRC)/dragon/heavy_ion/DSSSD.hxx $(SRC)/dragon/heavy_ion/IonChamber.hxx \
$(SRC)/dragon/heavy_ion/MCP.hxx $(SRC)/dragon/heavy_ion/SurfaceBarrier.hxx \
$(SRC)/dragon/heavy_ion/Auxillary.hxx $(SRC)/dragon/gamma/Gamma.hxx $(SRC)/dragon/gamma/Bgo.hxx \
$(SRC)/tstamp/TStamp.hxx $(SRC)/odb/Odb.hxx $(SRC)/odb/MidasXML.hxx $(SRC)/dragon/MidasEvent.hxx \
$(SRC)/midas/TMidasFile.h $(SRC)/midas/TMidasBanks.h $(SRC)/midas/TMidasEvent.h

SOURCES=$(SRC)/vme/V792.cxx $(SRC)/vme/V1190.cxx $(SRC)/vme/IO32.cxx \
$(SRC)/dragon/Modules.cxx $(SRC)/dragon/Dragon.cxx \
$(SRC)/dragon/heavy_ion/HeavyIon.cxx \
$(SRC)/dragon/heavy_ion/DSSSD.cxx $(SRC)/dragon/heavy_ion/IonChamber.cxx \
$(SRC)/dragon/heavy_ion/MCP.cxx $(SRC)/dragon/heavy_ion/SurfaceBarrier.cxx \
$(SRC)/dragon/heavy_ion/Auxillary.cxx $(SRC)/dragon/gamma/Gamma.cxx $(SRC)/dragon/gamma/Bgo.cxx  \
$(SRC)/tstamp/TStamp.cxx $(SRC)/odb/Odb.cxx $(SRC)/odb/MidasXML.cxx $(SRC)/dragon/MidasEvent.cxx \
$(SRC)/midas/TMidasFile.cxx $(SRC)/midas/TMidasEvent.cxx

OBJECTS=$(OBJ)/vme/V792.o $(OBJ)/vme/V1190.o $(OBJ)/vme/IO32.o \
$(OBJ)/dragon/Modules.o $(OBJ)/dragon/Dragon.o \
$(OBJ)/dragon/heavy_ion/HeavyIon.o \
$(OBJ)/dragon/heavy_ion/DSSSD.o $(OBJ)/dragon/heavy_ion/IonChamber.o \
$(OBJ)/dragon/heavy_ion/MCP.o $(OBJ)/dragon/heavy_ion/SurfaceBarrier.o \
$(OBJ)/dragon/heavy_ion/Auxillary.o $(OBJ)/dragon/gamma/Gamma.o $(OBJ)/dragon/gamma/Bgo.o  \
$(OBJ)/tstamp/TStamp.o $(OBJ)/odb/Odb.o $(OBJ)/odb/MidasXML.o $(OBJ)/dragon/MidasEvent.o \
$(OBJ)/midas/TMidasFile.o $(OBJ)/midas/TMidasEvent.o

### DRAGON LIBRARY ###
all: $(DRLIB)/libDragon.so
lib: $(DRLIB)/libDragon.so
$(DRLIB)/libDragon.so: $(CINT)/DragonDictionary.cxx $(OBJECTS)
	$(LINK) $(DYLIB) $(FPIC) -o $@ $(MIDASLIBS) $(OBJECTS) \
-p $(CINT)/DragonDictionary.cxx $\


### OBJECT FILES ###

V792: $(OBJ)/vme/V792.o
$(OBJ)/vme/V792.o: $(CINT)/DragonDictionary.cxx $(SRC)/vme/V792.cxx
	$(COMPILE) $(FPIC) -c \
-o $@ -p $(SRC)/vme/V792.cxx \

V1190: $(OBJ)/vme/V1190.o
$(OBJ)/vme/V1190.o: $(CINT)/DragonDictionary.cxx $(SRC)/vme/V1190.cxx
	$(COMPILE) $(FPIC) -c \
-o $@ -p $(SRC)/vme/V1190.cxx \

IO32: $(OBJ)/vme/IO32.o
$(OBJ)/vme/IO32.o: $(CINT)/DragonDictionary.cxx $(SRC)/vme/IO32.cxx
	$(COMPILE) $(FPIC) -c \
-o $@ -p $(SRC)/vme/IO32.cxx \

Modules: $(OBJ)/dragon/Modules.o
$(OBJ)/dragon/Modules.o: $(CINT)/DragonDictionary.cxx $(SRC)/dragon/Modules.cxx
	$(COMPILE) $(FPIC) -c \
-o $@ -p $(SRC)/dragon/Modules.cxx \

Dragon: $(OBJ)/dragon/Dragon.o
$(OBJ)/dragon/Dragon.o: $(CINT)/DragonDictionary.cxx $(SRC)/dragon/Dragon.cxx
	$(COMPILE) $(FPIC) -c \
-o $@ -p $(SRC)/dragon/Dragon.cxx \

MidasEvent: $(OBJ)/dragon/MidasEvent.o
$(OBJ)/dragon/MidasEvent.o: $(CINT)/DragonDictionary.cxx $(SRC)/dragon/MidasEvent.cxx
	$(COMPILE) $(FPIC) -c \
-o $@ -p $(SRC)/dragon/MidasEvent.cxx \

TMidasEvent: $(OBJ)/midas/TMidasEvent.o
$(OBJ)/midas/TMidasEvent.o: $(CINT)/DragonDictionary.cxx $(SRC)/midas/TMidasEvent.cxx
	$(COMPILE) $(FPIC) -c \
-o $@ -p $(SRC)/midas/TMidasEvent.cxx \

TMidasFile: $(OBJ)/midas/TMidasFile.o
$(OBJ)/midas/TMidasFile.o: $(CINT)/DragonDictionary.cxx $(SRC)/midas/TMidasFile.cxx
	$(COMPILE) $(FPIC) -c \
-o $@ -p $(SRC)/midas/TMidasFile.cxx \

HeavyIon: $(OBJ)/dragon/heavy_ion/HeavyIon.o
$(OBJ)/dragon/heavy_ion/HeavyIon.o: $(CINT)/DragonDictionary.cxx $(SRC)/dragon/heavy_ion/HeavyIon.cxx
	$(COMPILE) $(FPIC) -c \
-o $@ -p $(SRC)/dragon/heavy_ion/HeavyIon.cxx \

DSSSD: $(OBJ)/dragon/heavy_ion/DSSSD.o
$(OBJ)/dragon/heavy_ion/DSSSD.o: $(CINT)/DragonDictionary.cxx $(SRC)/dragon/heavy_ion/DSSSD.cxx
	$(COMPILE) $(FPIC) -c \
-o $@ -p $(SRC)/dragon/heavy_ion/DSSSD.cxx \

IonChamber: $(OBJ)/dragon/heavy_ion/IonChamber.o
$(OBJ)/dragon/heavy_ion/IonChamber.o: $(CINT)/DragonDictionary.cxx $(SRC)/dragon/heavy_ion/IonChamber.cxx
	$(COMPILE) $(FPIC) -c \
-o $@ -p $(SRC)/dragon/heavy_ion/IonChamber.cxx \

MCP: $(OBJ)/dragon/heavy_ion/MCP.o
$(OBJ)/dragon/heavy_ion/MCP.o: $(CINT)/DragonDictionary.cxx $(SRC)/dragon/heavy_ion/MCP.cxx
	$(COMPILE) $(FPIC) -c \
-o $@ -p $(SRC)/dragon/heavy_ion/MCP.cxx \

SurfaceBarrier: $(OBJ)/dragon/heavy_ion/SurfaceBarrier.o
$(OBJ)/dragon/heavy_ion/SurfaceBarrier.o: $(CINT)/DragonDictionary.cxx $(SRC)/dragon/heavy_ion/SurfaceBarrier.cxx
	$(COMPILE) $(FPIC) -c \
-o $@ -p $(SRC)/dragon/heavy_ion/SurfaceBarrier.cxx \

Auxillary: $(OBJ)/dragon/heavy_ion/Auxillary.o
$(OBJ)/dragon/heavy_ion/Auxillary.o: $(CINT)/DragonDictionary.cxx $(SRC)/dragon/heavy_ion/Auxillary.cxx
	$(COMPILE) $(FPIC) -c \
-o $@ -p $(SRC)/dragon/heavy_ion/Auxillary.cxx \

Gamma: $(OBJ)/dragon/gamma/Gamma.o
$(OBJ)/dragon/gamma/Gamma.o: $(CINT)/DragonDictionary.cxx $(SRC)/dragon/gamma/Gamma.cxx
	$(COMPILE) $(FPIC) -c \
-o $@ -p $(SRC)/dragon/gamma/Gamma.cxx \

Bgo: $(OBJ)/dragon/gamma/Bgo.o
$(OBJ)/dragon/gamma/Bgo.o: $(CINT)/DragonDictionary.cxx $(SRC)/dragon/gamma/Bgo.cxx
	$(COMPILE) $(FPIC) -c \
-o $@ -p $(SRC)/dragon/gamma/Bgo.cxx \

TStamp: $(OBJ)/tstamp/TStamp.o
$(OBJ)/tstamp/TStamp.o: $(CINT)/DragonDictionary.cxx $(SRC)/tstamp/TStamp.cxx
	$(COMPILE) $(FPIC) -c \
-o $@ -p $(SRC)/tstamp/TStamp.cxx \

Odb: $(OBJ)/odb/Odb.o
$(OBJ)/odb/Odb.o: $(CINT)/DragonDictionary.cxx $(SRC)/odb/Odb.cxx
	$(COMPILE) $(FPIC) -c \
-o $@ -p $(SRC)/odb/Odb.cxx \

MidasXML: $(OBJ)/odb/MidasXML.o $(MXML)
$(OBJ)/odb/MidasXML.o: $(CINT)/DragonDictionary.cxx $(SRC)/odb/MidasXML.cxx
	$(COMPILE) $(FPIC) -c \
-o $@ -p $(SRC)/odb/MidasXML.cxx \

libmxml: $(SRC)/odb/mxml/libmxml.a
$(SRC)/odb/mxml/libmxml.a: $(SRC)/odb/mxml/*.c $(SRC)/odb/mxml/*.h
	$(COMPILE) $(FPIC) -shared \
-o $@ -p $(SRC)/odb/mxml/mxml.c $(SRC)/odb/mxml/strlcpy.c

### CINT DICTIONARY ###
dict: $(CINT)/DragonDictionary.cxx
$(CINT)/DragonDictionary.cxx:  $(HEADERS) $(CINT)/Linkdef.h
	rootcint -f $@ -c $(CXXFLAGS) -p $(HEADERS) $(CINT)/Linkdef.h \


#### REMOVE EVERYTHING GENERATED BY MAKE ####

clean:
	rm -f $(DRLIB)/*.so rootbeer $(CINT)/DragonDictionary.* $(OBJ)/*.o $(OBJ)/*/*.o $(SRC)/odb/mxml/*.a


#### FOR DOXYGEN ####

doc::
	./make_doc.sh
