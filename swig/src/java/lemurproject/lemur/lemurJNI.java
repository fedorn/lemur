/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.36
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package lemurproject.lemur;

class lemurJNI {

  static {
    System.loadLibrary("lemur_jni");
  }
  
  public final static native long new_TermInfo(int jarg1, int jarg2);
  public final static native int TermInfo_termID(long jarg1, TermInfo jarg1_);
  public final static native int TermInfo_count(long jarg1, TermInfo jarg1_);
  public final static native int[] TermInfo_positions(long jarg1, TermInfo jarg1_);
  public final static native int TermInfo_position(long jarg1, TermInfo jarg1_);
  public final static native void delete_TermInfo(long jarg1);
  public final static native void TermInfoList_startIteration(long jarg1, TermInfoList jarg1_);
  public final static native boolean TermInfoList_hasMore(long jarg1, TermInfoList jarg1_);
  public final static native long TermInfoList_nextEntry(long jarg1, TermInfoList jarg1_);
  public final static native void delete_TermInfoList(long jarg1);
  public final static native long new_DocInfo(int jarg1, int jarg2);
  public final static native int DocInfo_docID(long jarg1, DocInfo jarg1_);
  public final static native int DocInfo_termCount(long jarg1, DocInfo jarg1_);
  public final static native int[] DocInfo_positions(long jarg1, DocInfo jarg1_);
  public final static native void delete_DocInfo(long jarg1);
  public final static native void DocInfoList_startIteration(long jarg1, DocInfoList jarg1_);
  public final static native boolean DocInfoList_hasMore(long jarg1, DocInfoList jarg1_);
  public final static native long DocInfoList_nextEntry(long jarg1, DocInfoList jarg1_);
  public final static native void delete_DocInfoList(long jarg1);
  public final static native String DocumentManager_getDoc(long jarg1, DocumentManager jarg1_, String jarg2);
  public final static native String DocumentManager_docElement(long jarg1, DocumentManager jarg1_, String jarg2, String jarg3);
  public final static native void delete_DocumentManager(long jarg1);
  public final static native boolean Index_open(long jarg1, Index jarg1_, String jarg2) throws java.lang.Exception;
  public final static native int Index_term__SWIG_0(long jarg1, Index jarg1_, String jarg2) throws java.lang.Exception;
  public final static native String Index_term__SWIG_1(long jarg1, Index jarg1_, int jarg2) throws java.lang.Exception;
  public final static native int Index_document__SWIG_0(long jarg1, Index jarg1_, String jarg2) throws java.lang.Exception;
  public final static native String Index_document__SWIG_1(long jarg1, Index jarg1_, int jarg2) throws java.lang.Exception;
  public final static native long Index_docManager(long jarg1, Index jarg1_, int jarg2) throws java.lang.Exception;
  public final static native int Index_docCount__SWIG_0(long jarg1, Index jarg1_) throws java.lang.Exception;
  public final static native int Index_termCountUnique(long jarg1, Index jarg1_) throws java.lang.Exception;
  public final static native int Index_termCount__SWIG_0(long jarg1, Index jarg1_, int jarg2) throws java.lang.Exception;
  public final static native int Index_termCount__SWIG_1(long jarg1, Index jarg1_) throws java.lang.Exception;
  public final static native float Index_docLengthAvg(long jarg1, Index jarg1_) throws java.lang.Exception;
  public final static native int Index_docCount__SWIG_1(long jarg1, Index jarg1_, int jarg2) throws java.lang.Exception;
  public final static native int Index_docLength(long jarg1, Index jarg1_, int jarg2) throws java.lang.Exception;
  public final static native long Index_docInfoList(long jarg1, Index jarg1_, int jarg2) throws java.lang.Exception;
  public final static native long Index_termInfoList(long jarg1, Index jarg1_, int jarg2) throws java.lang.Exception;
  public final static native long Index_termInfoListSeq(long jarg1, Index jarg1_, int jarg2) throws java.lang.Exception;
  public final static native void Index_setProps(long jarg1, Index jarg1_);
  public final static native long IndexManager_openIndex(String jarg1) throws java.lang.Exception;
  public final static native long Query_makeQuery__SWIG_0(String jarg1, String jarg2, String jarg3, String jarg4, String jarg5) throws java.lang.Exception;
  public final static native long Query_makeQuery__SWIG_1(String jarg1, String jarg2, String jarg3, String jarg4) throws java.lang.Exception;
  public final static native long Query_makeQuery__SWIG_2(String jarg1, String jarg2, String jarg3) throws java.lang.Exception;
  public final static native long Query_makeQuery__SWIG_3(String jarg1, String jarg2) throws java.lang.Exception;
  public final static native long Query_makeQuery__SWIG_4(String jarg1) throws java.lang.Exception;
  public final static native void RetrievalMethod_scoreCollection(long jarg1, RetrievalMethod jarg1_, long jarg2, QueryRep jarg2_, IndexedReal[] jarg3) throws java.lang.Exception;
  public final static native void RetrievalMethod_updateQuery(long jarg1, RetrievalMethod jarg1_, long jarg2, QueryRep jarg2_, long jarg3, WeightedIDSet jarg3_) throws java.lang.Exception;
  public final static native void delete_RetrievalMethod(long jarg1);
  public final static native void delete_TextQueryRetMethod(long jarg1);
  public final static native void delete_StructQueryRetMethod(long jarg1);
  public final static native void delete_IndriRetMethod(long jarg1);
  public final static native RetrievalMethod RetMethodManager_createModel__SWIG_0(long jarg1, Index jarg1_, long jarg2, ScoreAccumulator jarg2_, String jarg3) throws java.lang.Exception;
  public final static native RetrievalMethod RetMethodManager_createModel__SWIG_1(long jarg1, Index jarg1_, long jarg2, ScoreAccumulator jarg2_) throws java.lang.Exception;
  public final static native IndexedReal[] RetMethodManager_runTextQuery__SWIG_0(String jarg1, long jarg2, TextQueryRetMethod jarg2_, String jarg3, String jarg4, String jarg5, String jarg6) throws java.lang.Exception;
  public final static native IndexedReal[] RetMethodManager_runTextQuery__SWIG_1(String jarg1, long jarg2, TextQueryRetMethod jarg2_, String jarg3, String jarg4, String jarg5) throws java.lang.Exception;
  public final static native IndexedReal[] RetMethodManager_runTextQuery__SWIG_2(String jarg1, long jarg2, TextQueryRetMethod jarg2_, String jarg3, String jarg4) throws java.lang.Exception;
  public final static native IndexedReal[] RetMethodManager_runTextQuery__SWIG_3(String jarg1, long jarg2, TextQueryRetMethod jarg2_, String jarg3) throws java.lang.Exception;
  public final static native IndexedReal[] RetMethodManager_runTextQuery__SWIG_4(String jarg1, long jarg2, TextQueryRetMethod jarg2_) throws java.lang.Exception;
  public final static native IndexedReal[] RetMethodManager_runStructQuery__SWIG_0(String jarg1, long jarg2, StructQueryRetMethod jarg2_, String jarg3, String jarg4, String jarg5, String jarg6) throws java.lang.Exception;
  public final static native IndexedReal[] RetMethodManager_runStructQuery__SWIG_1(String jarg1, long jarg2, StructQueryRetMethod jarg2_, String jarg3, String jarg4, String jarg5) throws java.lang.Exception;
  public final static native IndexedReal[] RetMethodManager_runStructQuery__SWIG_2(String jarg1, long jarg2, StructQueryRetMethod jarg2_, String jarg3, String jarg4) throws java.lang.Exception;
  public final static native IndexedReal[] RetMethodManager_runStructQuery__SWIG_3(String jarg1, long jarg2, StructQueryRetMethod jarg2_, String jarg3) throws java.lang.Exception;
  public final static native IndexedReal[] RetMethodManager_runStructQuery__SWIG_4(String jarg1, long jarg2, StructQueryRetMethod jarg2_) throws java.lang.Exception;
  public final static native IndexedReal[] RetMethodManager_runIndriQuery(String jarg1, long jarg2, IndriRetMethod jarg2_) throws java.lang.Exception;
  public final static native long new_ArrayAccumulator(int jarg1);
  public final static native void delete_ArrayAccumulator(long jarg1);
  public final static native TMatch[] MatchInfo_getMatches(long jarg1, Index jarg1_, long jarg2, Query jarg2_, int jarg3) throws java.lang.Exception;
  public final static native void ParamSet(String jarg1, String jarg2) throws java.lang.Exception;
  public final static native void ParamClear() throws java.lang.Exception;
  public final static native int ParamPushFile(String jarg1) throws java.lang.Exception;
  public final static native String ParamPopFile() throws java.lang.Exception;
  public final static native long SWIGTextQueryRetMethodUpcast(long jarg1);
  public final static native long SWIGStructQueryRetMethodUpcast(long jarg1);
  public final static native long SWIGIndriRetMethodUpcast(long jarg1);
  public final static native long SWIGArrayAccumulatorUpcast(long jarg1);
}
