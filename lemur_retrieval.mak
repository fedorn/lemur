# Microsoft Developer Studio Generated NMAKE File, Based on lemur_retrieval.dsp
!IF "$(CFG)" == ""
CFG=lemur_retrieval - Win32 Release
!MESSAGE No configuration specified. Defaulting to lemur_retrieval - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "lemur_retrieval - Win32 Release" && "$(CFG)" != "lemur_retrieval - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lemur_retrieval.mak" CFG="lemur_retrieval - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lemur_retrieval - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "lemur_retrieval - Win32 Debug" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "lemur_retrieval - Win32 Release"

OUTDIR=.\retrieval\obj
INTDIR=.\retrieval\obj

ALL : ".\lemur_retrieval.lib"


CLEAN :
	-@erase "$(INTDIR)\CORIRetMethod.obj"
	-@erase "$(INTDIR)\FreqVector.obj"
	-@erase "$(INTDIR)\IndexedReal.obj"
	-@erase "$(INTDIR)\OkapiRetMethod.obj"
	-@erase "$(INTDIR)\ResultFile.obj"
	-@erase "$(INTDIR)\RetMethodManager.obj"
	-@erase "$(INTDIR)\RetrievalMethod.obj"
	-@erase "$(INTDIR)\ScoreAccumulator.obj"
	-@erase "$(INTDIR)\SimpleKLRetMethod.obj"
	-@erase "$(INTDIR)\TextQueryRep.obj"
	-@erase "$(INTDIR)\TextQueryRetMethod.obj"
	-@erase "$(INTDIR)\TFIDFRetMethod.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase ".\lemur_retrieval.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "retrieval\include" /I "langmod\include" /I "index\include" /I "utility\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\lemur_retrieval.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\lemur_retrieval.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"lemur_retrieval.lib" 
LIB32_OBJS= \
	"$(INTDIR)\CORIRetMethod.obj" \
	"$(INTDIR)\FreqVector.obj" \
	"$(INTDIR)\IndexedReal.obj" \
	"$(INTDIR)\OkapiRetMethod.obj" \
	"$(INTDIR)\ResultFile.obj" \
	"$(INTDIR)\RetMethodManager.obj" \
	"$(INTDIR)\RetrievalMethod.obj" \
	"$(INTDIR)\ScoreAccumulator.obj" \
	"$(INTDIR)\SimpleKLRetMethod.obj" \
	"$(INTDIR)\TextQueryRep.obj" \
	"$(INTDIR)\TextQueryRetMethod.obj" \
	"$(INTDIR)\TFIDFRetMethod.obj"

".\lemur_retrieval.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "lemur_retrieval - Win32 Debug"

OUTDIR=.\retrieval\obj
INTDIR=.\retrieval\obj

ALL : ".\lemur_retrieval.lib"


CLEAN :
	-@erase "$(INTDIR)\CORIRetMethod.obj"
	-@erase "$(INTDIR)\FreqVector.obj"
	-@erase "$(INTDIR)\IndexedReal.obj"
	-@erase "$(INTDIR)\OkapiRetMethod.obj"
	-@erase "$(INTDIR)\ResultFile.obj"
	-@erase "$(INTDIR)\RetMethodManager.obj"
	-@erase "$(INTDIR)\RetrievalMethod.obj"
	-@erase "$(INTDIR)\ScoreAccumulator.obj"
	-@erase "$(INTDIR)\SimpleKLRetMethod.obj"
	-@erase "$(INTDIR)\TextQueryRep.obj"
	-@erase "$(INTDIR)\TextQueryRetMethod.obj"
	-@erase "$(INTDIR)\TFIDFRetMethod.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase ".\lemur_retrieval.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "retrieval\include" /I "langmod\include" /I "index\include" /I "utility\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\lemur_retrieval.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ  /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\lemur_retrieval.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"lemur_retrieval.lib" 
LIB32_OBJS= \
	"$(INTDIR)\CORIRetMethod.obj" \
	"$(INTDIR)\FreqVector.obj" \
	"$(INTDIR)\IndexedReal.obj" \
	"$(INTDIR)\OkapiRetMethod.obj" \
	"$(INTDIR)\ResultFile.obj" \
	"$(INTDIR)\RetMethodManager.obj" \
	"$(INTDIR)\RetrievalMethod.obj" \
	"$(INTDIR)\ScoreAccumulator.obj" \
	"$(INTDIR)\SimpleKLRetMethod.obj" \
	"$(INTDIR)\TextQueryRep.obj" \
	"$(INTDIR)\TextQueryRetMethod.obj" \
	"$(INTDIR)\TFIDFRetMethod.obj"

".\lemur_retrieval.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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
!IF EXISTS("lemur_retrieval.dep")
!INCLUDE "lemur_retrieval.dep"
!ELSE 
!MESSAGE Warning: cannot find "lemur_retrieval.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "lemur_retrieval - Win32 Release" || "$(CFG)" == "lemur_retrieval - Win32 Debug"
SOURCE=.\retrieval\src\CORIRetMethod.cpp

"$(INTDIR)\CORIRetMethod.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\retrieval\src\FreqVector.cpp

"$(INTDIR)\FreqVector.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\retrieval\src\IndexedReal.cpp

"$(INTDIR)\IndexedReal.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\retrieval\src\OkapiRetMethod.cpp

"$(INTDIR)\OkapiRetMethod.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\retrieval\src\ResultFile.cpp

"$(INTDIR)\ResultFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\retrieval\src\RetMethodManager.cpp

"$(INTDIR)\RetMethodManager.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\retrieval\src\RetrievalMethod.cpp

"$(INTDIR)\RetrievalMethod.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\retrieval\src\ScoreAccumulator.cpp

"$(INTDIR)\ScoreAccumulator.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\retrieval\src\SimpleKLRetMethod.cpp

"$(INTDIR)\SimpleKLRetMethod.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\retrieval\src\TextQueryRep.cpp

"$(INTDIR)\TextQueryRep.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\retrieval\src\TextQueryRetMethod.cpp

"$(INTDIR)\TextQueryRetMethod.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\retrieval\src\TFIDFRetMethod.cpp

"$(INTDIR)\TFIDFRetMethod.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

