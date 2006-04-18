include ../../MakeDefns
MYSRC = $(patsubst %, %.cpp, $(ALLBIN))

.PHONY: all clean install
all: $(ALLBIN)

clean:
	rm -f *.o; rm -f ../depend/*.d; rm -f $(ALLBIN)

ifneq (clean, $(MAKECMDGOALS))
ifneq (install, $(MAKECMDGOALS))
ifneq ($(strip $(MYSRC)),)
-include $(patsubst %, ../depend/%, $(MYSRC:.cpp=.d))
endif
## there are no C apps, but this needs to be addressed.
ifneq ($(strip $(CSRC)),)
-include $(patsubst %, ../depend/%, $(CSRC:.c=.d))
endif
endif
endif

## override default dependency rule to add individual library dependencies.
## use -MM to omit system header files.
# how to generate .d  file from a .C file
../depend/%.d: ../src/%.cpp
	@echo "Generating $@ for $<"
	@set -e; $(CXX) -MM $(CXXFLAGS) $(ALLINCPATH) $< | sed 's/\($*\)\.o[ :]*/\1.o \.\.\/depend\/\1.d : /g' > $@; [ -s $@ ] || rm -f $@
	@DEPS="" ; XLIB=""; for mod in $(ALLMODULES) ; do if `grep "../../$$mod/include" $@ >/dev/null` ; then echo "$*: ../../$$mod/obj/lib$$mod.a" >> $@;  DEPS="$$DEPS $$mod" ; fi ; done ; if `grep "../../parsing/include" $@ >/dev/null` ; then echo "$*: ../../contrib/xpdf/obj/libxpdf.a" >> $@; XLIB="$$XLIB xpdf" ; fi ; if `grep "../../retrieval/include" $@ >/dev/null` ; then echo "$*: ../../contrib/antlr/obj/libantlr.a" >> $@; XLIB="$$XLIB antlr";  fi ; DEPS="$$DEPS" ; echo "$*.o_INCPATH = -I../include $$""(patsubst %, -I../../%/include, $$DEPS) $$""(patsubst %, -I../../contrib/%/include, $$XLIB)" >> $@; echo "$*_LIBPATH = $$""(patsubst %, -L../../%/obj, $$DEPS) $$""(patsubst %, -L../../contrib/%/obj, $$XLIB)" >> $@; DEPS="$$DEPS $$XLIB" ; echo "$*_LIBFLAGS = $$""(patsubst %, -l%, $$DEPS)" >> $@;

# how to generate .d file from a .c file
../depend/%.d: ../src/%.c
	@echo "Generating $@ for $<"
	@set -e; $(CC) -MM $(CFLAGS) $(ALLINCPATH) $< | sed 's/\($*\)\.o[ :]*/\1.o \.\.\/depend\/\1.d : /g' > $@; [ -s $@ ] || rm -f $@
	@DEPS="" ; XLIB=""; for mod in $(ALLMODULES) ; do if `grep "../../$$mod/include" $@ >/dev/null` ; then echo "$*: ../../$$mod/obj/lib$$mod.a" >> $@;  DEPS="$$DEPS $$mod" ; fi ; done ; if `grep "../../parsing/include" $@ >/dev/null` ; then echo "$*: ../../contrib/xpdf/obj/libxpdf.a" >> $@; XLIB="$$XLIB xpdf" ; fi ; if `grep "../../retrieval/include" $@ >/dev/null` ; then echo "$*: ../../contrib/antlr/obj/libantlr.a" >> $@; XLIB="$$XLIB antlr";  fi ; DEPS="$$DEPS" ; echo "$*.o_INCPATH = -I../include $$""(patsubst %, -I../../%/include, $$DEPS) $$""(patsubst %, -I../../contrib/%/include, $$XLIB)" >> $@; echo "$*_LIBPATH = $$""(patsubst %, -L../../%/obj, $$DEPS) $$""(patsubst %, -L../../contrib/%/obj, $$XLIB)" >> $@; DEPS="$$DEPS $$XLIB" ; echo "$*_LIBFLAGS = $$""(patsubst %, -l%, $$DEPS)" >> $@;

# how to make an application

# compiling C++ code
%.o: ../src/%.cpp
	$(CXX) $(CXXFLAGS) $($@_INCPATH) -o $@ -c $<

# compiling C code
%.o: ../src/%.c
	$(CC) $(CFLAGS) $($@_INCPATH) -o $@ -c $<

%: %.o
	$(CXX) -o $@ $< $($@_LIBPATH) $($@_LIBFLAGS) $(CPPLDFLAGS)

install:
	$(INSTALL_PROGRAM) $(ALLBIN)  $(bindir)
