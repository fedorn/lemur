SHELL = /bin/sh

# The following two lines specify all the subdirectories
# and should be the only line you want to change
# add the directory name here when you add a new module  or application directory to lemur
# IMPORTANT: Make sure the order reflects the dependency!
#            It is usually safe to add your module directory at the end of all
#            other module directories

# module ("library") directories
LIBDIRS = utility index langmod retrieval distrib summarization

# application directories
APPDIRS = app

DIRS = $(LIBDIRS) $(APPDIRS)

# IN GENERAL, DO NOT CHANGE THE FOLLOWING LINES

# derived targets for cleaning directories
CLEANDIRS = $(DIRS:%=clean%)

# derived targets for installing directories
INSTALLDIRS = $(DIRS:%=install%)


# derived targets for sub directories
OBJDIRS = $(DIRS:%=%/obj)
DEPENDDIRS = $(DIRS:%=%/depend)

.PHONY: $(DIRS) $(CLEANDIRS) $(INSTALLDIRS) CLEANDEST DATAINSTALL

# "make all" means ensuring obj and depend subdirs to exist and making each directory
all All ALL: $(DIRS) 

# "make lib" means to ensure that obj and depend subdirs to exist and make each module/library directory
lib Lib LIB: $(LIBDIRS)

# Create OBJDIRS and DEPENDDIRS if necessary
$(OBJDIRS) $(DEPENDDIRS):
	mkdir $@

# how to make each directory
# for more verbose output, remove the "-s" below
$(DIRS): $(OBJDIRS) $(DEPENDDIRS)
	cd $@/obj && $(MAKE) -s -f ../src/Makefile all

# "make clean" means cleaning each directory
clean Clean CLEAN: $(CLEANDIRS)

# how to clean each directory
$(CLEANDIRS):
	cd $(patsubst clean%,%,$@)/obj && $(MAKE) -s -f ../src/Makefile clean

# "make install" means creating destination directories and installing each module and application directory
install Install INSTALL:  $(LEMUR_INSTALL_PATH)/lemur $(LEMUR_INSTALL_PATH)/lemur/include $(LEMUR_INSTALL_PATH)/lemur/lib  $(LEMUR_INSTALL_PATH)/lemur/bin $(LEMUR_INSTALL_PATH)/lemur/kstem_data $(LEMUR_INSTALL_PATH)/lemur/arabic_stem_data  CLEANDEST $(INSTALLDIRS) DATAINSTALL

# how to make the installation directory
$(LEMUR_INSTALL_PATH)/lemur $(LEMUR_INSTALL_PATH)/lemur/include $(LEMUR_INSTALL_PATH)/lemur/lib $(LEMUR_INSTALL_PATH)/lemur/bin $(LEMUR_INSTALL_PATH)/lemur/kstem_data $(LEMUR_INSTALL_PATH)/lemur/arabic_stem_data:
	mkdir $@; echo $@ created

#clean the destination directories
CLEANDEST:
	rm -f $(LEMUR_INSTALL_PATH)/lemur/include/* ;\
	rm -f $(LEMUR_INSTALL_PATH)/lemur/lib/*	; \
	rm -f $(LEMUR_INSTALL_PATH)/lemur/bin/*

# how to install each directory
$(INSTALLDIRS):
	cd $(patsubst install%,%,$@)/obj && $(MAKE) -s -f ../src/Makefile install

DATAINSTALL:
	cp data/kstem_data/*.txt $(LEMUR_INSTALL_PATH)/lemur/kstem_data 
	cp data/arabic_stem_data/*.txt $(LEMUR_INSTALL_PATH)/lemur/arabic_stem_data
