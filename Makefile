SHELL = /bin/sh

# this line specifies all the subdirectories
# and should be the only line you want to change
# add the directory name here when you add a new module  or application directory to lemur
# IMPORTANT: Make sure the order reflects the dependency!
#            It is usually safe to add your module directory at the end of all
#            other module directories but before any application directory,
#            and add your application directory at the end of the whole list
DIRS = utility index langmod retrieval app


# IN GENERAL, DO NOT CHANGE THE FOLLOWING LINES

# derived targets for cleaning directories
CLEANDIRS = $(DIRS:%=clean%)

# derived targets for installing directories
INSTALLDIRS = $(DIRS:%=install%)


# derived targets for sub directories
OBJDIRS = $(DIRS:%=%/obj)
DEPENDDIRS = $(DIRS:%=%/depend)

.PHONY: $(DIRS) $(CLEANDIRS) $(INSTALLDIRS) CLEANDEST 

# "make all" means ensuring obj and depend subdirs to exist and making each directory
all All ALL: $(DIRS) 

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
install Install INSTALL:  $(LEMUR_INSTALL_PATH)/lemur $(LEMUR_INSTALL_PATH)/lemur/include $(LEMUR_INSTALL_PATH)/lemur/lib  $(LEMUR_INSTALL_PATH)/lemur/bin  CLEANDEST $(INSTALLDIRS)

# how to make the installation directory
$(LEMUR_INSTALL_PATH)/lemur $(LEMUR_INSTALL_PATH)/lemur/include $(LEMUR_INSTALL_PATH)/lemur/lib $(LEMUR_INSTALL_PATH)/lemur/bin:
	mkdir $@; echo $@ created

#clean the destination directories
CLEANDEST:
	rm -f $(LEMUR_INSTALL_PATH)/lemur/include/* ;\
	rm -f $(LEMUR_INSTALL_PATH)/lemur/lib/*	; \
	rm -f $(LEMUR_INSTALL_PATH)/lemur/bin/*

# how to install each directory
$(INSTALLDIRS):
	cd $(patsubst install%,%,$@)/obj && $(MAKE) -s -f ../src/Makefile install




