#define this module
MODULE	= xpdf
include ../../../MakeContrib
INCDIR = $(includedir)/$(MODULE)
ALLHEADER = $(wildcard ../include/*.h*)
ALLHEADER += $(wildcard ../include/*.xpm)
install:
	$(INSTALL_DIR) $(INCDIR)
	$(INSTALL_DATA) $(ALLHEADER) $(INCDIR)
	$(AR) -qs $(libdir)/$(LEMURLIB) $(OBJ)
