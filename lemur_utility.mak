# Microsoft Developer Studio Generated NMAKE File, Based on lemur_utility.dsp
!IF "$(CFG)" == ""
CFG=lemur_utility - Win32 Release
!MESSAGE No configuration specified. Defaulting to lemur_utility - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "lemur_utility - Win32 Release" && "$(CFG)" != "lemur_utility - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lemur_utility.mak" CFG="lemur_utility - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lemur_utility - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "lemur_utility - Win32 Debug" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "lemur_utility - Win32 Release"

OUTDIR=.\utility\obj
INTDIR=.\utility\obj

ALL : ".\lemur_utility.lib"


CLEAN :
	-@erase "$(INTDIR)\BasicDocStream.obj"
	-@erase "$(INTDIR)\ByteHash.obj"
	-@erase "$(INTDIR)\error.obj"
	-@erase "$(INTDIR)\FlattextDocMgr.obj"
	-@erase "$(INTDIR)\FUtil.obj"
	-@erase "$(INTDIR)\lex_parser.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\MemCache.obj"
	-@erase "$(INTDIR)\MemList.obj"
	-@erase "$(INTDIR)\Param.obj"
	-@erase "$(INTDIR)\parameters.obj"
	-@erase "$(INTDIR)\porter_stemmer.obj"
	-@erase "$(INTDIR)\PorterStemmer.obj"
	-@erase "$(INTDIR)\ReutersParser.obj"
	-@erase "$(INTDIR)\RVLCompress.obj"
	-@erase "$(INTDIR)\Stopper.obj"
	-@erase "$(INTDIR)\string-set.obj"
	-@erase "$(INTDIR)\String.obj"
	-@erase "$(INTDIR)\Timer.obj"
	-@erase "$(INTDIR)\TrecParser.obj"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\WebParser.obj"
	-@erase "$(INTDIR)\WordSet.obj"
	-@erase "$(INTDIR)\WriterTextHandler.obj"
	-@erase ".\lemur_utility.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "utility\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\lemur_utility.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\lemur_utility.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"lemur_utility.lib" 
LIB32_OBJS= \
	"$(INTDIR)\BasicDocStream.obj" \
	"$(INTDIR)\ByteHash.obj" \
	"$(INTDIR)\error.obj" \
	"$(INTDIR)\FlattextDocMgr.obj" \
	"$(INTDIR)\FUtil.obj" \
	"$(INTDIR)\lex_parser.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\MemCache.obj" \
	"$(INTDIR)\MemList.obj" \
	"$(INTDIR)\Param.obj" \
	"$(INTDIR)\parameters.obj" \
	"$(INTDIR)\porter_stemmer.obj" \
	"$(INTDIR)\PorterStemmer.obj" \
	"$(INTDIR)\ReutersParser.obj" \
	"$(INTDIR)\RVLCompress.obj" \
	"$(INTDIR)\Stopper.obj" \
	"$(INTDIR)\String.obj" \
	"$(INTDIR)\string-set.obj" \
	"$(INTDIR)\Timer.obj" \
	"$(INTDIR)\TrecParser.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\WebParser.obj" \
	"$(INTDIR)\WordSet.obj" \
	"$(INTDIR)\WriterTextHandler.obj"

".\lemur_utility.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "lemur_utility - Win32 Debug"

OUTDIR=.\utility\obj
INTDIR=.\utility\obj

ALL : ".\lemur_utility.lib"


CLEAN :
	-@erase "$(INTDIR)\BasicDocStream.obj"
	-@erase "$(INTDIR)\ByteHash.obj"
	-@erase "$(INTDIR)\error.obj"
	-@erase "$(INTDIR)\FlattextDocMgr.obj"
	-@erase "$(INTDIR)\FUtil.obj"
	-@erase "$(INTDIR)\lex_parser.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\MemCache.obj"
	-@erase "$(INTDIR)\MemList.obj"
	-@erase "$(INTDIR)\Param.obj"
	-@erase "$(INTDIR)\parameters.obj"
	-@erase "$(INTDIR)\porter_stemmer.obj"
	-@erase "$(INTDIR)\PorterStemmer.obj"
	-@erase "$(INTDIR)\ReutersParser.obj"
	-@erase "$(INTDIR)\RVLCompress.obj"
	-@erase "$(INTDIR)\Stopper.obj"
	-@erase "$(INTDIR)\string-set.obj"
	-@erase "$(INTDIR)\String.obj"
	-@erase "$(INTDIR)\Timer.obj"
	-@erase "$(INTDIR)\TrecParser.obj"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\WebParser.obj"
	-@erase "$(INTDIR)\WordSet.obj"
	-@erase "$(INTDIR)\WriterTextHandler.obj"
	-@erase ".\lemur_utility.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "utility\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\lemur_utility.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ  /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\lemur_utility.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"lemur_utility.lib" 
LIB32_OBJS= \
	"$(INTDIR)\BasicDocStream.obj" \
	"$(INTDIR)\ByteHash.obj" \
	"$(INTDIR)\error.obj" \
	"$(INTDIR)\FlattextDocMgr.obj" \
	"$(INTDIR)\FUtil.obj" \
	"$(INTDIR)\lex_parser.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\MemCache.obj" \
	"$(INTDIR)\MemList.obj" \
	"$(INTDIR)\Param.obj" \
	"$(INTDIR)\parameters.obj" \
	"$(INTDIR)\porter_stemmer.obj" \
	"$(INTDIR)\PorterStemmer.obj" \
	"$(INTDIR)\ReutersParser.obj" \
	"$(INTDIR)\RVLCompress.obj" \
	"$(INTDIR)\Stopper.obj" \
	"$(INTDIR)\String.obj" \
	"$(INTDIR)\string-set.obj" \
	"$(INTDIR)\Timer.obj" \
	"$(INTDIR)\TrecParser.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\WebParser.obj" \
	"$(INTDIR)\WordSet.obj" \
	"$(INTDIR)\WriterTextHandler.obj"

".\lemur_utility.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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
!IF EXISTS("lemur_utility.dep")
!INCLUDE "lemur_utility.dep"
!ELSE 
!MESSAGE Warning: cannot find "lemur_utility.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "lemur_utility - Win32 Release" || "$(CFG)" == "lemur_utility - Win32 Debug"
SOURCE=.\utility\src\BasicDocStream.cpp

"$(INTDIR)\BasicDocStream.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\ByteHash.cpp

"$(INTDIR)\ByteHash.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\error.c

"$(INTDIR)\error.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\FlattextDocMgr.cpp

"$(INTDIR)\FlattextDocMgr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\FUtil.cpp

"$(INTDIR)\FUtil.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\lex_parser.cpp

"$(INTDIR)\lex_parser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\main.cpp

"$(INTDIR)\main.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\MemCache.cpp

"$(INTDIR)\MemCache.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\MemList.cpp

"$(INTDIR)\MemList.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\Param.cpp

"$(INTDIR)\Param.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\parameters.c

"$(INTDIR)\parameters.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\porter_stemmer.cpp

"$(INTDIR)\porter_stemmer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\PorterStemmer.cpp

"$(INTDIR)\PorterStemmer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\ReutersParser.cpp

"$(INTDIR)\ReutersParser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\RVLCompress.cpp

"$(INTDIR)\RVLCompress.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\Stopper.cpp

"$(INTDIR)\Stopper.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=".\utility\src\string-set.c"

"$(INTDIR)\string-set.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\String.cpp

"$(INTDIR)\String.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\Timer.cpp

"$(INTDIR)\Timer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\TrecParser.cpp

"$(INTDIR)\TrecParser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\util.c

"$(INTDIR)\util.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\WebParser.cpp

"$(INTDIR)\WebParser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\WordSet.cpp

"$(INTDIR)\WordSet.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\WriterTextHandler.cpp

"$(INTDIR)\WriterTextHandler.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

