# Microsoft Developer Studio Generated NMAKE File, Based on lemur_cluster.dsp
!IF "$(CFG)" == ""
CFG=lemur_cluster - Win32 Release
!MESSAGE No configuration specified. Defaulting to lemur_cluster - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "lemur_cluster - Win32 Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lemur_cluster.mak" CFG="lemur_cluster - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lemur_cluster - Win32 Release" (based on "Win32 (x86) Static Library")
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
OUTDIR=.\cluster\obj
INTDIR=.\cluster\obj

ALL : ".\lemur_cluster.lib"


CLEAN :
	-@erase "$(INTDIR)\AgglomCluster.obj"
	-@erase "$(INTDIR)\CentroidCluster.obj"
	-@erase "$(INTDIR)\Cluster.obj"
	-@erase "$(INTDIR)\ClusterDB.obj"
	-@erase "$(INTDIR)\ClusterFactory.obj"
	-@erase "$(INTDIR)\ClusterRep.obj"
	-@erase "$(INTDIR)\CosSim.obj"
	-@erase "$(INTDIR)\FlatFileClusterDB.obj"
	-@erase "$(INTDIR)\FloatFreqVector.obj"
	-@erase "$(INTDIR)\KeyfileClusterDB.obj"
	-@erase "$(INTDIR)\OfflineCluster.obj"
	-@erase "$(INTDIR)\PLSA.obj"
	-@erase "$(INTDIR)\ThresholdFcn.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase ".\lemur_cluster.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\lemur_cluster.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"lemur_cluster.lib" 
LIB32_OBJS= \
	"$(INTDIR)\AgglomCluster.obj" \
	"$(INTDIR)\CentroidCluster.obj" \
	"$(INTDIR)\Cluster.obj" \
	"$(INTDIR)\ClusterDB.obj" \
	"$(INTDIR)\ClusterFactory.obj" \
	"$(INTDIR)\ClusterRep.obj" \
	"$(INTDIR)\CosSim.obj" \
	"$(INTDIR)\FlatFileClusterDB.obj" \
	"$(INTDIR)\FloatFreqVector.obj" \
	"$(INTDIR)\KeyfileClusterDB.obj" \
	"$(INTDIR)\OfflineCluster.obj" \
	"$(INTDIR)\PLSA.obj" \
	"$(INTDIR)\ThresholdFcn.obj"

".\lemur_cluster.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

CPP_PROJ=/nologo /ML /W3 /GR /GX /O2 /I "utility\include" /I "index\include" /I "langmod\include" /I "retrieval\include" /I "cluster\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\lemur_cluster.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
!IF EXISTS("lemur_cluster.dep")
!INCLUDE "lemur_cluster.dep"
!ELSE 
!MESSAGE Warning: cannot find "lemur_cluster.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "lemur_cluster - Win32 Release"
SOURCE=.\cluster\src\AgglomCluster.cpp

"$(INTDIR)\AgglomCluster.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\cluster\src\CentroidCluster.cpp

"$(INTDIR)\CentroidCluster.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\cluster\src\Cluster.cpp

"$(INTDIR)\Cluster.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\cluster\src\ClusterDB.cpp

"$(INTDIR)\ClusterDB.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\cluster\src\ClusterFactory.cpp

"$(INTDIR)\ClusterFactory.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\cluster\src\ClusterRep.cpp

"$(INTDIR)\ClusterRep.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\cluster\src\CosSim.cpp

"$(INTDIR)\CosSim.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\cluster\src\FlatFileClusterDB.cpp

"$(INTDIR)\FlatFileClusterDB.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\cluster\src\FloatFreqVector.cpp

"$(INTDIR)\FloatFreqVector.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\cluster\src\KeyfileClusterDB.cpp

"$(INTDIR)\KeyfileClusterDB.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\cluster\src\OfflineCluster.cpp

"$(INTDIR)\OfflineCluster.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\cluster\src\PLSA.cpp

"$(INTDIR)\PLSA.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\cluster\src\ThresholdFcn.cpp

"$(INTDIR)\ThresholdFcn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

