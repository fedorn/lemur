# Microsoft Developer Studio Generated NMAKE File, Based on lemur_distrib.dsp
!IF "$(CFG)" == ""
CFG=lemur_distrib - Win32 Release
!MESSAGE No configuration specified. Defaulting to lemur_distrib - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "lemur_distrib - Win32 Release" && "$(CFG)" != "lemur_distrib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lemur_distrib.mak" CFG="lemur_distrib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lemur_distrib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "lemur_distrib - Win32 Debug" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "lemur_distrib - Win32 Release"

OUTDIR=.\distrib\obj
INTDIR=.\distrib\obj

ALL : ".\lemur_distrib.lib"


CLEAN :
	-@erase "$(INTDIR)\CORIMergeMethod.obj"
	-@erase "$(INTDIR)\CtfIndexer.obj"
	-@erase "$(INTDIR)\DistMergeMethod.obj"
	-@erase "$(INTDIR)\DistSearchMethod.obj"
	-@erase "$(INTDIR)\DocFreqIndexer.obj"
	-@erase "$(INTDIR)\DocScore.obj"
	-@erase "$(INTDIR)\FreqCounter.obj"
	-@erase "$(INTDIR)\LemurDBManager.obj"
	-@erase "$(INTDIR)\LemurMemParser.obj"
	-@erase "$(INTDIR)\MultiRegrMergeMethod.obj"
	-@erase "$(INTDIR)\QryBasedSampler.obj"
	-@erase "$(INTDIR)\SingleRegrMergeMethod.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase ".\lemur_distrib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GR /GX /O2 /I "utility\include" /I "index\include" /I "retrieval\include" /I "langmod\include" /I "distrib\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\lemur_distrib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\lemur_distrib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"lemur_distrib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\CORIMergeMethod.obj" \
	"$(INTDIR)\CtfIndexer.obj" \
	"$(INTDIR)\DistMergeMethod.obj" \
	"$(INTDIR)\DistSearchMethod.obj" \
	"$(INTDIR)\DocFreqIndexer.obj" \
	"$(INTDIR)\DocScore.obj" \
	"$(INTDIR)\FreqCounter.obj" \
	"$(INTDIR)\LemurDBManager.obj" \
	"$(INTDIR)\LemurMemParser.obj" \
	"$(INTDIR)\MultiRegrMergeMethod.obj" \
	"$(INTDIR)\QryBasedSampler.obj" \
	"$(INTDIR)\SingleRegrMergeMethod.obj"

".\lemur_distrib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "lemur_distrib - Win32 Debug"

OUTDIR=.\distrib\obj
INTDIR=.\distrib\obj

ALL : ".\lemur_distrib.lib"


CLEAN :
	-@erase "$(INTDIR)\CORIMergeMethod.obj"
	-@erase "$(INTDIR)\CtfIndexer.obj"
	-@erase "$(INTDIR)\DistMergeMethod.obj"
	-@erase "$(INTDIR)\DistSearchMethod.obj"
	-@erase "$(INTDIR)\DocFreqIndexer.obj"
	-@erase "$(INTDIR)\DocScore.obj"
	-@erase "$(INTDIR)\FreqCounter.obj"
	-@erase "$(INTDIR)\LemurDBManager.obj"
	-@erase "$(INTDIR)\LemurMemParser.obj"
	-@erase "$(INTDIR)\MultiRegrMergeMethod.obj"
	-@erase "$(INTDIR)\QryBasedSampler.obj"
	-@erase "$(INTDIR)\SingleRegrMergeMethod.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase ".\lemur_distrib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GR /GX /ZI /Od /I "utility\include" /I "index\include" /I "retrieval\include" /I "langmod\include" /I "distrib\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\lemur_distrib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ  /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\lemur_distrib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"lemur_distrib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\CORIMergeMethod.obj" \
	"$(INTDIR)\CtfIndexer.obj" \
	"$(INTDIR)\DistMergeMethod.obj" \
	"$(INTDIR)\DistSearchMethod.obj" \
	"$(INTDIR)\DocFreqIndexer.obj" \
	"$(INTDIR)\DocScore.obj" \
	"$(INTDIR)\FreqCounter.obj" \
	"$(INTDIR)\LemurDBManager.obj" \
	"$(INTDIR)\LemurMemParser.obj" \
	"$(INTDIR)\MultiRegrMergeMethod.obj" \
	"$(INTDIR)\QryBasedSampler.obj" \
	"$(INTDIR)\SingleRegrMergeMethod.obj"

".\lemur_distrib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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
!IF EXISTS("lemur_distrib.dep")
!INCLUDE "lemur_distrib.dep"
!ELSE 
!MESSAGE Warning: cannot find "lemur_distrib.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "lemur_distrib - Win32 Release" || "$(CFG)" == "lemur_distrib - Win32 Debug"
SOURCE=.\distrib\src\CORIMergeMethod.cpp

"$(INTDIR)\CORIMergeMethod.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\distrib\src\CtfIndexer.cpp

"$(INTDIR)\CtfIndexer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\distrib\src\DistMergeMethod.cpp

"$(INTDIR)\DistMergeMethod.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\distrib\src\DistSearchMethod.cpp

"$(INTDIR)\DistSearchMethod.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\distrib\src\DocFreqIndexer.cpp

"$(INTDIR)\DocFreqIndexer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\distrib\src\DocScore.cpp

"$(INTDIR)\DocScore.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\distrib\src\FreqCounter.cpp

"$(INTDIR)\FreqCounter.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\distrib\src\LemurDBManager.cpp

"$(INTDIR)\LemurDBManager.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\distrib\src\LemurMemParser.cpp

"$(INTDIR)\LemurMemParser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\distrib\src\MultiRegrMergeMethod.cpp

"$(INTDIR)\MultiRegrMergeMethod.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\distrib\src\QryBasedSampler.cpp

"$(INTDIR)\QryBasedSampler.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\distrib\src\SingleRegrMergeMethod.cpp

"$(INTDIR)\SingleRegrMergeMethod.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

