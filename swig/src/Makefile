LEMURDIR = ../..
include $(LEMURDIR)/MakeDefns

PACKAGE = lemur_jni
VERSION = 1.0

LIBNAMES= retrieval langmod index parsing utility
CONTMODULES=$(LIBNAMES)  contrib/antlr contrib/xpdf

INCPATH = $(patsubst %, -I$(LEMURDIR)/%/include, $(CONTMODULES))

CFLAGS += $(INCPATH)
## GCC 4.0 misoptimizes the wrappers with strict-aliasing
CXXFLAGS += -fno-strict-aliasing $(INCPATH)

LIBPATH=$(patsubst %,-L$(LEMURDIR)/%/obj, $(CONTMODULES))
LIBS=$(patsubst %,-l%, $(LIBNAMES) antlr xpdf)

LIBDIRS=$(patsubst %,$(LEMURDIR)/%/obj/, $(CONTMODULES))
LIBNAMESFULL=$(patsubst %,lib%.a, $(LIBNAMES))
LIBDEPS=$(join $(LIBDIRS), $(LIBNAMESFULL))

LDFLAGS = $(LIBPATH) $(LIBS) $(CPPLDFLAGS)
SRC=interface/lemur.i
INDRI_SRC=interface/indri.i

OBJ=../obj
OUTDIR=$(OBJ)/java/lemurproject/lemur
OUTDIRUI=$(OUTDIR)/ui
JSRC=java/lemurproject/lemur
JSRCUI=$(JSRC)/ui
INDRI_JSRC=java/lemurproject/indri
INDRI_JSRCUI=$(INDRI_JSRC)/ui
INDRI_OUTDIR=$(OBJ)/java/lemurproject/indri
INDRI_OUTDIRUI=$(INDRI_OUTDIR)/ui
JAVASRC = $(notdir $(wildcard $(JSRC)/*.java))
JAVASRCUI = $(notdir $(wildcard $(JSRCUI)/*.java))
INDRI_JAVASRC = $(notdir $(wildcard $(INDRI_JSRC)/*.java))
INDRI_JAVASRCUI = $(notdir $(wildcard $(INDRI_JSRCUI)/*.java))
ALLSRC= $(wildcard $(JSRC)/*.java) $(wildcard $(JSRCUI)/*.java) $(wildcard $(INDRI_JSRC)/*.java) $(wildcard $(INDRI_JSRCUI)/*.java)

OUTDIRPROP=$(OUTDIR)/ui/properties
PROPS = $(patsubst %,$(OUTDIRPROP)/%, $(notdir $(wildcard $(JSRCUI)/properties/*)))
PROPS += $(patsubst %,$(OUTDIRPROP)/docs/%, $(notdir $(wildcard $(JSRCUI)/properties/docs/*)))
PROPS += $(patsubst %,$(OUTDIRPROP)/docs/images/%, $(notdir $(wildcard $(JSRCUI)/properties/docs/images/*)))

INDRI_OUTDIRPROP=$(INDRI_OUTDIR)/ui/properties
INDRI_PROPS = $(patsubst %,$(INDRI_OUTDIRPROP)/%, $(notdir $(wildcard $(INDRI_JSRCUI)/properties/*)))

LEMUR_MFST=java/manifest.lemur
LEMUR_MFSTRET=java/manifest.LemurRet
LEMUR_MFSTIND=java/manifest.LemurIndex
INDRI_MFST=java/manifest.indri
MFSTRET=java/manifest.RetUI
MFSTIND=java/manifest.IndexUI

JARFILES=$(OBJ)/java/indri.jar $(OBJ)/java/RetUI.jar $(OBJ)/java/IndexUI.jar $(OBJ)/java/lemur.jar $(OBJ)/java/LemurRet.jar $(OBJ)/java/LemurIndex.jar

INTERFACES = $(wildcard interface/*.i)

CSHARPSRC=interface/lemur.i
CSHARP_CPP=csharp/lemur_csharp.cpp
INDRI_CSHARPSRC=interface/indri.i
INDRI_CSHARP_CPP=csharp/indri_csharp.cpp

PHPSRC=interface/indri.i
PHPINTERFACES = $(PHPSRC) interface/indritypemaps.i
## get from configure

OUTDIRPHP=$(OBJ)/php
PHPLIB=$(OUTDIRPHP)/libindri_php$(PHP_SO)
PHPLIBPHP=php/libindri_php.php

JAVADIRS = $(OUTDIR) $(INDRI_OUTDIR) $(OUTDIRUI) $(INDRI_OUTDIRUI) $(OUTDIRPROP)/docs/images $(INDRI_OUTDIRPROP) $(OBJ)/java/docs

DIRS=

JAVAOBJ = $(patsubst %,$(OUTDIR)/%, $(JAVASRC:.java=.class)) $(patsubst %,$(INDRI_OUTDIR)/%, $(INDRI_JAVASRC:.java=.class)) $(patsubst %,$(OUTDIRUI)/%, $(JAVASRCUI:.java=.class)) $(patsubst %,$(INDRI_OUTDIRUI)/%, $(INDRI_JAVASRCUI:.java=.class))

SOLIB=$(OBJ)/java/lib$(PACKAGE)$(JAVA_SO)

OUTDIRCSHARP=$(OBJ)/csharp

#naming issues on windows
CSHARPLIB=$(OUTDIRCSHARP)/liblemur_csharp$(CSHARP_SO)
CSHARP_DLL=$(OUTDIRCSHARP)/LemurCsharp.dll
PCK=lemurproject.lemur
INDRI_PCK=lemurproject.indri

DOC_PCK=$(PCK) $(INDRI_PCK) $(PCK).ui $(INDRI_PCK).ui
DOCARGS=-private -author -version

DOC=$(JAVADOC) -classpath java  $(DOCARGS)
DOCDEP=$(OBJ)/java/docs/index.html
TARGETS=
ifeq ($(WITH_SWIG), 1)
  ifeq ($(WITH_JAVA), 1)
    TARGETS += java-lib
    DIRS += $(JAVADIRS)
  endif
  ifeq ($(WITH_PHP), 1)
    TARGETS += php-lib
    DIRS += $(OUTDIRPHP)
  endif
  ifeq ($(WITH_CSHARP), 1)
    TARGETS += csharp-lib
    DIRS += $(OUTDIRCSHARP)
  endif
endif
.PHONY: java-lib php-lib csharp-lib

all: $(TARGETS)

java-lib: $(JAVADIRS) $(PROPS) $(INDRI_PROPS) $(SOLIB) $(JARFILES)

php-lib: $(OUTDIRPHP) $(PHPLIB)

csharp-lib: $(OUTDIRCSHARP) $(CSHARPLIB) $(CSHARP_DLL)

$(SOLIB): $(LIBDEPS)

$(SOLIB): $(OBJ)/java/lemur_jni.o $(OBJ)/java/indri_jni.o  $(OBJ)/java/JBuildIndex.o
	$(CXX) $(JAVA_SHARED)  $(OBJ)/java/lemur_jni.o $(OBJ)/java/indri_jni.o  $(OBJ)/java/JBuildIndex.o -o $@ $(LDFLAGS)

$(OBJ)/java/lemur_jni.o: java/lemur_jni.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(JAVAINC)

$(OBJ)/java/JBuildIndex.o: java/JBuildIndex.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(JAVAINC)

java/lemur_jni.cpp: $(INTERFACES)
ifneq ("$(SWIG)", "")
	$(SWIG) -c++ -java -package $(PCK) -o $@ -outdir $(JSRC) $(SRC) 
endif

$(OBJ)/java/indri_jni.o: java/indri_jni.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(JAVAINC)

java/indri_jni.cpp: $(INTERFACES)
ifneq ("$(SWIG)", "")
	$(SWIG) -c++ -java -package $(INDRI_PCK) -o $@ -outdir $(INDRI_JSRC) $(INDRI_SRC) 
endif

$(CSHARP_CPP): $(INTERFACES)
ifneq ("$(SWIG)", "")
	$(SWIG) -c++ -csharp -namespace Lemur -o $@ -outdir csharp $(CSHARPSRC)
endif

$(OUTDIRCSHARP)/lemur_csharp.o: $(CSHARP_CPP)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(INDRI_CSHARP_CPP): $(INTERFACES)
ifneq ("$(SWIG)", "")
	$(SWIG) -c++ -csharp -dllimport lemur_csharp -namespace Indri -o $@ -outdir csharp $(INDRI_CSHARPSRC) 
endif

$(OUTDIRCSHARP)/indri_csharp.o: $(INDRI_CSHARP_CPP)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

#needs csharp_shared
$(CSHARPLIB): $(LIBDEPS)

$(CSHARPLIB): $(OUTDIRCSHARP)/lemur_csharp.o  $(OUTDIRCSHARP)/indri_csharp.o
	$(CXX) -g $(CSHARP_SHARED) $(OUTDIRCSHARP)/lemur_csharp.o  $(OUTDIRCSHARP)/indri_csharp.o -o $@ $(LDFLAGS)

$(CSHARP_DLL): $(CSHARP_CPP)
	$(MCS) -target:library -out:$@ csharp/*.cs

$(DIRS):
	$(INSTALL_DIR) $@

$(OUTDIR)/%.class:$(JSRC)/%.java
	$(JAVAC) -classpath java -d $(OBJ)/java $<

$(INDRI_OUTDIR)/%.class:$(INDRI_JSRC)/%.java
	$(JAVAC) -classpath java -d $(OBJ)/java $<

$(OUTDIRPROP)/%:$(JSRCUI)/properties/%
	if test -f $< ; then $(INSTALL_DATA) $< $@ ; fi

$(OUTDIRPROP)/docs/%:$(JSRCUI)/properties/docs/%
	if test -f $< ; then $(INSTALL_DATA) $< $@ ; fi

$(OUTDIRPROP)/docs/images/%:$(JSRCUI)/properties/docs/images/%
	if test -f $< ; then $(INSTALL_DATA) $< $@ ; fi

$(INDRI_OUTDIRPROP)/%:$(INDRI_JSRCUI)/properties/%
	if test -f $< ; then $(INSTALL_DATA) $< $@ ; fi


$(DOCDEP): $(ALLSRC)
	$(DOC) -d $(OBJ)/java/docs -doctitle "Lemur JNI" -windowtitle "Lemur JNI" $(DOC_PCK)

$(OBJ)/java/lemur.jar: $(JAVAOBJ) $(LEMUR_MFST) $(DOCDEP)
	rm -f $@
	cd $(OBJ)/java; $(JAR) cmf ../../src/$(LEMUR_MFST) lemur.jar lemurproject docs

$(OBJ)/java/LemurRet.jar: $(JAVAOBJ) $(LEMUR_MFSTRET) $(DOCDEP)
	rm -f $@
	cd $(OBJ)/java; $(JAR) cmf ../../src/$(LEMUR_MFSTRET) LemurRet.jar lemurproject docs

$(OBJ)/java/LemurIndex.jar: $(JAVAOBJ) $(LEMUR_MFSTIND) $(DOCDEP)
	rm -f $@
	cd $(OBJ)/java; $(JAR) cmf ../../src/$(LEMUR_MFSTIND) LemurIndex.jar lemurproject docs

$(OBJ)/java/indri.jar: $(JAVAOBJ) $(INDRI_MFST) $(DOCDEP)
	rm -f $@
	cd $(OBJ)/java; $(JAR) cmf ../../src/$(INDRI_MFST) indri.jar lemurproject docs

$(OBJ)/java/RetUI.jar: $(JAVAOBJ) $(MFSTRET) $(DOCDEP)
	rm -f $@
	cd $(OBJ)/java; $(JAR) cmf ../../src/$(MFSTRET) RetUI.jar lemurproject docs

$(OBJ)/java/IndexUI.jar: $(JAVAOBJ) $(MFSTIND) $(DOCDEP)
	rm -f $@
	cd $(OBJ)/java; $(JAR) cmf ../../src/$(MFSTIND) IndexUI.jar lemurproject docs

## PHP
php/indri_phpwrap.cpp: $(PHPINTERFACES)
ifneq ("$(SWIG)", "")
	$(SWIG) -php5 -c++ -o php/indri_phpwrap.cpp -outdir php $(INCLUDES) $(PHPSRC) 
endif

$(OUTDIRPHP)/indri_phpwrap.o: php/indri_phpwrap.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(PHPINCLUDE)

$(PHPLIB): $(LIBDEPS)

$(PHPLIB): $(OUTDIRPHP)/indri_phpwrap.o 
	$(CXX) -g $(PHP_SHARED) $< -o $@ $(LDFLAGS)


clean:
	rm -rf $(OUTDIRPHP) $(OUTDIRCSHARP) $(OBJ)/java 


install_dirs: $(pkgdatadir) $(pkgdatadir)/JNIdoc $(libdir)
$(pkgdatadir) $(pkgdatadir)/JNIdoc $(libdir):
	$(INSTALL_DIR) $@

install: install_dirs install_data install_docs

install_docs:
ifeq ($(WITH_SWIG), 1)
  ifeq ($(WITH_JAVA), 1)
	cp -pr $(OBJ)/java/docs/* $(pkgdatadir)/JNIdoc
  endif
endif

install_data:
ifeq ($(WITH_SWIG), 1)
  ifeq ($(WITH_JAVA), 1)
	$(INSTALL_DATA)  $(JARFILES) $(pkgdatadir)
	$(INSTALL_PROGRAM) $(SOLIB) $(libdir)
  endif
  ifeq ($(WITH_PHP), 1)
	$(INSTALL_PROGRAM) $(PHPLIB) $(libdir)
	$(INSTALL_DATA) $(PHPLIBPHP) $(pkgdatadir)
  endif
  ifeq ($(WITH_CSHARP), 1)
	$(INSTALL_PROGRAM) $(CSHARPLIB) $(libdir)
	$(INSTALL_PROGRAM) $(CSHARP_DLL) $(libdir)
  endif
endif
