### SET OPTIONAL #defines HERE ###
DEFINITIONS=-DDISPLAY_MODULES

### Set to YES (NO) to turn on (off) ROOT [or ROOTBEER] usage ###
USE_ROOT=YES
USE_ROOTBEER=NO

### Set ROOTBEER home directory (ignore if USE_ROOTBEER=NO) ###
RB_HOME=/Users/gchristian/soft/develop/rootbeer


### CHOOSE YOUR COMPILER IF YOU WANT ###
CXX=g++ -Wall
#CXX=clang++ -I/opt/local/include/ -I/opt/local/include/root

CC=gcc -Wall
#CC=clang -I/opt/local/include/ -I/opt/local/include/root




### Variable definitions
SRC=$(PWD)/src
OBJ=$(PWD)/obj
CINT=$(PWD)/cint
DRLIB=$(PWD)/lib

##RPATH    += -Wl,-rpath,$(ROOTSYS)/lib -Wl,-rpath,$(PWD)/lib
DYLIB=-shared
FPIC=-fPIC
INCFLAGS=-I$(SRC) -I$(CINT)
DEBUG=-ggdb -O3 -DDEBUG
CXXFLAGS=$(DEBUG) $(INCFLAGS) -L$(PWD)/lib $(DEFINITIONS)


ROOTLIBS=
ifeq ($(USE_ROOT),YES)
ifdef ROOTSYS
ROOTLIBS= -L$(ROOTSYS)/lib $(shell $(ROOTSYS)/bin/root-config --cflags --libs --glibs) -I$(ROOTSYS)/include -lXMLParser
else
ROOTLIBS= - $(shell $(ROOTSYS)/bin/root-config --cflags --libs --glibs) -lXMLParser -lThread -lTreePlayer
endif
endif

ifdef MIDASSYS
CXXFLAGS += -DMIDASSYS
MIDASLIBS = -lmidas -L$(MIDAS_LIB_DIR) -lm -lz -lutil -lnsl -lpthread -lrt
INCFLAGS += -I$(MIDASSYS)/include
endif


UNAME=$(shell uname)
ifeq ($(UNAME),Darwin)
ifdef MIDASSYS
CXXFLAGS += -DOS_LINUX -DOS_DARWIN
MIDAS_LIB_DIR=$(MIDASSYS)/darwin/lib
endif
DYLIB=-dynamiclib -single_module -undefined dynamic_lookup
FPIC=
RPATH=
endif

ifeq ($(UNAME),Linux)
ifdef MIDASSYS
CXXFLAGS += -DOS_LINUX
MIDAS_LIB_DIR=$(MIDASSYS)/linux/lib
endif
endif



CXX+=$(CXXFLAGS)

CC+=$(CXXFLAGS)

LINK=$(CXX) $(RPATH) $(ROOTLIBS)

MAKE_DRAGON_DICT=
DR_DICT=
DR_DICT_DEP=
ifeq ($(USE_ROOT),YES)
MAKE_DRAGON_DICT+=rootcint -f $@ -c $(CXXFLAGS) -p $(HEADERS) $(CINT)/Linkdef.h
DR_DICT=-p $(CINT)/DragonDictionary.cxx 
DR_DICT_DEP=$(CINT)/DragonDictionary.cxx 
endif


#### DRAGON LIBRARY ####
OBJECTS=                            	\
$(OBJ)/midas/internal/mxml.o        	\
$(OBJ)/midas/internal/strlcpy.o     	\
$(OBJ)/midas/Odb.o                  	\
$(OBJ)/midas/Xml.o                  	\
$(OBJ)/midas/internal/TMidasFile.o  	\
$(OBJ)/midas/internal/TMidasEvent.o 	\
$(OBJ)/midas/Event.o                	\
					\
$(OBJ)/utils/TStamp.o              	\
					\
$(OBJ)/vme/V792.o             	      	\
$(OBJ)/vme/V1190.o             	     	\
$(OBJ)/vme/IO32.o              	     	\
					\
$(OBJ)/dragon/Bgo.o                 	\
$(OBJ)/dragon/MCP.o                 	\
$(OBJ)/dragon/DSSSD.o               	\
$(OBJ)/dragon/Auxillary.o           	\
$(OBJ)/dragon/IonChamber.o          	\
$(OBJ)/dragon/SurfaceBarrier.o      	\
					\
$(OBJ)/dragon/Head.o                	\
$(OBJ)/dragon/Tail.o                	\
					\
$(OBJ)/dragon/Coinc.o
## END OBJECTS ##


HEADERS=                  \
$(SRC)/midas/*.hxx        \
$(SRC)/midas/internal/*.h \
$(SRC)/utils/*.hxx        \
$(SRC)/utils/*.h          \
$(SRC)/vme/*.hxx          \
$(SRC)/dragon/*.hxx       \

### DRAGON LIBRARY ###
MAKE__ALL=$(DRLIB)/libDragon.so
ifeq ($(USE_ROOTBEER),YES)
MAKE__ALL+=$(DRLIB)/libRBDragon.so
endif

all:  $(MAKE__ALL)

libDragon: $(DRLIB)/libDragon.so

$(DRLIB)/libDragon.so: $(DR_DICT_DEP) $(OBJECTS)
	$(LINK) $(DYLIB) $(FPIC) $(MIDASLIBS) \
\
$(OBJECTS) $(DR_DICT) \
\
-o $@

### OBJECT FILES ###

$(OBJ)/dragon/%.o: $(SRC)/dragon/%.cxx $(DR_DICT_DEP)
	$(CXX) $(FPIC) -c \
-o $@ -p $< \

$(OBJ)/utils/%.o: $(SRC)/utils/%.cxx $(DR_DICT_DEP)
	$(CXX) $(FPIC) -c \
-o $@ -p $< \

$(OBJ)/vme/%.o: $(SRC)/vme/%.cxx $(DR_DICT_DEP)
	$(CXX) $(FPIC) -c \
-o $@ -p $< \

$(OBJ)/midas/internal/%.o: $(SRC)/midas/internal/%.c $(DR_DICT_DEP)
	$(CC) $(FPIC) -c \
-o $@ -p $< \

$(OBJ)/midas/internal/%.o: $(SRC)/midas/internal/%.cxx $(DR_DICT_DEP)
	$(CXX) $(FPIC) -c \
-o $@ -p $< \

$(OBJ)/midas/%.o: $(SRC)/midas/%.cxx $(DR_DICT_DEP)
	$(CXX) $(FPIC) -c \
-o $@ -p $< \

### CINT DICTIONARY ###
dict: $(CINT)/DragonDictionary.cxx


$(CINT)/DragonDictionary.cxx:  $(HEADERS) $(CINT)/Linkdef.h
	$(MAKE_DRAGON_DICT) \

definitions:
	scp dragon@ladd06.triumf.ca:/home/dragon/online/src/definitions.h \
$(PWD)/src/utils/


### FOR ROOTANA ###
ROOTANA=$(HOME)/packages/rootana
ROOTANA_FLAGS=-ansi -Df2cFortran -DHAVE_LIBNETDIRECTORY -I$(ROOTANA)

ROOTANA_OBJS=					\
$(ROOTANA)/libNetDirectory/netDirectoryServer.o \
$(OBJ)/rootana/Timestamp.o

ROOTANA_LIBS=-lrootana -lNetDirectory -L/home/dragon/packages/rootana/libNetDirectory/ -L/home/dragon/packages/rootana/

$(OBJ)/rootana/%.o: $(SRC)/rootana/%.cxx $(SRC)/rootana/*.hxx
	$(CXX) -c \
-o $@ -p $<

anaDragon: $(SRC)/rootana/anaDragon.cxx $(DRLIB)/libDragon.so $(ROOTANA_OBJS) $(SRC)/rootana/Globals.h
	$(LINK) $(RBINC) $(ROOTANA_FLAGS) \
-o $@ -p $< $(ROOTANA_OBJS) $(MIDASLIBS) $(ROOTANA_LIBS) -lDragon -L$(DRLIB) \



rootanaTimestamp: $(OBJ)/rootana/Timestamp.o


### FOR ROOTBEER ###

RBINC=-I$(RB_HOME)/src
RB_OBJECTS= 				\
$(OBJ)/rootbeer/Timestamp.o		\
$(OBJ)/rootbeer/MidasBuffer.o		\
$(OBJ)/rootbeer/DragonEvents.o		\
$(OBJ)/rootbeer/DragonRootbeer.o	\

RB_HEADERS= $(SRC)/rootbeer/*.hxx


$(DRLIB)/libRBDragon.so: $(RB_OBJECTS) $(RB_HEADERS)
	$(LINK) $(RBINC) $(DYLIB) $(FPIC) -o $@ $(MIDASLIBS) $(RB_OBJECTS) \

libRBDragon: $(DRLIB)/libRBDragon.so

$(OBJ)/rootbeer/%.o: $(SRC)/rootbeer/%.cxx $(SRC)/rootbeer/*.hxx
	$(CXX) $(RBINC) $(FPIC) -c \
-o $@ -p $< \


Timestamp: $(OBJ)/rootbeer/Timestamp.o
MidasBuffer: $(OBJ)/rootbeer/MidasBuffer.o
DragonEvents: $(OBJ)/rootbeer/DragonEvents.o
DragonRootbeer: $(OBJ)/rootbeer/DragonRootbeer.o

#### REMOVE EVERYTHING GENERATED BY MAKE ####

clean:
	rm -f $(DRLIB)/*.so $(CINT)/DragonDictionary.* $(OBJECTS) $(RB_OBJECTS) obj/rootana/*.o anaDragon


#### FOR DOXYGEN ####

doc::
	doxygen doc/Doxyfile

docclean::
	rm -fr /Volumes/public/gchristian/public_html/dragon/analyzer/html \
/Volumes/public/gchristian/public_html/dragon/analyzer/latex


#### FOR UNIT TESTING ####

mxml.o:           $(OBJ)/midas/internal/mxml.o
strlcpy.o:        $(OBJ)/midas/internal/strlcpy.o
Odb.o:            $(OBJ)/midas/Odb.o
Xml.o:            $(OBJ)/midas/Xml.o
TMidasFile.o:     $(OBJ)/midas/internal/TMidasFile.o
TMidasEvent.o:    $(OBJ)/midas/internal/TMidasEvent.o
Event.o:          $(OBJ)/midas/Event.o

TStamp.o:         $(OBJ)/tstamp/TStamp.o

V792.o:           $(OBJ)/vme/V792.o
V1190.o:          $(OBJ)/vme/V1190.o
IO32.o:           $(OBJ)/vme/IO32.o

Bgo.o:            $(OBJ)/dragon/Bgo.o
MCP.o:            $(OBJ)/dragon/MCP.o
DSSSD.o:          $(OBJ)/dragon/DSSSD.o
Auxillary.o:      $(OBJ)/dragon/Auxillary.o
IonChamber.o:     $(OBJ)/dragon/IonChamber.o
SurfaceBarrier.o: $(OBJ)/dragon/SurfaceBarrier.o

Head.o:           $(OBJ)/dragon/Head.o
Tail.o:           $(OBJ)/dragon/Tail.o

odbtest: $(DRLIB)/libDragon.so
	$(LINK) src/midas/Odb.cxx -o test/odbtest -DMIDASSYS -lDragon -L$(DRLIB) $(MIDASLIBS) -DODB_TEST -I$(PWD)/src

filltest: test/filltest.cxx $(DRLIB)/libDragon.so
	$(LINK) test/filltest.cxx -o test/filltest -DMIDAS_BUFFERS -lDragon -L$(DRLIB) -I$(PWD)/src \