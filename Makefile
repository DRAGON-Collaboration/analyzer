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
CXXFLAGS=$(DEBUG) $(INCFLAGS) -L$(PWD)/lib $(STOCK_BUFFERS) -DBUFFER_TYPE=$(USER_BUFFER_TYPE)


ifdef ROOTSYS
ROOTGLIBS = -L$(ROOTSYS)/lib $(shell $(ROOTSYS)/bin/root-config --glibs) -lXMLParser -lThread -lTreePlayer
CXXFLAGS += -L$(ROOTSYS)/lib $(shell $(ROOTSYS)/bin/root-config --cflags) -I$(ROOTSYS)/include
else
ROOTGLIBS = $(shell root-config --glibs --cflags) -lXMLParser -lThread -lTreePlayer
endif

UNAME=$(shell uname)
ifeq ($(UNAME),Darwin)
CXXFLAGS += -DOS_LINUX -DOS_DARWIN
ifdef MIDASSYS
MIDASLIBS = -L$(MIDASSYS)/darwin/lib -lmidas
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
ROOTCINT=rootcint $(USER_DEFINITIONS)

#### DRAGON LIBRARY ####
HEADERS=$(SRC)/utils/Bits.hxx $(SRC)/vme/Vme.hxx $(SRC)/vme/IO32.hxx \
$(SRC)/vme/caen/V1190.hxx $(SRC)/vme/caen/Adc.hxx \
$(SRC)/dragon/Modules.hxx $(SRC)/dragon/Dragon.hxx \
$(SRC)/dragon/heavy_ion/HeavyIon.hxx \
$(SRC)/dragon/heavy_ion/DSSSD.hxx $(SRC)/dragon/heavy_ion/IonChamber.hxx \
$(SRC)/dragon/heavy_ion/MCP.hxx $(SRC)/dragon/heavy_ion/SurfaceBarrier.hxx \
$(SRC)/dragon/gamma/Gamma.hxx $(SRC)/dragon/gamma/Bgo.hxx \
$(SRC)/tstamp/TStamp.hxx

SOURCES=$(SRC)/vme/caen/Adc.cxx $(SRC)/vme/caen/V1190.cxx \
$(SRC)/dragon/Modules.cxx $(SRC)/dragon/Dragon.cxx \
$(SRC)/dragon/heavy_ion/HeavyIon.cxx \
$(SRC)/dragon/heavy_ion/DSSSD.cxx $(SRC)/dragon/heavy_ion/IonChamber.cxx \
$(SRC)/dragon/heavy_ion/MCP.cxx $(SRC)/dragon/heavy_ion/SurfaceBarrier.cxx \
$(SRC)/dragon/gamma/Gamma.cxx $(SRC)/dragon/gamma/Bgo.cxx  \
$(SRC)/tstamp/TStamp.cxx

OBJECTS=$(OBJ)/vme/caen/Adc.o $(OBJ)/vme/caen/V1190.o \
$(OBJ)/dragon/Modules.o $(OBJ)/dragon/Dragon.o \
$(OBJ)/dragon/heavy_ion/HeavyIon.o \
$(OBJ)/dragon/heavy_ion/DSSSD.o $(OBJ)/dragon/heavy_ion/IonChamber.o \
$(OBJ)/dragon/heavy_ion/MCP.o $(OBJ)/dragon/heavy_ion/SurfaceBarrier.o \
$(OBJ)/dragon/gamma/Gamma.o $(OBJ)/dragon/gamma/Bgo.o  \
$(OBJ)/tstamp/TStamp.o

### DRAGON LIBRARY ###
all: $(DRLIB)/libDragon.so
lib: $(DRLIB)/libDragon.so
$(DRLIB)/libDragon.so: $(CINT)/DragonDictionary.cxx $(OBJECTS)
	$(LINK) $(DYLIB) $(FPIC) -o $@ $(MIDASLIBS) $(OBJECTS) \
-p $(CINT)/DragonDictionary.cxx $\


### OBJECT FILES ###
Adc: $(OBJ)/vme/caen/Adc.o
$(OBJ)/vme/caen/Adc.o: $(CINT)/DragonDictionary.cxx $(SRC)/vme/caen/Adc.cxx
	$(COMPILE) $(FPIC) -c \
-o $@ -p $(SRC)/vme/caen/Adc.cxx \

V1190: $(OBJ)/vme/caen/V1190.o
$(OBJ)/vme/caen/V1190.o: $(CINT)/DragonDictionary.cxx $(SRC)/vme/caen/V1190.cxx
	$(COMPILE) $(FPIC) -c \
-o $@ -p $(SRC)/vme/caen/V1190.cxx \

Modules: $(OBJ)/dragon/Modules.o
$(OBJ)/dragon/Modules.o: $(CINT)/DragonDictionary.cxx $(SRC)/dragon/Modules.cxx
	$(COMPILE) $(FPIC) -c \
-o $@ -p $(SRC)/dragon/Modules.cxx \

Dragon: $(OBJ)/dragon/Dragon.o
$(OBJ)/dragon/Dragon.o: $(CINT)/DragonDictionary.cxx $(SRC)/dragon/Dragon.cxx
	$(COMPILE) $(FPIC) -c \
-o $@ -p $(SRC)/dragon/Dragon.cxx \

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

### CINT DICTIONARY ###
dict: $(CINT)/DragonDictionary.cxx
$(CINT)/DragonDictionary.cxx:  $(HEADERS) $(CINT)/Linkdef.h
	rootcint -f $@ -c $(CXXFLAGS) $(USER_DEFINITIONS) -p $(HEADERS) $(CINT)/Linkdef.h \


#### REMOVE EVERYTHING GENERATED BY MAKE ####

clean:
	rm -f $(DRLIB)/*.so rootbeer $(CINT)/DragonDictionary.* $(OBJ)/*.o $(OBJ)/*/*.o


#### FOR DOXYGEN ####

doc: $(DRLIB)/libDragon.so
	./make_doc.sh
