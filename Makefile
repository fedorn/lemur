
# these two lines specifies all the subdirectories
# and should be the only lines you want to change

# add the directory name here when you add a new MODULE
# directory to lemur

MODDIRS = eval cl_utility utility cl_index index classify retrieval

# add the directory name here when you add a new APPLICATION
# directory to lemur

APPDIRS = cl_app app test

DIRS = $(MODDIRS) $(APPDIRS)


# IN GENERAL, DO NOT CHANGE THE FOLLOWING LINES


MODLIBS = $(foreach dir, $(MODDIRS), $(dir)/src/lib$(dir).a)

CINCS = $(foreach dir, $(MODDIRS), $(shell ls $(dir)/include/*.h))
CPPINCS = $(foreach dir, $(MODDIRS), $(shell ls $(dir)/include/*.hpp))

ALLINCS = $(CINCS) $(CPPINCS)

INSTALLINCS = $(ALLINCS:%=install%)

CLEANDIRS = $(DIRS:%=clean%)

.PHONY: $(DIRS) $(CLEANDIRS)

# "make all" means making each directory
all: $(DIRS)

# how to make each directory
$(DIRS): 
	cd $@/src && $(MAKE) all

# "make clean" means cleaning each directory
clean: $(CLEANDIRS)

# how to clean each directory
$(CLEANDIRS):
	cd $(patsubst clean%,%,$@)/src && $(MAKE) clean


# how to install LEMUR lib and header files
install Install INSTALL:  $(LEMUR_INSTALL_PATH)/lemur $(LEMUR_INSTALL_PATH)/lemur/include $(LEMUR_INSTALL_PATH)/lemur/lib  $(INSTALLINCS) InstallLib

# how to make the installation directory
$(LEMUR_INSTALL_PATH)/lemur $(LEMUR_INSTALL_PATH)/lemur/include $(LEMUR_INSTALL_PATH)/lemur/lib:
	mkdir $@; echo $@ created

# how to install LEMUR library
InstallLib:
	ar -rs $(LEMUR_INSTALL_PATH)/lemur/lib/liblemur.a $(MODLIBS)


# how to install header files

$(INSTALLINCS):
	cp $(patsubst install%, %,$@) $(LEMUR_INSTALL_PATH)/lemur/include/


