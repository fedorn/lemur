#define this module
MODULE	= antlr
include ../../../MakeContrib
INCDIR = $(includedir)/$(MODULE)
ALLHEADER = $(wildcard ../include/antlr/*.h*)
install:
	$(INSTALL_DIR) $(INCDIR)
	$(INSTALL_DATA) $(ALLHEADER) $(INCDIR)
	$(AR) -qs $(libdir)/$(LEMURLIB) $(OBJ)
