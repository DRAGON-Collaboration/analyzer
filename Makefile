#################################
### USER CUSTOMIZABLE SECTION ###
#################################

### SET OPTIONAL #defines HERE  ###

### Don't remove the line below ###
DEFINITIONS=
###

## Uncomment (Comment) to (not) export "raw" modules for viewing in ROOT treew
DEFINITIONS+=-DDISPLAY_MODULES

## (Un)comment to (not) ignore corresponding heavy-ion detectors
#DEFINITIONS+=DRAGON_OMIT_DSSSD
#DEFINITIONS+=DRAGON_OMIT_IC
#DEFINITIONS+=DRAGON_OMIT_NAI
#DEFINITIONS+=DRAGON_OMIT_GE


### Set to YES (NO) to turn on (off) root [or rootbeer, or rootana, or ...] usage ###
USE_ROOT=YES
USE_ROOTANA=NO
USE_ROOTBEER=YES
USE_MIDAS=YES


## Automatically turn off rootana if on jabberwock
THE_HOST := $(shell hostname)
ifeq ($(THE_HOST),jabberwock.triumf.ca)
USE_ROOTANA=NO
endif


### Set ROOTBEER home directory (ignore if USE_ROOTBEER=NO) ###
RB_HOME=$(HOME)/packages/rootbeer


### CHOOSE YOUR COMPILER IF YOU WANT ###
##CXX=g++ -Wall
#CXX=clang++ -I/opt/local/include/

##CC=gcc -Wall
#CC=clang -I/opt/local/include/


#####################################
### END USER CUSTOMIZABLE SECTION ###
#####################################


### Variable definitions
SRC=$(PWD)/src
OBJ=$(PWD)/obj
CINT=$(PWD)/cint
DRLIB=$(PWD)/lib

DEFINITIONS+=-DDRAGON_AME12_FILENAME=\"$(PWD)/src/utils/mass.mas12\" 
DYLIB=-shared
FPIC=-fPIC
INCFLAGS=-I$(SRC) -I$(CINT)

DEBUG=-ggdb -O3 -DDEBUG
#CXXFLAGS = -g -O2 -Wall -Wuninitialized
CXXFLAGS = $(DEBUG) $(INCFLAGS) $(DEFINITIONS)
CXXFLAGS += -DHAVE_ZLIB


ROOTLIBS=
ifeq ($(USE_ROOT),YES)
DEFINITIONS+= -DUSE_ROOT
ifdef ROOTSYS
ROOTLIBS= -L$(ROOTSYS)/lib $(shell $(ROOTSYS)/bin/root-config --cflags --libs --glibs) -I$(ROOTSYS)/include -lXMLParser -lSpectrum -lMinuit
CXXFLAGS += -I$(ROOTSYS)/include
else
ROOTLIBS= - $(shell $(ROOTSYS)/bin/root-config --cflags --libs --glibs) -lXMLParser -lThread -lTreePlayer -lSpectrum -lMinuit
endif
else
USE_ROOTBEER=NO
endif

ifeq ($(USE_MIDAS),YES)
$(info ************  USE_MIDAS ************)
ifdef MIDASSYS
CXXFLAGS += -DMIDASSYS
MIDASLIBS = -lmidas -L$(MIDAS_LIB_DIR)
INCFLAGS += -I$(MIDASSYS)/include
endif
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
MIDASLIBS+= -lm -lz -lutil -lnsl -lrt
endif
endif


CXX+=$(CXXFLAGS)
CC+=$(CXXFLAGS)

LINK=$(CXX) $(ROOTLIBS) $(RPATH) -L$(PWD)/lib

MAKE_DRAGON_DICT=
DR_DICT=
DR_DICT_DEP=
ifeq ($(USE_ROOT),YES)
MAKE_DRAGON_DICT+=rootcint -f $@ -c $(CXXFLAGS) -p $(HEADERS) TTree.h $(CINT)/Linkdef.h
DR_DICT=$(CINT)/DragonDictionary.cxx 
DR_DICT_DEP=$(CINT)/DragonDictionary.cxx 
endif


#### DRAGON LIBRARY ####
OBJECTS=                            		\
$(OBJ)/midas/mxml.o  	    	  		\
$(OBJ)/midas/strlcpy.o			     	\
$(OBJ)/midas/Odb.o                  		\
$(OBJ)/midas/Xml.o                  		\
$(OBJ)/midas/libMidasInterface/TMidasFile.o  	\
$(OBJ)/midas/libMidasInterface/TMidasEvent.o 	\
$(OBJ)/midas/Event.o                	\
					\
$(OBJ)/Unpack.o				\
$(OBJ)/TStamp.o		              	\
$(OBJ)/Vme.o				\
$(OBJ)/Dragon.o				\
$(OBJ)/utils/TAtomicMass.o              \
$(OBJ)/utils/Uncertainty.o
#$(OBJ)/utils/UDouble.o

ifeq ($(USE_MIDAS), YES)
OBJECTS+=$(OBJ)/midas/libMidasInterface/TMidasOnline.o
endif

ifeq ($(USE_ROOT), YES)
OBJECTS+=$(OBJ)/utils/RootAnalysis.o
OBJECTS+=$(OBJ)/utils/Selectors.o
OBJECTS+=$(OBJ)/utils/Calibration.o
OBJECTS+=$(OBJ)/utils/LinearFitter.o
endif


## END OBJECTS ##


HEADERS=	                	\
$(SRC)/midas/*.hxx	        	\
$(SRC)/midas/*.h			\
$(SRC)/midas/libMidasInterface/*.h     	\
$(SRC)/utils/*.hxx        		\
$(SRC)/utils/*.h          		\
$(SRC)/*.hxx


### DRAGON LIBRARY ###
MID2ROOT_LIBS=-lDragon $(MIDASLIBS)

MAKE_ALL=$(DRLIB)/libDragon.so $(PWD)/bin/mid2root
ifeq ($(USE_ROOTBEER),YES)
$(info ************  USE_ROOTBEER ************)
MAKE_ALL+=$(PWD)/bin/rbdragon
DEFINITIONS+=-DUSE_ROOTBEER
MID2ROOT_LIBS+=-L$(PWD)/../../rootbeer/lib -lRootbeer
MID2ROOT_INC=$(RBINC) 
endif
ifeq ($(USE_ROOTANA),YES)
MAKE_ALL+=$(PWD)/bin/anaDragon
endif

all:  $(MAKE_ALL)

libDragon: $(DRLIB)/libDragon.so

$(DRLIB)/libDragon.so: $(DR_DICT_DEP) $(OBJECTS)
	$(LINK) $(DYLIB) $(FPIC) $(MIDASLIBS) \
\
$(OBJECTS) $(DR_DICT) \
\
-o $@ \

mid2root: $(PWD)/bin/mid2root
$(PWD)/bin/mid2root: src/mid2root.cxx $(DRLIB)/libDragon.so
	$(LINK) $(MID2ROOT_INC) $(MID2ROOT_LIBS) $< \
-o $@ \

mid2root: $(PWD)/bin/mid2root

rbdragon.o: $(OBJ)/rootbeer/rbdragon.o 

### OBJECT FILES ###

$(OBJ)/utils/%.o: $(SRC)/utils/%.cxx $(DR_DICT_DEP)
	$(CXX) $(FPIC) -c \
-o $@ $< \

$(OBJ)/midas/%.o: $(SRC)/midas/%.c $(DR_DICT_DEP)
	$(CC) $(FPIC) -c \
-o $@ $< \

$(OBJ)/midas/libMidasInterface/%.o: $(SRC)/midas/libMidasInterface/%.cxx $(DR_DICT_DEP)
	$(CXX) $(FPIC) -c \
-o $@ $< \

$(OBJ)/midas/%.o: $(SRC)/midas/%.cxx $(DR_DICT_DEP)
	$(CXX) $(FPIC) -c \
-o $@ $< \

$(OBJ)/rootana/%.o: $(SRC)/rootana/%.cxx $(CINT)/rootana/Dict.cxx
	$(CXX) $(ROOTANA_FLAGS) $(ROOTANA_DEFS) -c $(FPIC) \
-o $@ $< \

## Must be last object rule!!
$(OBJ)/%.o: $(SRC)/%.cxx $(DR_DICT_DEP)
	$(CXX) $(FPIC) -c \
-o $@ $< \

### CINT DICTIONARY ###
dict: $(CINT)/DragonDictionary.cxx

$(CINT)/DragonDictionary.cxx:  $(HEADERS) $(CINT)/Linkdef.h
	$(MAKE_DRAGON_DICT) \


### FOR ROOTANA ###
ROOTANA=$(HOME)/usr/packages/rootana
ROOTANA_FLAGS=-ansi -Df2cFortran -I$(ROOTANA)
ROOTANA_DEFS=-DROOTANA_DEFAULT_HISTOS=$(HOME)/usr/packages/dragon/analyzer/histos.dat

ROOTANA_REMOTE_OBJS=				\
$(ROOTANA)/libNetDirectory/netDirectoryServer.o

ROOTANA_OBJS=					\
$(OBJ)/rootana/Application.o			\
$(OBJ)/rootana/Callbacks.o			\
$(OBJ)/rootana/HistParser.o			\
$(OBJ)/rootana/Directory.o

ROOTANA_HEADERS= $(SRC)/rootana/Globals.h $(SRC)/rootana/*.hxx

ROOTANA_LIBS=-lrootana -lNetDirectory -L$(HOME)/usr/packages/rootana/libNetDirectory/ -L$(HOME)/usr/packages/rootana/lib

$(CINT)/rootana/Dict.cxx: $(ROOTANA_HEADERS) $(SRC)/rootana/Linkdef.h $(CINT)/DragonDictionary.cxx
	rootcint -f $@ -c $(CXXFLAGS) $(ROOTANA_FLAGS) -p $(ROOTANA_HEADERS) $(SRC)/rootana/Linkdef.h \

$(CINT)/rootana/CutDict.cxx: $(SRC)/rootana/Cut.hxx $(SRC)/rootana/CutLinkdef.h
	rootcint -f $@ -c $(CXXFLAGS) $(ROOTANA_FLAGS) -p $(SRC)/rootana/Cut.hxx $(SRC)/rootana/CutLinkdef.h \

$(DRLIB)/libRootanaCut.so: $(CINT)/rootana/CutDict.cxx
	$(LINK)  $(DYLIB) $(FPIC) $(ROOTANA_FLAGS) $(ROOTANA_DEFS)  \
-o $@ $< \

libRootanaDragon.so: $(DRLIB)/libDragon.so $(CINT)/rootana/Dict.cxx $(DRLIB)/libRootanaCut.so $(ROOTANA_OBJS)
	$(LINK)  $(DYLIB) $(FPIC) $(ROOTANA_FLAGS) $(ROOTANA_DEFS)  \
-o $@ $< $(CINT)/rootana/Dict.cxx $(ROOTANA_OBJS) -lDragon -lRootanaCut -L$(DRLIB) \

$(PWD)/bin/anaDragon: $(SRC)/rootana/anaDragon.cxx $(DRLIB)/libDragon.so $(CINT)/rootana/Dict.cxx $(DRLIB)/libRootanaCut.so $(ROOTANA_OBJS) $(ROOTANA_REMOTE_OBJS)
	$(LINK) $(ROOTANA_FLAGS) $(ROOTANA_DEFS) \
-o $@ $< $(CINT)/rootana/Dict.cxx $(ROOTANA_OBJS) -lDragon -lRootanaCut -L$(DRLIB) $(ROOTANA_LIBS) $(MIDASLIBS) \

rootana_clean:
	rm -f $(ROOTANA_OBJS) anaDragon libRootanaDragon.so $(CINT)/rootana/* $(DRLIB)/libRootanaCut.so

Dragon: $(OBJ)/Dragon.o


### FOR ROOTBEER ###

RBINC=-I$(RB_HOME)/src
RB_OBJECTS= 				\
$(OBJ)/rootbeer/rbdragon.o

RB_HEADERS= $(SRC)/rootbeer/rbdragon.hxx

RB_DEFS=-DRB_DRAGON_HOMEDIR=$(PWD)


$(CINT)/rootbeer/rootbeerDict.cxx: $(SRC)/rootbeer/rbsymbols.hxx $(DR_DICT_DEP) $(RB_HOME)/cint/RBDictionary.cxx $(RB_HOME)/cint/MidasDict.cxx
	rootcint -f $@ -c $(CXXFLAGS) $(RBINC) -p $< $(CINT)/rootbeer/rblinkdef.h \

$(OBJ)/rootbeer/rbdragon.o: $(SRC)/rootbeer/rbdragon.cxx $(SRC)/rootbeer/*.hxx $(DR_DICT_DEP) $(CINT)/rootbeer/rootbeerDict.cxx
	$(CXX) $(RB_DEFS) $(RBINC) $(FPIC) -c \
-o $@ $< \

$(PWD)/bin/rbdragon: $(CINT)/rootbeer/rootbeerDict.cxx $(RB_OBJECTS) 
	$(LINK) $^ $(RBINC) -L$(PWD)/../../rootbeer/lib  -lDragon -lRootbeer -lrbMidas \
-o $@ \



Timestamp: $(OBJ)/rootbeer/Timestamp.o
MidasBuffer: $(OBJ)/rootbeer/MidasBuffer.o
DragonEvents: $(OBJ)/rootbeer/DragonEvents.o
DragonRootbeer: $(OBJ)/rootbeer/DragonRootbeer.o



#### REMOVE EVERYTHING GENERATED BY MAKE ####

clean:
	rm -rf $(DRLIB)/*.so $(CINT)/DragonDictionary.* $(OBJECTS) $(RB_OBJECTS) obj/rootana/*.o obj/*/*.o $(CINT)/rootana/* bin/*


#### FOR DOXYGEN ####

doc::
	cd doc ; doxygen Doxyfile ; cd ..


#### FOR UNIT TESTING ####

mxml.o:           $(OBJ)/midas/libMidasInterface/mxml.o
strlcpy.o:        $(OBJ)/midas/libMidasInterface/strlcpy.o
Odb.o:            $(OBJ)/midas/Odb.o
Xml.o:            $(OBJ)/midas/Xml.o
TMidasFile.o:     $(OBJ)/midas/libMidasInterface/TMidasFile.o
TMidasEvent.o:    $(OBJ)/midas/libMidasInterface/TMidasEvent.o
Event.o:          $(OBJ)/midas/Event.o

TStamp.o:         $(OBJ)/tstamp/TStamp.o

V792.o:           $(OBJ)/vme/V792.o
V1190.o:          $(OBJ)/vme/V1190.o
Io32.o:           $(OBJ)/vme/Io32.o

Bgo.o:            $(OBJ)/dragon/Bgo.o
Mcp.o:            $(OBJ)/dragon/Mcp.o
Dsssd.o:          $(OBJ)/dragon/Dsssd.o
Auxillary.o:      $(OBJ)/dragon/Auxillary.o
IonChamber.o:     $(OBJ)/dragon/IonChamber.o
SurfaceBarrier.o: $(OBJ)/dragon/SurfaceBarrier.o

Head.o:           $(OBJ)/dragon/Head.o
Tail.o:           $(OBJ)/dragon/Tail.o
Scaler.o:         $(OBJ)/dragon/Scaler.o

test/%: test/%.cxx $(DRLIB)/libDragon.so
	$(LINK) \
$< -o $@ \
-DMIDASSYS -lDragon -L$(DRLIB) $(MIDASLIBS) -DODB_TEST -I$(PWD)/src


odbtest: $(DRLIB)/libDragon.so
	$(LINK) src/midas/Odb.cxx -o test/odbtest -DMIDASSYS -lDragon -L$(DRLIB) $(MIDASLIBS) -DODB_TEST -I$(PWD)/src

filltest: test/filltest.cxx $(DRLIB)/libDragon.so
	$(LINK) test/filltest.cxx -o test/filltest -DMIDAS_BUFFERS -lDragon -L$(DRLIB) -I$(PWD)/src \
