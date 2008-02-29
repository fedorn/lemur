LEMURDIR = ../..
include $(LEMURDIR)/MakeDefns

PACKAGE=ireval
VERSION=1.0
OBJ=../obj
OUTDIR=$(OBJ)/$(PACKAGE)
JSRC=$(PACKAGE)
JAVASRC = $(wildcard $(JSRC)/*.java)
JAVAOBJ = $(patsubst %,$(OBJ)/%, $(JAVASRC:.java=.class))
MFST=manifest.mf
JARFILES=$(OBJ)/ireval.jar
JAVADIRS = $(OBJ) $(OBJ)/javadoc
PCK=$(PACKAGE)
DOC_PCK=$(PCK) $(PCK).stat
DOCARGS=-private -author -version
DOC=$(JAVADOC)   $(DOCARGS)
DOCDEP=$(OBJ)/javadoc/index.html
DIRS += $(JAVADIRS)

all: $(DIRS) $(JARFILES)

$(DIRS):
	$(INSTALL_DIR) $@

$(OUTDIR)/%.class:$(JSRC)/%.java
	$(JAVAC) -d $(OBJ) $<

$(DOCDEP): $(JAVASRC)
	$(DOC) -d $(OBJ)/javadoc -doctitle "Ireval" -windowtitle "Ireval" $(DOC_PCK)

$(JARFILES): $(JAVAOBJ) $(MFST) $(DOCDEP)
	rm -f $@
	cd $(OBJ); $(JAR) cmf ../src/$(MFST) ireval.jar ireval javadoc

clean:
	rm -rf $(OBJ)


install_dirs: $(pkgdatadir) $(pkgdatadir)/JNIdoc $(libdir)
$(pkgdatadir) $(pkgdatadir)/JNIdoc $(libdir):
	$(INSTALL_DIR) $@

install: install_dirs install_data install_docs
#fixme
install_docs:
ifeq ($(WITH_SWIG), 1)
  ifeq ($(WITH_JAVA), 1)
	cp -pr $(OBJ)/java/docs/* $(pkgdatadir)/JNIdoc
  endif
endif

install_data:
	$(INSTALL_DATA)  $(JARFILES) $(pkgdatadir)
