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
	-@erase "$(INTDIR)\arabic_stem.obj"
	-@erase "$(INTDIR)\ArabicParser.obj"
	-@erase "$(INTDIR)\ArabicStemmer.obj"
	-@erase "$(INTDIR)\BasicDocStream.obj"
	-@erase "$(INTDIR)\BrillPOSParser.obj"
	-@erase "$(INTDIR)\BrillPOSTokenizer.obj"
	-@erase "$(INTDIR)\ByteHash.obj"
	-@erase "$(INTDIR)\ChineseCharParser.obj"
	-@erase "$(INTDIR)\ChineseParser.obj"
	-@erase "$(INTDIR)\DocMgrManager.obj"
	-@erase "$(INTDIR)\error.obj"
	-@erase "$(INTDIR)\FlattextDocMgr.obj"
	-@erase "$(INTDIR)\FUtil.obj"
	-@erase "$(INTDIR)\IdentifinderParser.obj"
	-@erase "$(INTDIR)\InqArabicParser.obj"
	-@erase "$(INTDIR)\InQueryOpParser.obj"
	-@erase "$(INTDIR)\Keyfile.obj"
	-@erase "$(INTDIR)\keyfilecode.obj"
	-@erase "$(INTDIR)\KeyfileDocMgr.obj"
	-@erase "$(INTDIR)\keyprint.obj"
	-@erase "$(INTDIR)\kstem.obj"
	-@erase "$(INTDIR)\KStemmer.obj"
	-@erase "$(INTDIR)\lex_parser.obj"
	-@erase "$(INTDIR)\LinkedPropertyList.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\MemCache.obj"
	-@erase "$(INTDIR)\MemList.obj"
	-@erase "$(INTDIR)\Param.obj"
	-@erase "$(INTDIR)\parameters.obj"
	-@erase "$(INTDIR)\Parser.obj"
	-@erase "$(INTDIR)\porter_stemmer.obj"
	-@erase "$(INTDIR)\PorterStemmer.obj"
	-@erase "$(INTDIR)\Property.obj"
	-@erase "$(INTDIR)\QueryDocument.obj"
	-@erase "$(INTDIR)\ReadBuffer.obj"
	-@erase "$(INTDIR)\ReutersParser.obj"
	-@erase "$(INTDIR)\RVLCompress.obj"
	-@erase "$(INTDIR)\Stopper.obj"
	-@erase "$(INTDIR)\string-set.obj"
	-@erase "$(INTDIR)\String.obj"
	-@erase "$(INTDIR)\TermCache.obj"
	-@erase "$(INTDIR)\TextHandlerManager.obj"
	-@erase "$(INTDIR)\Timer.obj"
	-@erase "$(INTDIR)\TrecParser.obj"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\WebParser.obj"
	-@erase "$(INTDIR)\WordSet.obj"
	-@erase "$(INTDIR)\WriteBuffer.obj"
	-@erase "$(INTDIR)\WriterInQueryHandler.obj"
	-@erase "$(INTDIR)\WriterTextHandler.obj"
	-@erase ".\lemur_utility.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GR /GX /O2 /I "utility\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\lemur_utility.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\lemur_utility.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"lemur_utility.lib" 
LIB32_OBJS= \
	"$(INTDIR)\arabic_stem.obj" \
	"$(INTDIR)\ArabicParser.obj" \
	"$(INTDIR)\ArabicStemmer.obj" \
	"$(INTDIR)\BasicDocStream.obj" \
	"$(INTDIR)\BrillPOSParser.obj" \
	"$(INTDIR)\BrillPOSTokenizer.obj" \
	"$(INTDIR)\ByteHash.obj" \
	"$(INTDIR)\ChineseCharParser.obj" \
	"$(INTDIR)\ChineseParser.obj" \
	"$(INTDIR)\DocMgrManager.obj" \
	"$(INTDIR)\error.obj" \
	"$(INTDIR)\FlattextDocMgr.obj" \
	"$(INTDIR)\FUtil.obj" \
	"$(INTDIR)\IdentifinderParser.obj" \
	"$(INTDIR)\InqArabicParser.obj" \
	"$(INTDIR)\InQueryOpParser.obj" \
	"$(INTDIR)\Keyfile.obj" \
	"$(INTDIR)\keyfilecode.obj" \
	"$(INTDIR)\KeyfileDocMgr.obj" \
	"$(INTDIR)\keyprint.obj" \
	"$(INTDIR)\kstem.obj" \
	"$(INTDIR)\KStemmer.obj" \
	"$(INTDIR)\lex_parser.obj" \
	"$(INTDIR)\LinkedPropertyList.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\MemCache.obj" \
	"$(INTDIR)\MemList.obj" \
	"$(INTDIR)\Param.obj" \
	"$(INTDIR)\parameters.obj" \
	"$(INTDIR)\Parser.obj" \
	"$(INTDIR)\porter_stemmer.obj" \
	"$(INTDIR)\PorterStemmer.obj" \
	"$(INTDIR)\Property.obj" \
	"$(INTDIR)\QueryDocument.obj" \
	"$(INTDIR)\ReadBuffer.obj" \
	"$(INTDIR)\ReutersParser.obj" \
	"$(INTDIR)\RVLCompress.obj" \
	"$(INTDIR)\Stopper.obj" \
	"$(INTDIR)\string-set.obj" \
	"$(INTDIR)\String.obj" \
	"$(INTDIR)\TermCache.obj" \
	"$(INTDIR)\TextHandlerManager.obj" \
	"$(INTDIR)\Timer.obj" \
	"$(INTDIR)\TrecParser.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\WebParser.obj" \
	"$(INTDIR)\WordSet.obj" \
	"$(INTDIR)\WriteBuffer.obj" \
	"$(INTDIR)\WriterInQueryHandler.obj" \
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
	-@erase "$(INTDIR)\arabic_stem.obj"
	-@erase "$(INTDIR)\ArabicParser.obj"
	-@erase "$(INTDIR)\ArabicStemmer.obj"
	-@erase "$(INTDIR)\BasicDocStream.obj"
	-@erase "$(INTDIR)\BrillPOSParser.obj"
	-@erase "$(INTDIR)\BrillPOSTokenizer.obj"
	-@erase "$(INTDIR)\ByteHash.obj"
	-@erase "$(INTDIR)\ChineseCharParser.obj"
	-@erase "$(INTDIR)\ChineseParser.obj"
	-@erase "$(INTDIR)\DocMgrManager.obj"
	-@erase "$(INTDIR)\error.obj"
	-@erase "$(INTDIR)\FlattextDocMgr.obj"
	-@erase "$(INTDIR)\FUtil.obj"
	-@erase "$(INTDIR)\IdentifinderParser.obj"
	-@erase "$(INTDIR)\InqArabicParser.obj"
	-@erase "$(INTDIR)\InQueryOpParser.obj"
	-@erase "$(INTDIR)\Keyfile.obj"
	-@erase "$(INTDIR)\keyfilecode.obj"
	-@erase "$(INTDIR)\KeyfileDocMgr.obj"
	-@erase "$(INTDIR)\keyprint.obj"
	-@erase "$(INTDIR)\kstem.obj"
	-@erase "$(INTDIR)\KStemmer.obj"
	-@erase "$(INTDIR)\lex_parser.obj"
	-@erase "$(INTDIR)\LinkedPropertyList.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\MemCache.obj"
	-@erase "$(INTDIR)\MemList.obj"
	-@erase "$(INTDIR)\Param.obj"
	-@erase "$(INTDIR)\parameters.obj"
	-@erase "$(INTDIR)\Parser.obj"
	-@erase "$(INTDIR)\porter_stemmer.obj"
	-@erase "$(INTDIR)\PorterStemmer.obj"
	-@erase "$(INTDIR)\Property.obj"
	-@erase "$(INTDIR)\QueryDocument.obj"
	-@erase "$(INTDIR)\ReadBuffer.obj"
	-@erase "$(INTDIR)\ReutersParser.obj"
	-@erase "$(INTDIR)\RVLCompress.obj"
	-@erase "$(INTDIR)\Stopper.obj"
	-@erase "$(INTDIR)\string-set.obj"
	-@erase "$(INTDIR)\String.obj"
	-@erase "$(INTDIR)\TermCache.obj"
	-@erase "$(INTDIR)\TextHandlerManager.obj"
	-@erase "$(INTDIR)\Timer.obj"
	-@erase "$(INTDIR)\TrecParser.obj"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\WebParser.obj"
	-@erase "$(INTDIR)\WordSet.obj"
	-@erase "$(INTDIR)\WriteBuffer.obj"
	-@erase "$(INTDIR)\WriterInQueryHandler.obj"
	-@erase "$(INTDIR)\WriterTextHandler.obj"
	-@erase ".\lemur_utility.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GR /GX /ZI /Od /I "utility\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\lemur_utility.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\lemur_utility.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"lemur_utility.lib" 
LIB32_OBJS= \
	"$(INTDIR)\arabic_stem.obj" \
	"$(INTDIR)\ArabicParser.obj" \
	"$(INTDIR)\ArabicStemmer.obj" \
	"$(INTDIR)\BasicDocStream.obj" \
	"$(INTDIR)\BrillPOSParser.obj" \
	"$(INTDIR)\BrillPOSTokenizer.obj" \
	"$(INTDIR)\ByteHash.obj" \
	"$(INTDIR)\ChineseCharParser.obj" \
	"$(INTDIR)\ChineseParser.obj" \
	"$(INTDIR)\DocMgrManager.obj" \
	"$(INTDIR)\error.obj" \
	"$(INTDIR)\FlattextDocMgr.obj" \
	"$(INTDIR)\FUtil.obj" \
	"$(INTDIR)\IdentifinderParser.obj" \
	"$(INTDIR)\InqArabicParser.obj" \
	"$(INTDIR)\InQueryOpParser.obj" \
	"$(INTDIR)\Keyfile.obj" \
	"$(INTDIR)\keyfilecode.obj" \
	"$(INTDIR)\KeyfileDocMgr.obj" \
	"$(INTDIR)\keyprint.obj" \
	"$(INTDIR)\kstem.obj" \
	"$(INTDIR)\KStemmer.obj" \
	"$(INTDIR)\lex_parser.obj" \
	"$(INTDIR)\LinkedPropertyList.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\MemCache.obj" \
	"$(INTDIR)\MemList.obj" \
	"$(INTDIR)\Param.obj" \
	"$(INTDIR)\parameters.obj" \
	"$(INTDIR)\Parser.obj" \
	"$(INTDIR)\porter_stemmer.obj" \
	"$(INTDIR)\PorterStemmer.obj" \
	"$(INTDIR)\Property.obj" \
	"$(INTDIR)\QueryDocument.obj" \
	"$(INTDIR)\ReadBuffer.obj" \
	"$(INTDIR)\ReutersParser.obj" \
	"$(INTDIR)\RVLCompress.obj" \
	"$(INTDIR)\Stopper.obj" \
	"$(INTDIR)\string-set.obj" \
	"$(INTDIR)\String.obj" \
	"$(INTDIR)\TermCache.obj" \
	"$(INTDIR)\TextHandlerManager.obj" \
	"$(INTDIR)\Timer.obj" \
	"$(INTDIR)\TrecParser.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\WebParser.obj" \
	"$(INTDIR)\WordSet.obj" \
	"$(INTDIR)\WriteBuffer.obj" \
	"$(INTDIR)\WriterInQueryHandler.obj" \
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
SOURCE=.\utility\src\arabic_stem.cpp

"$(INTDIR)\arabic_stem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\ArabicParser.cpp

"$(INTDIR)\ArabicParser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\ArabicStemmer.cpp

"$(INTDIR)\ArabicStemmer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\BasicDocStream.cpp

"$(INTDIR)\BasicDocStream.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\BrillPOSParser.cpp

"$(INTDIR)\BrillPOSParser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\BrillPOSTokenizer.cpp

"$(INTDIR)\BrillPOSTokenizer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\ByteHash.cpp

"$(INTDIR)\ByteHash.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\ChineseCharParser.cpp

"$(INTDIR)\ChineseCharParser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\ChineseParser.cpp

"$(INTDIR)\ChineseParser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\DocMgrManager.cpp

"$(INTDIR)\DocMgrManager.obj" : $(SOURCE) "$(INTDIR)"
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


SOURCE=.\utility\src\IdentifinderParser.cpp

"$(INTDIR)\IdentifinderParser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\InqArabicParser.cpp

"$(INTDIR)\InqArabicParser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\InQueryOpParser.cpp

"$(INTDIR)\InQueryOpParser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\Keyfile.cpp

"$(INTDIR)\Keyfile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\keyfilecode.c

"$(INTDIR)\keyfilecode.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\KeyfileDocMgr.cpp

"$(INTDIR)\KeyfileDocMgr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\keyprint.c

"$(INTDIR)\keyprint.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\kstem.cpp

"$(INTDIR)\kstem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\KStemmer.cpp

"$(INTDIR)\KStemmer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\lex_parser.cpp

"$(INTDIR)\lex_parser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\LinkedPropertyList.cpp

"$(INTDIR)\LinkedPropertyList.obj" : $(SOURCE) "$(INTDIR)"
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


SOURCE=.\utility\src\Parser.cpp

"$(INTDIR)\Parser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\porter_stemmer.cpp

"$(INTDIR)\porter_stemmer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\PorterStemmer.cpp

"$(INTDIR)\PorterStemmer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\Property.cpp

"$(INTDIR)\Property.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\QueryDocument.cpp

"$(INTDIR)\QueryDocument.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\ReadBuffer.cpp

"$(INTDIR)\ReadBuffer.obj" : $(SOURCE) "$(INTDIR)"
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


SOURCE=.\utility\src\TermCache.cpp

"$(INTDIR)\TermCache.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\TextHandlerManager.cpp

"$(INTDIR)\TextHandlerManager.obj" : $(SOURCE) "$(INTDIR)"
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


SOURCE=.\utility\src\WriteBuffer.cpp

"$(INTDIR)\WriteBuffer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\WriterInQueryHandler.cpp

"$(INTDIR)\WriterInQueryHandler.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility\src\WriterTextHandler.cpp

"$(INTDIR)\WriterTextHandler.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

