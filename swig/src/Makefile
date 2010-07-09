LEMURDIR = ../..
include $(LEMURDIR)/MakeDefns

PACKAGE = lemur_jni
VERSION = 1.0

LIBNAMES= retrieval index parsing utility
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
INDRI_SRC=interface/indri.i

OBJ=../obj
INDRI_JSRC=java/lemurproject/indri
INDRI_JSRCUI=$(INDRI_JSRC)/ui
INDRI_OUTDIR=$(OBJ)/java/lemurproject/indri
INDRI_OUTDIRUI=$(INDRI_OUTDIR)/ui
INDRI_JAVASRC = $(notdir $(wildcard $(INDRI_JSRC)/*.java))
INDRI_JAVASRCUI = $(notdir $(wildcard $(INDRI_JSRCUI)/*.java))
ALLSRC= $(wildcard $(INDRI_JSRC)/*.java) $(wildcard $(INDRI_JSRCUI)/*.java)
INDRI_OUTDIRPROP=$(INDRI_OUTDIR)/ui/properties
INDRI_PROPS = $(patsubst %,$(INDRI_OUTDIRPROP)/%, $(notdir $(wildcard $(INDRI_JSRCUI)/properties/*)))

INDRI_MFST=java/manifest.indri
MFSTRET=java/manifest.RetUI
MFSTIND=java/manifest.IndexUI

JARFILES=$(OBJ)/java/indri.jar $(OBJ)/java/RetUI.jar $(OBJ)/java/IndexUI.jar

INTERFACES = $(wildcard interface/*.i)

INDRI_CSHARPSRC=interface/indri.i
INDRI_CSHARP_CPP=csharp/indri_csharp.cpp

PHPSRC=interface/indri.i
PHPINTERFACES = $(PHPSRC) interface/indritypemaps.i
## get from configure

OUTDIRPHP=$(OBJ)/php
PHPLIB=$(OUTDIRPHP)/libindri_php$(PHP_SO)
PHPLIBPHP=php/libindri_php.php

JAVADIRS = $(INDRI_OUTDIR) $(INDRI_OUTDIRUI)  $(INDRI_OUTDIRPROP) $(OBJ)/java/docs
DIRS=
JAVAOBJ = $(patsubst %,$(INDRI_OUTDIR)/%, $(INDRI_JAVASRC:.java=.class)) $(patsubst %,$(INDRI_OUTDIRUI)/%, $(INDRI_JAVASRCUI:.java=.class))

SOLIB=$(OBJ)/java/lib$(PACKAGE)$(JAVA_SO)

OUTDIRCSHARP=$(OBJ)/csharp

#naming issues on windows
CSHARPLIB=$(OUTDIRCSHARP)/liblemur_csharp$(CSHARP_SO)
CSHARP_DLL=$(OUTDIRCSHARP)/LemurCsharp.dll
INDRI_PCK=lemurproject.indri

DOC_PCK=$(INDRI_PCK) $(INDRI_PCK).ui
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

$(SOLIB): $(OBJ)/java/indri_jni.o
	$(CXX) $(JAVA_SHARED)  $(OBJ)/java/indri_jni.o  -o $@ $(LDFLAGS)

$(OBJ)/java/indri_jni.o: java/indri_jni.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(JAVAINC)

java/indri_jni.cpp: $(INTERFACES)
ifneq ("$(SWIG)", "")
	$(SWIG) -c++ -java -package $(INDRI_PCK) -o $@ -outdir $(INDRI_JSRC) $(INDRI_SRC) 
endif

$(INDRI_CSHARP_CPP): $(INTERFACES)
ifneq ("$(SWIG)", "")
	$(SWIG) -c++ -csharp -dllimport lemur_csharp -namespace Indri -o $@ -outdir csharp $(INDRI_CSHARPSRC) 
endif

$(OUTDIRCSHARP)/indri_csharp.o: $(INDRI_CSHARP_CPP)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

#needs csharp_shared
$(CSHARPLIB): $(LIBDEPS)

$(CSHARPLIB): $(OUTDIRCSHARP)/indri_csharp.o
	$(CXX) -g $(CSHARP_SHARED) $(OUTDIRCSHARP)/indri_csharp.o -o $@ $(LDFLAGS)

$(CSHARP_DLL): $(CSHARP_CPP)
	$(MCS) -target:library -out:$@ csharp/*.cs

$(DIRS):
	$(INSTALL_DIR) $@

$(INDRI_OUTDIR)/%.class:$(INDRI_JSRC)/%.java
	$(JAVAC) -classpath java -d $(OBJ)/java $<

$(INDRI_OUTDIRPROP)/%:$(INDRI_JSRCUI)/properties/%
	if test -f $< ; then $(INSTALL_DATA) $< $@ ; fi

$(DOCDEP): $(ALLSRC)
	$(DOC) -d $(OBJ)/java/docs -doctitle "Lemur JNI" -windowtitle "Lemur JNI" $(DOC_PCK)

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
