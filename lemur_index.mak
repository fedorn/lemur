# Microsoft Developer Studio Generated NMAKE File, Based on lemur_index.dsp
!IF "$(CFG)" == ""
CFG=LEMUR_INDEX - WIN32 RELEASE
!MESSAGE No configuration specified. Defaulting to LEMUR_INDEX - WIN32 RELEASE.
!ENDIF 

!IF "$(CFG)" != "lemur_index - Win32 Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lemur_index.mak" CFG="LEMUR_INDEX - WIN32 RELEASE"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lemur_index - Win32 Release" (based on "Win32 (x86) Static Library")
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
OUTDIR=.\index\obj
INTDIR=.\index\obj

ALL : ".\lemur_index.lib"


CLEAN :
	-@erase "$(INTDIR)\BasicDocInfoList.obj"
	-@erase "$(INTDIR)\BasicIndex.obj"
	-@erase "$(INTDIR)\BasicIndexWithCat.obj"
	-@erase "$(INTDIR)\BasicTermInfoList.obj"
	-@erase "$(INTDIR)\GammaCompress.obj"
	-@erase "$(INTDIR)\IncFPPushIndex.obj"
	-@erase "$(INTDIR)\IncFPTextHandler.obj"
	-@erase "$(INTDIR)\IncPassagePushIndex.obj"
	-@erase "$(INTDIR)\IndexCount.obj"
	-@erase "$(INTDIR)\IndexManager.obj"
	-@erase "$(INTDIR)\IndexProb.obj"
	-@erase "$(INTDIR)\InvDocList.obj"
	-@erase "$(INTDIR)\InvFPDocList.obj"
	-@erase "$(INTDIR)\InvFPIndex.obj"
	-@erase "$(INTDIR)\InvFPIndexMerge.obj"
	-@erase "$(INTDIR)\InvFPPushIndex.obj"
	-@erase "$(INTDIR)\InvFPTermList.obj"
	-@erase "$(INTDIR)\InvFPTermPropList.obj"
	-@erase "$(INTDIR)\InvFPTextHandler.obj"
	-@erase "$(INTDIR)\InvIndex.obj"
	-@erase "$(INTDIR)\InvIndexMerge.obj"
	-@erase "$(INTDIR)\InvPassagePushIndex.obj"
	-@erase "$(INTDIR)\InvPushIndex.obj"
	-@erase "$(INTDIR)\InvTermList.obj"
	-@erase "$(INTDIR)\KeyfileDocListSegmentReader.obj"
	-@erase "$(INTDIR)\KeyfileIncIndex.obj"
	-@erase "$(INTDIR)\KeyfileTextHandler.obj"
	-@erase "$(INTDIR)\PropIndexTH.obj"
	-@erase "$(INTDIR)\QueryTextHandler.obj"
	-@erase "$(INTDIR)\Terms.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase ".\lemur_index.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\lemur_index.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"lemur_index.lib" 
LIB32_OBJS= \
	"$(INTDIR)\BasicDocInfoList.obj" \
	"$(INTDIR)\BasicIndex.obj" \
	"$(INTDIR)\BasicIndexWithCat.obj" \
	"$(INTDIR)\BasicTermInfoList.obj" \
	"$(INTDIR)\GammaCompress.obj" \
	"$(INTDIR)\IncFPPushIndex.obj" \
	"$(INTDIR)\IncFPTextHandler.obj" \
	"$(INTDIR)\IncPassagePushIndex.obj" \
	"$(INTDIR)\IndexCount.obj" \
	"$(INTDIR)\IndexManager.obj" \
	"$(INTDIR)\IndexProb.obj" \
	"$(INTDIR)\InvDocList.obj" \
	"$(INTDIR)\InvFPDocList.obj" \
	"$(INTDIR)\InvFPIndex.obj" \
	"$(INTDIR)\InvFPIndexMerge.obj" \
	"$(INTDIR)\InvFPPushIndex.obj" \
	"$(INTDIR)\InvFPTermList.obj" \
	"$(INTDIR)\InvFPTermPropList.obj" \
	"$(INTDIR)\InvFPTextHandler.obj" \
	"$(INTDIR)\InvIndex.obj" \
	"$(INTDIR)\InvIndexMerge.obj" \
	"$(INTDIR)\InvPassagePushIndex.obj" \
	"$(INTDIR)\InvPushIndex.obj" \
	"$(INTDIR)\InvTermList.obj" \
	"$(INTDIR)\KeyfileDocListSegmentReader.obj" \
	"$(INTDIR)\KeyfileIncIndex.obj" \
	"$(INTDIR)\KeyfileTextHandler.obj" \
	"$(INTDIR)\PropIndexTH.obj" \
	"$(INTDIR)\QueryTextHandler.obj" \
	"$(INTDIR)\Terms.obj"

".\lemur_index.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

CPP_PROJ=/nologo /ML /W3 /GR /GX /O2 /I "utility\include" /I "index\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\lemur_index.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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


!IF "$(CFG)" == "lemur_index - Win32 Release"
SOURCE=.\index\src\BasicDocInfoList.cpp

"$(INTDIR)\BasicDocInfoList.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\index\src\BasicIndex.cpp

"$(INTDIR)\BasicIndex.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\index\src\BasicIndexWithCat.cpp

"$(INTDIR)\BasicIndexWithCat.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\index\src\BasicTermInfoList.cpp

"$(INTDIR)\BasicTermInfoList.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\index\src\GammaCompress.cpp

"$(INTDIR)\GammaCompress.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\index\src\IncFPPushIndex.cpp

"$(INTDIR)\IncFPPushIndex.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\index\src\IncFPTextHandler.cpp

"$(INTDIR)\IncFPTextHandler.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\index\src\IncPassagePushIndex.cpp

"$(INTDIR)\IncPassagePushIndex.obj" : $(SOURCE) "$(INTDIR)"
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


SOURCE=.\index\src\InvDocList.cpp

"$(INTDIR)\InvDocList.obj" : $(SOURCE) "$(INTDIR)"
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


SOURCE=.\index\src\InvFPTermPropList.cpp

"$(INTDIR)\InvFPTermPropList.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\index\src\InvFPTextHandler.cpp

"$(INTDIR)\InvFPTextHandler.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\index\src\InvIndex.cpp

"$(INTDIR)\InvIndex.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\index\src\InvIndexMerge.cpp

"$(INTDIR)\InvIndexMerge.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\index\src\InvPassagePushIndex.cpp

"$(INTDIR)\InvPassagePushIndex.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\index\src\InvPushIndex.cpp

"$(INTDIR)\InvPushIndex.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\index\src\InvTermList.cpp

"$(INTDIR)\InvTermList.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\index\src\KeyfileDocListSegmentReader.cpp

"$(INTDIR)\KeyfileDocListSegmentReader.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\index\src\KeyfileIncIndex.cpp

"$(INTDIR)\KeyfileIncIndex.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\index\src\KeyfileTextHandler.cpp

"$(INTDIR)\KeyfileTextHandler.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\index\src\PropIndexTH.cpp

"$(INTDIR)\PropIndexTH.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\index\src\QueryTextHandler.cpp

"$(INTDIR)\QueryTextHandler.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\index\src\Terms.cpp

"$(INTDIR)\Terms.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

