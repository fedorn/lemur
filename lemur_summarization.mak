# Microsoft Developer Studio Generated NMAKE File, Based on lemur_summarization.dsp
!IF "$(CFG)" == ""
CFG=lemur_summarization - Win32 Release
!MESSAGE No configuration specified. Defaulting to lemur_summarization - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "lemur_summarization - Win32 Release" && "$(CFG)" != "lemur_summarization - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lemur_summarization.mak" CFG="lemur_summarization - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lemur_summarization - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "lemur_summarization - Win32 Debug" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "lemur_summarization - Win32 Release"

OUTDIR=.\summarization\obj
INTDIR=.\summarization\obj

ALL : ".\lemur_summarization.lib"


CLEAN :
	-@erase "$(INTDIR)\BasicPassage.obj"
	-@erase "$(INTDIR)\BasicSumm.obj"
	-@erase "$(INTDIR)\MMRPassage.obj"
	-@erase "$(INTDIR)\MMRSumm.obj"
	-@erase "$(INTDIR)\Passage.obj"
	-@erase "$(INTDIR)\Summarizer.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase ".\lemur_summarization.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "summarization\include" /I "index\include" /I "utility\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\lemur_summarization.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\lemur_summarization.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"lemur_summarization.lib" 
LIB32_OBJS= \
	"$(INTDIR)\BasicPassage.obj" \
	"$(INTDIR)\BasicSumm.obj" \
	"$(INTDIR)\MMRPassage.obj" \
	"$(INTDIR)\MMRSumm.obj" \
	"$(INTDIR)\Passage.obj" \
	"$(INTDIR)\Summarizer.obj"

".\lemur_summarization.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "lemur_summarization - Win32 Debug"

OUTDIR=.\summarization\obj
INTDIR=.\summarization\obj

ALL : ".\lemur_summarization.lib"


CLEAN :
	-@erase "$(INTDIR)\BasicPassage.obj"
	-@erase "$(INTDIR)\BasicSumm.obj"
	-@erase "$(INTDIR)\MMRPassage.obj"
	-@erase "$(INTDIR)\MMRSumm.obj"
	-@erase "$(INTDIR)\Passage.obj"
	-@erase "$(INTDIR)\Summarizer.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase ".\lemur_summarization.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "summarization\include" /I "index\include" /I "utility\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\lemur_summarization.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ  /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\lemur_summarization.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"lemur_summarization.lib" 
LIB32_OBJS= \
	"$(INTDIR)\BasicPassage.obj" \
	"$(INTDIR)\BasicSumm.obj" \
	"$(INTDIR)\MMRPassage.obj" \
	"$(INTDIR)\MMRSumm.obj" \
	"$(INTDIR)\Passage.obj" \
	"$(INTDIR)\Summarizer.obj"

".\lemur_summarization.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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
!IF EXISTS("lemur_summarization.dep")
!INCLUDE "lemur_summarization.dep"
!ELSE 
!MESSAGE Warning: cannot find "lemur_summarization.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "lemur_summarization - Win32 Release" || "$(CFG)" == "lemur_summarization - Win32 Debug"
SOURCE=.\summarization\src\BasicPassage.cpp

"$(INTDIR)\BasicPassage.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\summarization\src\BasicSumm.cpp

"$(INTDIR)\BasicSumm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\summarization\src\MMRPassage.cpp

"$(INTDIR)\MMRPassage.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\summarization\src\MMRSumm.cpp

"$(INTDIR)\MMRSumm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\summarization\src\Passage.cpp

"$(INTDIR)\Passage.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\summarization\src\Summarizer.cpp

"$(INTDIR)\Summarizer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

