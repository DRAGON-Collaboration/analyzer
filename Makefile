### Include local makefile with user customization. This is supposed to be generated by the ./configure script
include config.mk

ifdef USE_ROOT
else
$(error No config.mk file found. Please run the configure script first. Running './configure --help' will give instructions on how to do this)
endif

SHLIBFILE    = $(DRLIB)/libDragon.so
ROOTMAPFILE := $(patsubst %.so,%.rootmap,$(SHLIBFILE))

ifeq ($(USE_ROOT),YES)
ifeq ($(ROOTMAJORVERSION),6)
MAKE_DRAGON_DICT += $(ROOTCINT) -f $@ -s $(SHLIBFILE) -rml $(SHLIBFILE) -rmf $(ROOTMAPFILE) -c $(CINTFLAGS) \
-p $(HEADERS) TError.h TNamed.h TObject.h TString.h TTree.h $(CINT)/Linkdef.h
else
MAKE_DRAGON_DICT += $(ROOTCINT) -f $@ -c $(CINTFLAGS) -p $(HEADERS) TTree.h $(CINT)/Linkdef5.h
endif
DRA_DICT          = $(DRLIB)/DragonDictionary.cxx
DRA_DICT_DEP      = $(DRLIB)/DragonDictionary.cxx
ROOTLIBS         += -lXMLParser -lTreePlayer -lSpectrum -lMinuit
ROOTGLIBS        += -lXMLParser -lTreePlayer -lSpectrum -lMinuit
endif

# DEBUG       += -ggdb -O3 -DDEBUG
# CXXFLAGS     = -g -O2 -Wall -Wuninitialized
# CXXFLAGS    += -Wall $(DEBUG) $(INCLUDE)
CXXFLAGS    += $(DEFINITIONS) -DHAVE_ZLIB
CCFLAGS     +=

ifeq ($(USE_ROOTBEER),YES)
# conditional for rootcint var to compile librootbeer, rbdragon
endif

ifeq ($(USE_ROOTANA),YES)
# conditional for rootcint var to compile libanaDragon, anaDragon
endif

CC        += $(filter-out -std=c++11, $(CXXFLAGS))
CXXFLAGS  += $(INCLUDE)
CINTFLAGS := $(filter-out ($(ROOTCFLAGS)), $(CXXFLAGS))

ifeq ($(NAME),Ubuntu5)
CXX += $(filter-out -std=c++11, $(CXXFLAGS))
else
CXX += $(CXXFLAGS)
endif

LD   = $(CXX) $(LDFLAGS) $(ROOTGLIBS) $(RPATH) -L$(PWD)/lib

HEADERS =								\
$(SRC)/midas/*.hxx						\
$(SRC)/midas/*.h						\
$(SRC)/midas/libMidasInterface/*.h		\
$(SRC)/utils/*.hxx						\
$(SRC)/utils/*.h						\
$(SRC)/*.hxx

#### OBJECTS ####
OBJECTS =										\
$(OBJ)/midas/mxml.o								\
$(OBJ)/midas/Odb.o								\
$(OBJ)/midas/Xml.o								\
$(OBJ)/midas/libMidasInterface/TMidasFile.o		\
$(OBJ)/midas/libMidasInterface/TMidasEvent.o	\
$(OBJ)/midas/Event.o							\
$(OBJ)/Unpack.o									\
$(OBJ)/TStamp.o									\
$(OBJ)/Vme.o									\
$(OBJ)/Dragon.o									\
$(OBJ)/Sonik.o									\
$(OBJ)/utils/Uncertainty.o						\
$(OBJ)/utils/ErrorDragon.o

ifeq ($(USE_MIDAS), YES)
OBJECTS += $(OBJ)/midas/libMidasInterface/TMidasOnline.o
endif

ifeq ($(USE_ROOT), YES)
OBJECTS += $(OBJ)/utils/RootAnalysis.o
OBJECTS += $(OBJ)/utils/Selectors.o
OBJECTS += $(OBJ)/utils/Calibration.o
OBJECTS += $(OBJ)/utils/LinearFitter.o
OBJECTS += $(OBJ)/utils/TAtomicMass.o
OBJECTS += $(OBJ)/utils/Kin2Body.o
endif
## END OBJECTS ##

all:  $(MAKE_ALL)

libDragon: $(SHLIBFILE)

$(SHLIBFILE): $(DRA_DICT_DEP) $(OBJECTS)
	$(LD) $(DYLIB) $(MIDASLIBS) $(OBJECTS) $(DRA_DICT) -o $@ \

mid2root: $(PWD)/bin/mid2root

$(PWD)/bin/mid2root: src/mid2root.cxx $(SHLIBFILE)
	$(LD) $(MID2ROOT_INC) $(MID2ROOT_LIBS) $< -o $@ \

#rbdragon.o: $(OBJ)/rootbeer/rbdragon.o

# rbdragon_impl.o: $(OBJ)/rootbeer/rbdragon_impl.o

### OBJECT FILES ###
$(OBJ)/utils/%.o: $(SRC)/utils/%.cxx $(DRA_DICT_DEP)
	$(CXX) -c -o $@ $< \

$(OBJ)/midas/%.o: $(SRC)/midas/%.c $(DRA_DICT_DEP)
	$(CC) -c -o $@ $< \

$(OBJ)/midas/%.o: $(SRC)/midas/%.cxx $(DRA_DICT_DEP)
	$(CXX) -c -o $@ $< \

$(OBJ)/midas/libMidasInterface/%.o: $(SRC)/midas/libMidasInterface/%.cxx $(DRA_DICT_DEP)
	$(CXX) -c -o $@ $< \

$(OBJ)/rootana/%.o: $(SRC)/rootana/%.cxx $(ANA_DRAGON_DICT)
	$(CXX) $(ROOTANA_FLAGS) $(ROOTANA_DEFS) -c -o $@ $< \

## Must be last object rule!!
$(OBJ)/%.o: $(SRC)/%.cxx $(DRA_DICT_DEP)
	$(CXX) -c -o $@ $< \

### CINT DICTIONARY ###
dict: $(DRA_DICT)

$(DRA_DICT):  $(HEADERS) $(CINT)/Linkdef.h
	$(MAKE_DRAGON_DICT) \

$(ANA_DRAGON_DICT): $(ROOTANA_HEADERS) $(DRA_DICT) $(SRC)/rootana/Linkdef.h
	$(ROOTCINT) -f $@ -c $(CINTFLAGS) $(ROOTANA_FLAGS) -p $(ROOTANA_HEADERS) $(SRC)/rootana/Linkdef.h \

$(DRLIB)/CutDict.cxx: $(SRC)/rootana/Cut.hxx $(SRC)/rootana/CutLinkdef.h
	$(ROOTCINT) -f $@ -c $(CINTFLAGS) $(ROOTANA_FLAGS) -p $(SRC)/rootana/Cut.hxx $(SRC)/rootana/CutLinkdef.h \

libRootanaCut: $(DRLIB)/libRootanaCut.so

$(DRLIB)/libRootanaCut.so: $(DRLIB)/CutDict.cxx
	$(LD) $(DYLIB) $(ROOTANA_FLAGS) $(ROOTANA_DEFS) -o $@ $< \

libRootanaDragon: $(DRLIB)/libRootanaDragon.so

$(DRLIB)/libRootanaDragon.so: $(SHLIBFILE) $(ANA_DRAGON_DICT) $(DRLIB)/libRootanaCut.so $(ROOTANA_OBJS)
	$(LD) $(DYLIB)  $(ROOTANA_FLAGS) $(ROOTANA_DEFS) -o $@ $< $(ANA_DRAGON_DICT) $(ROOTANA_OBJS) \
	-lDragon -lRootanaCut -L$(DRLIB) \

anaDragon: $(PWD)/bin/anaDragon

$(PWD)/bin/anaDragon: $(SRC)/rootana/anaDragon.cxx $(SHLIBFILE) $(ANA_DRAGON_DICT) $(DRLIB)/libRootanaCut.so $(ROOTANA_OBJS) $(ROOTANA_REMOTE_OBJS)
	$(LD) $(ROOTANA_FLAGS) $(ROOTANA_DEFS) -o $@ $< $(ANA_DRAGON_DICT) $(ROOTANA_OBJS) \
	-lDragon -lRootanaCut -L$(DRLIB) $(ROOTANA_LIBS) $(MIDASLIBS) \

rootana_clean:
	rm -f $(ANA_DRAGON_DICT) $(ROOTANA_OBJS) $(PWD)/bin/anaDragon

Dragon: $(OBJ)/Dragon.o

$(RB_DICT): $(SRC)/rootbeer/rbsymbols.hxx $(DRA_DICT_DEP) $(RB_HOME)/cint/RBDictionary.cxx $(RB_HOME)/cint/MidasDict.cxx
	$(ROOTCINT) -f $@ -c $(CINTFLAGS) $(RBINC) -p $< $(CINT)/rootbeer/rblinkdef.h \

$(OBJ)/rootbeer/rbdragon.o: $(SRC)/rootbeer/rbdragon.cxx $(SRC)/rootbeer/*.hxx $(DRA_DICT_DEP) $(RB_DICT)
	$(CXX) $(RB_DEFS) $(RBINC) -c -o $@ $< \

$(OBJ)/rootbeer/rbsonik.o: $(SRC)/rootbeer/rbsonik.cxx $(SRC)/rootbeer/*.hxx $(DRA_DICT_DEP) $(RB_DICT)
	$(CXX) $(RB_DEFS) $(RBINC) -c -o $@ $< \

$(OBJ)/rootbeer/rbdragon_impl.o: $(SRC)/rootbeer/rbdragon_impl.cxx $(SRC)/rootbeer/*.hxx $(DRA_DICT_DEP) $(RB_DICT)
	$(CXX) $(RB_DEFS) $(RBINC) -c -o $@ $< \

$(OBJ)/rootbeer/rbsonik_impl.o: $(SRC)/rootbeer/rbsonik_impl.cxx $(SRC)/rootbeer/*.hxx $(DRA_DICT_DEP) $(RB_DICT)
	$(CXX) $(RB_DEFS) $(RBINC) -c -o $@ $< \

rbdragon: $(PWD)/bin/rbdragon

rbsonik: $(PWD)/bin/rbsonik

$(PWD)/bin/rbdragon: $(RB_DICT) $(RB_DRAGON_OBJECTS)
	$(LD) $^ $(RBINC) $(RB_LIBS) -lDragon -o $@ \

$(PWD)/bin/rbsonik: $(RB_DICT) $(RB_SONIK_OBJECTS)
	$(LD) $^ $(RBINC) $(RB_LIBS) -lDragon -o $@ \

rootbeer_clean:
	rm -f $(RB_DICT) $(PWD)/bin/rbdragon $(PWD)/bin/rbsonik

Timestamp: $(OBJ)/rootbeer/Timestamp.o

MidasBuffer: $(OBJ)/rootbeer/MidasBuffer.o

DragonEvents: $(OBJ)/rootbeer/DragonEvents.o

DragonRootbeer: $(OBJ)/rootbeer/DragonRootbeer.o

#### REMOVE EVERYTHING GENERATED BY MAKE ####
.PHONY: clean
clean: $(CLEAN_ALL)
	rm -f $(DRA_DICT) $(SHLIBFILE) $(ROOTMAPFILE) $(OBJECTS) $(RB_DRAGON_OBJECTS) $(RB_SONIK_OBJECTS) $(DRLIB)/*.so $(DRLIB)/*.pcm $(DRLIB)/*.h $(PWD)/bin/mid2root

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

test/%: test/%.cxx $(SHLIBFILE)
	$(LD) \
	$< -o $@ \
	-DMIDASSYS -lDragon -L$(DRLIB) $(MIDASLIBS) -DODB_TEST -I$(PWD)/src

odbtest: $(SHLIBFILE)
	$(LD) src/midas/Odb.cxx \
	-o test/odbtest -DMIDASSYS \
	-lDragon -L$(DRLIB) $(MIDASLIBS) \
	-DODB_TEST -I$(PWD)/src

filltest: test/filltest.cxx $(SHLIBFILE)
	$(LD) test/filltest.cxx \
	-o bin/filltest \
	-DMIDAS_BUFFERS \
	-lDragon -L$(DRLIB) -I$(PWD)/src \
