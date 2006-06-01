LEMURDIR = ../..
include $(LEMURDIR)/MakeDefns
LIBNAMES= $(ALLMODULES) antlr xpdf

INCPATH = $(patsubst %, -I$(LEMURDIR)/%/include, $(CONTMODULES))

CFLAGS += $(INCPATH)
CXXFLAGS += $(INCPATH)

LIBPATH=$(patsubst %,-L$(LEMURDIR)/%/obj, $(CONTMODULES))
LIBS=$(patsubst %,-l%, $(LIBNAMES))

LIBDIRS=$(patsubst %,$(LEMURDIR)/%/obj/, $(CONTMODULES))
LIBNAMESFULL=$(patsubst %,lib%.a, $(LIBNAMES))
LIBDEPS=$(join $(LIBDIRS), $(LIBNAMESFULL))

LDFLAGS = $(LIBPATH) $(LIBS) $(CPPLDFLAGS)

OBJS = LemurCGI.o CGIConfiguration.o CGIOutput.o DBInterface.o DictionaryHash.o SingleResultItem.o
PROG = bin/lemur.cgi

all: $(PROG)

$(PROG): $(OBJS) $(LIBDEPS)
	$(CXX)  -o $@ $(OBJS) $(LDFLAGS)

clean:
	rm -f $(PROG) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

## target specific dependencies
LemurCGI.o: LemurCGI.cpp LemurSearchCGIConstants.h CGIConfiguration.h CGIOutput.h DictionaryHash.h DBInterface.h
CGIConfiguration.o: CGIConfiguration.cpp CGIConfiguration.h LemurSearchCGIConstants.h
CGIOutput.o: CGIOutput.cpp CGIOutput.h LemurSearchCGIConstants.h CGIConfiguration.h DictionaryHash.h SingleResultItem.h
DBInterface.o: DBInterface.cpp DBInterface.h CGIOutput.h
DictionaryHash.o: DictionaryHash.cpp DictionaryHash.h
SingleResultItem.o: SingleResultItem.cpp SingleResultItem.h LemurSearchCGIConstants.h DictionaryHash.h

