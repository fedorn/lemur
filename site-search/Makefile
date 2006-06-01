LEMURDIR = ..
include $(LEMURDIR)/MakeDefns

SUBDIRS=php cgi

all: 
	$(MAKE) -C php all
	$(MAKE) -C cgi all
clean:
	$(MAKE) -C php clean
	$(MAKE) -C cgi clean

install:
	$(MAKE) -C php install
	$(MAKE) -C cgi install
