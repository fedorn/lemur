# Microsoft Developer Studio Generated NMAKE File, Based on lemur_langmod.dsp
!IF "$(CFG)" == ""
CFG=lemur_langmod - Win32 Release
!MESSAGE No configuration specified. Defaulting to lemur_langmod - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "lemur_langmod - Win32 Release" && "$(CFG)" != "lemur_langmod - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lemur_langmod.mak" CFG="lemur_langmod - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lemur_langmod - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "lemur_langmod - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "lemur_langmod - Win32 Release"

OUTDIR=.\langmod\obj
INTDIR=.\langmod\obj

ALL : ".\lemur_langmod.lib"


CLEAN :
	-@erase "$(INTDIR)\DocUnigramCounter.obj"
	-@erase "$(INTDIR)\OneStepMarkovChain.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase ".\lemur_langmod.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GR /GX /O2 /I "index\include" /I "utility\include" /I "langmod\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\lemur_langmod.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\lemur_langmod.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"lemur_langmod.lib" 
LIB32_OBJS= \
	"$(INTDIR)\DocUnigramCounter.obj" \
	"$(INTDIR)\OneStepMarkovChain.obj"

".\lemur_langmod.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "lemur_langmod - Win32 Debug"

OUTDIR=.\langmod\obj
INTDIR=.\langmod\obj

ALL : ".\lemur_langmod.lib"


CLEAN :
	-@erase "$(INTDIR)\DocUnigramCounter.obj"
	-@erase "$(INTDIR)\OneStepMarkovChain.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase ".\lemur_langmod.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GR /GX /ZI /Od /I "index\include" /I "utility\include" /I "langmod\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\lemur_langmod.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\lemur_langmod.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"lemur_langmod.lib" 
LIB32_OBJS= \
	"$(INTDIR)\DocUnigramCounter.obj" \
	"$(INTDIR)\OneStepMarkovChain.obj"

".\lemur_langmod.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("lemur_langmod.dep")
!INCLUDE "lemur_langmod.dep"
!ELSE 
!MESSAGE Warning: cannot find "lemur_langmod.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "lemur_langmod - Win32 Release" || "$(CFG)" == "lemur_langmod - Win32 Debug"
SOURCE=.\langmod\src\DocUnigramCounter.cpp

"$(INTDIR)\DocUnigramCounter.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\langmod\src\OneStepMarkovChain.cpp

"$(INTDIR)\OneStepMarkovChain.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

