
# define the binary executables

# C++ applications
CPPAPPBIN = $(CPPSRC:.cpp=)

# C applications
CAPPBIN = $(CSRC:.c=)

# define dependent modules
# Assuming dependence on all library modules. In the future
# each application should have its own dependent list (to be fixed later)

DEPENDMODS = retrieval index utility eval

#IMPORTANT: MakeDefns should be included before MakeRules

# include common definitions
include ../../MakeDefns

# specify what it means by "make all" and "make clean"

all: $(CPPAPPBIN) $(CAPPBIN)

clean: cleanapp

# include common rules
include ../../MakeRules

# how to make an application

%: %.o $(OBJLIBS)
	$(CXX) -o $@ $< $(INCPATH) $(CPPLDFLAGS)




