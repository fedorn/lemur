LEMURDIR = ../..
include $(LEMURDIR)/MakeDefns
# if WITH_JAVA is 0, nothing will be made.
# run configure with --enable-java to set the values for JAVAC and JAR
ifeq ($(WITH_JAVA), 1)
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
  DOC=$(JAVADOC)   -classpath . $(DOCARGS)
  DOCDEP=$(OBJ)/javadoc/index.html
  DIRS += $(JAVADIRS)
endif

all: $(DIRS) $(JARFILES)

$(DIRS):
	$(INSTALL_DIR) $@

$(OUTDIR)/%.class:$(JSRC)/%.java
	$(JAVAC) -classpath . -Xlint:unchecked -d $(OBJ) $<

$(DOCDEP): $(JAVASRC)
	$(DOC) -d $(OBJ)/javadoc -doctitle "Ireval" -windowtitle "Ireval" $(DOC_PCK)

$(JARFILES): $(JAVAOBJ) $(MFST) $(DOCDEP)
	rm -f $@
	cd $(OBJ); $(JAR) cmf ../src/$(MFST) ireval.jar ireval javadoc

clean:
	rm -rf $(OBJ)


install_dirs: $(pkgdatadir) $(pkgdatadir)/ireval_doc $(libdir)
$(pkgdatadir) $(pkgdatadir)/ireval_doc $(libdir):
	$(INSTALL_DIR) $@

install: install_dirs install_data install_docs

install_docs:
ifeq ($(WITH_JAVA), 1)
	cp -pr $(OBJ)/javadoc/* $(pkgdatadir)/ireval_doc
endif
install_data:
ifeq ($(WITH_JAVA), 1)
	$(INSTALL_DATA)  $(JARFILES) $(pkgdatadir)
endif