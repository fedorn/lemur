# Microsoft Developer Studio Generated NMAKE File, Based on lemur_index.dsp
!IF "$(CFG)" == ""
CFG=lemur_index - Win32 Release
!MESSAGE No configuration specified. Defaulting to lemur_index - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "lemur_index - Win32 Release" && "$(CFG)" != "lemur_index - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lemur_index.mak" CFG="lemur_index - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lemur_index - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "lemur_index - Win32 Debug" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "lemur_index - Win32 Release"

OUTDIR=.\index\obj
INTDIR=.\index\obj

ALL : ".\lemur_index.lib"


CLEAN :
	-@erase "$(INTDIR)\BasicIndex.obj"
	-@erase "$(INTDIR)\BasicIndexWithCat.obj"
	-@erase "$(INTDIR)\GammaCompress.obj"
	-@erase "$(INTDIR)\IndexCount.obj"
	-@erase "$(INTDIR)\IndexManager.obj"
	-@erase "$(INTDIR)\IndexProb.obj"
	-@erase "$(INTDIR)\InvFPDocList.obj"
	-@erase "$(INTDIR)\InvFPIndex.obj"
	-@erase "$(INTDIR)\InvFPIndexMerge.obj"
	-@erase "$(INTDIR)\InvFPPushIndex.obj"
	-@erase "$(INTDIR)\InvFPTermList.obj"
	-@erase "$(INTDIR)\InvFPTextHandler.obj"
	-@erase "$(INTDIR)\MemList.obj"
	-@erase "$(INTDIR)\QueryTextHandler.obj"
	-@erase "$(INTDIR)\Terms.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase ".\lemur_index.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "index\include" /I "utility\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\lemur_index.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\lemur_index.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"lemur_index.lib" 
LIB32_OBJS= \
	"$(INTDIR)\BasicIndex.obj" \
	"$(INTDIR)\BasicIndexWithCat.obj" \
	"$(INTDIR)\GammaCompress.obj" \
	"$(INTDIR)\IndexCount.obj" \
	"$(INTDIR)\IndexManager.obj" \
	"$(INTDIR)\IndexProb.obj" \
	"$(INTDIR)\InvFPDocList.obj" \
	"$(INTDIR)\InvFPIndex.obj" \
	"$(INTDIR)\InvFPIndexMerge.obj" \
	"$(INTDIR)\InvFPPushIndex.obj" \
	"$(INTDIR)\InvFPTermList.obj" \
	"$(INTDIR)\InvFPTextHandler.obj" \
	"$(INTDIR)\MemList.obj" \
	"$(INTDIR)\QueryTextHandler.obj" \
	"$(INTDIR)\Terms.obj"

".\lemur_index.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "lemur_index - Win32 Debug"

OUTDIR=.\index\obj
INTDIR=.\index\obj

ALL : ".\lemur_index.lib"


CLEAN :
	-@erase "$(INTDIR)\BasicIndex.obj"
	-@erase "$(INTDIR)\BasicIndexWithCat.obj"
	-@erase "$(INTDIR)\GammaCompress.obj"
	-@erase "$(INTDIR)\IndexCount.obj"
	-@erase "$(INTDIR)\IndexManager.obj"
	-@erase "$(INTDIR)\IndexProb.obj"
	-@erase "$(INTDIR)\InvFPDocList.obj"
	-@erase "$(INTDIR)\InvFPIndex.obj"
	-@erase "$(INTDIR)\InvFPIndexMerge.obj"
	-@erase "$(INTDIR)\InvFPPushIndex.obj"
	-@erase "$(INTDIR)\InvFPTermList.obj"
	-@erase "$(INTDIR)\InvFPTextHandler.obj"
	-@erase "$(INTDIR)\MemList.obj"
	-@erase "$(INTDIR)\QueryTextHandler.obj"
	-@erase "$(INTDIR)\Terms.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase ".\lemur_index.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "index\include" /I "utility\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\lemur_index.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ  /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\lemur_index.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"lemur_index.lib" 
LIB32_OBJS= \
	"$(INTDIR)\BasicIndex.obj" \
	"$(INTDIR)\BasicIndexWithCat.obj" \
	"$(INTDIR)\GammaCompress.obj" \
	"$(INTDIR)\IndexCount.obj" \
	"$(INTDIR)\IndexManager.obj" \
	"$(INTDIR)\IndexProb.obj" \
	"$(INTDIR)\InvFPDocList.obj" \
	"$(INTDIR)\InvFPIndex.obj" \
	"$(INTDIR)\InvFPIndexMerge.obj" \
	"$(INTDIR)\InvFPPushIndex.obj" \
	"$(INTDIR)\InvFPTermList.obj" \
	"$(INTDIR)\InvFPTextHandler.obj" \
	"$(INTDIR)\MemList.obj" \
	"$(INTDIR)\QueryTextHandler.obj" \
	"$(INTDIR)\Terms.obj"

".\lemur_index.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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
!IF EXISTS("lemur_index.dep")
!INCLUDE "lemur_index.dep"
!ELSE 
!MESSAGE Warning: cannot find "lemur_index.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "lemur_index - Win32 Release" || "$(CFG)" == "lemur_index - Win32 Debug"
SOURCE=.\index\src\BasicIndex.cpp

"$(INTDIR)\BasicIndex.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\index\src\BasicIndexWithCat.cpp

"$(INTDIR)\BasicIndexWithCat.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\index\src\GammaCompress.cpp

"$(INTDIR)\GammaCompress.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\index\src\IndexCount.cpp

"$(INTDIR)\IndexCount.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\index\src\IndexManager.cpp

"$(INTDIR)\IndexManager.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\index\src\IndexProb.cpp

"$(INTDIR)\IndexProb.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\index\src\InvFPDocList.cpp

"$(INTDIR)\InvFPDocList.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\index\src\InvFPIndex.cpp

"$(INTDIR)\InvFPIndex.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\index\src\InvFPIndexMerge.cpp

"$(INTDIR)\InvFPIndexMerge.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\index\src\InvFPPushIndex.cpp

"$(INTDIR)\InvFPPushIndex.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\index\src\InvFPTermList.cpp

"$(INTDIR)\InvFPTermList.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\index\src\InvFPTextHandler.cpp

"$(INTDIR)\InvFPTextHandler.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\index\src\MemList.cpp

"$(INTDIR)\MemList.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\index\src\QueryTextHandler.cpp

"$(INTDIR)\QueryTextHandler.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\index\src\Terms.cpp

"$(INTDIR)\Terms.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

