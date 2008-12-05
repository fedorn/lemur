.PHONY: all clean
all: indrisoapd
include ../../MakeDefns
data_dir=$(pkgdatadir)/indri-soap
gsoap_dir = ../gsoap-2.7
MYSRC = indrisoapd.cpp
-include ../depend/indrisoapd.d
indrisoapd.o_INCPATH = -I../include $(patsubst %, -I../../%/include,  retrieval index parsing utility) $(patsubst %, -I../../contrib/%/include,  xpdf antlr)
indrisoapd_LIBPATH = $(patsubst %, -L../../%/obj,  retrieval index parsing utility) $(patsubst %, -L../../contrib/%/obj,  xpdf antlr)
indrisoapd_LIBFLAGS = $(patsubst %, -l%,  retrieval index parsing utility  xpdf antlr)

# how to generate .d  file from a .C file
../depend/%.d: ../src/%.cpp
	@echo "Generating $@ for $<"
	@set -e; $(CXX) -MM $(CXXFLAGS) -I$(gsoap_dir)/gsoap $(indrisoapd.o_INCPATH) $< | sed 's/\($*\)\.o[ :]*/\1.o \.\.\/depend\/\1.d : /g' > $@; [ -s $@ ] || rm -f $@
###
SOAPHEADER = ../src/indrisoap.hpp

AM_CFLAGS = 
AM_CXXFLAGS = 
DOC_FILES=../doc/indrisoapd.param ../doc/indrisoapd.txt
INCLUDES = -I$(gsoap_dir)/gsoap
SOAP = $(gsoap_dir)/gsoap/src/soapcpp2
SOAP_H_FILES = ../src/soapH.h ../src/soapStub.h 
SOAP_FLAGS = -x -S -I$(gsoap_dir)/gsoap/import
SOAP_CPP_SERVER = ../src/soapC.cpp
SOAP_CPP_LIB = $(gsoap_dir)/gsoap/libgsoap++.a
SOAP_NS = ../src/indrisoap.nsmap ../src/indrisoap.wsdl
SOAP_CPP_SRC = ../src/soapServer.cpp $(SOAP_H_FILES)
LDADD = -lgsoap++
BUILT_SOURCES = $(SOAP_CPP_SRC) 
CLEANFILES = $(SOAP_CPP_SRC) soapServerLib.cpp soapC.cpp soapindrisoapObject.h *.req.xml *.res.xml *.wsdl *.nsmap *.xsd *Proxy\.h *~

PROGRAMS = indrisoapd
indriapi_OBJECTS = indrisoapd.o soapServer.o soapC.o
indriapi_LDADD = $(LDADD)  $(CPPLDFLAGS)
indriapi_DEPENDENCIES = $(gsoap_dir)/gsoap/libgsoap++.a
indriapi_LDFLAGS =-L$(gsoap_dir)/gsoap

DEFS = -DHAVE_CONFIG_H
DEFAULT_INCLUDES =  -I.  -I$(gsoap_dir) $($@_INCPATH) 
CXXCOMPILE = $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)  -UPACKAGE_NAME -UPACKAGE_TARNAME -UPACKAGE_VERSION -UPACKAGE_STRING -UPACKAGE_BUGREPORT
CXXLD = $(CXX)
CXXLINK = $(CXXLD) $(AM_CXXFLAGS) $(CXXFLAGS) $(AM_LDFLAGS) $(LDFLAGS) \
	-o $@

COMPILE = $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) \
	$(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
CCLD = $(CC)
LINK = $(CXXLD) $(AM_CFLAGS)  $(AM_LDFLAGS) $(LDFLAGS) -o $@

indrisoapd$(EXEEXT): $(indriapi_DEPENDENCIES) $(BUILT_SOURCES)  $(indriapi_OBJECTS)
	@rm -f indrisoapd$(EXEEXT)
	$(LINK) $(indriapi_OBJECTS) $(indriapi_LDFLAGS) $($@_LIBPATH) $($@_LIBFLAGS) $(indriapi_LDADD) $(LIBS)

%.o: ../src/%.c
	$(COMPILE) -c $<

%.o: ../src/%.cpp
	$(CXXCOMPILE) -c $<

distclean:
	rm -rf $(CLEANFILES) $(PROGRAMS) $(DEPDIR) *.o
clean:
	rm -rf $(PROGRAMS) *.o
	$(MAKE) -C $(gsoap_dir) clean

$(gsoap_dir)/config.h:
	cd $(gsoap_dir) ; ./configure
$(gsoap_dir)/gsoap/libgsoap++.a: $(gsoap_dir)/config.h
	$(MAKE) -C $(gsoap_dir)

$(SOAP_CPP_SERVER) $(SOAP_CPP_SRC) : $(SOAPHEADER)
	cd ../src; $(SOAP)  $(SOAP_FLAGS) $< 
#install data in share...
install: $(data_dir) install_bin install_data
$(data_dir):
	$(INSTALL_DIR) $@
install_data:
	$(INSTALL_DATA)  $(SOAP_NS) $(DOC_FILES) $(data_dir)
install_bin:
	$(INSTALL_PROGRAM) $(PROGRAMS)  $(bindir)
