
# this line specifies all the subdirectories
# and should be the only line you want to change
# add the directory name here when you add a new
# directory to lemur

DIRS = eval utility index retrieval app test


# IN GENERAL, DO NOT CHANGE THE FOLLOWING LINES

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

