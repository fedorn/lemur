LEMURDIR = ../..
include $(LEMURDIR)/MakeDefns
# if WITH_JAVA is 0, nothing will be made.
# run configure with --enable-java to set the values for JAVAC and JAR
ifeq ($(WITH_JAVA), 1)
  PACKAGE=ireval
  VERSION=1.0
  OBJ=../obj
  OUTDIR=$(OBJ)/$(PACKAGE)
  OUTDIRPROP=$(OUTDIR)/properties
  JSRC=$(PACKAGE)
  JAVASRC = $(wildcard $(JSRC)/*.java)
  JAVAOBJ = $(patsubst %,$(OBJ)/%, $(JAVASRC:.java=.class))
  PROPS = $(patsubst %,$(OUTDIRPROP)/%, $(notdir $(wildcard $(JSRC)/properties/*)))
  MFST=manifest.mf
  JARFILES=$(OBJ)/ireval.jar
  GUIMFST=IrEvalGUI.mf
  JARGUI=$(OBJ)/irevalGUI.jar
  JAVADIRS = $(OBJ) $(OBJ)/javadoc $(OUTDIRPROP)
  PCK=$(PACKAGE)
  DOC_PCK=$(PCK) $(PCK).stat
  DOCARGS=-private -author -version
  DOC=$(JAVADOC)   -classpath .:$(CHART) $(DOCARGS)
  DOCDEP=$(OBJ)/javadoc/index.html
  DIRS += $(JAVADIRS)
  CHART=jfreechart-1.0.11.jar:jcommon-1.0.14.jar
  JFREEJARS=jfreechart-1.0.11.jar jcommon-1.0.14.jar
endif

all: $(DIRS) $(PROPS) $(JARFILES) $(JARGUI)

$(DIRS):
	$(INSTALL_DIR) $@

$(OUTDIRPROP)/%:$(JSRC)/properties/%
	if test -f $< ; then $(INSTALL_DATA) $< $@ ; fi

$(OUTDIR)/%.class:$(JSRC)/%.java
	$(JAVAC) -classpath .:$(CHART) -Xlint:unchecked -d $(OBJ) $<

$(DOCDEP): $(JAVASRC)
	$(DOC) -d $(OBJ)/javadoc -doctitle "Ireval" -windowtitle "Ireval" $(DOC_PCK)

$(JARFILES): $(JAVAOBJ) $(MFST) $(DOCDEP)
	rm -f $@
	cd $(OBJ); $(JAR) cmf ../src/$(MFST) ireval.jar ireval javadoc
$(JARGUI): $(JAVAOBJ) $(GUIMFST) $(DOCDEP)
	rm -f $@
	cd $(OBJ); $(JAR) cmf ../src/$(GUIMFST) irevalGUI.jar ireval javadoc

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
	$(INSTALL_DATA)  $(JARFILES) $(JARGUI) $(JFREEJARS) $(pkgdatadir)
endif