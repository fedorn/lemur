/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.29
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package lemurproject.indri;
import java.util.Map;
class indriJNI {
  public final static native QueryAnnotationNode QueryAnnotation_getQueryTree(long jarg1) throws java.lang.Exception;
  public final static native java.util.Map QueryAnnotation_getAnnotations(long jarg1) throws java.lang.Exception;
  public final static native ScoredExtentResult[] QueryAnnotation_getResults(long jarg1) throws java.lang.Exception;
  public final static native long new_QueryAnnotation();
  public final static native void delete_QueryAnnotation(long jarg1);
  public final static native long new_QueryEnvironment();
  public final static native void QueryEnvironment_addServer(long jarg1, String jarg2) throws java.lang.Exception;
  public final static native void QueryEnvironment_addIndex(long jarg1, String jarg2) throws java.lang.Exception;
  public final static native void QueryEnvironment_removeServer(long jarg1, String jarg2) throws java.lang.Exception;
  public final static native void QueryEnvironment_removeIndex(long jarg1, String jarg2) throws java.lang.Exception;
  public final static native void QueryEnvironment_close(long jarg1) throws java.lang.Exception;
  public final static native void QueryEnvironment_setMemory(long jarg1, long jarg2) throws java.lang.Exception;
  public final static native void QueryEnvironment_setScoringRules(long jarg1, String[] jarg2) throws java.lang.Exception;
  public final static native void QueryEnvironment_setStopwords(long jarg1, String[] jarg2) throws java.lang.Exception;
  public final static native ScoredExtentResult[] QueryEnvironment_runQuery__SWIG_0(long jarg1, String jarg2, int jarg3) throws java.lang.Exception;
  public final static native ScoredExtentResult[] QueryEnvironment_runQuery__SWIG_1(long jarg1, String jarg2, int[] jarg3, int jarg4) throws java.lang.Exception;
  public final static native long QueryEnvironment_runAnnotatedQuery__SWIG_0(long jarg1, String jarg2, int jarg3) throws java.lang.Exception;
  public final static native long QueryEnvironment_runAnnotatedQuery__SWIG_1(long jarg1, String jarg2, int[] jarg3, int jarg4) throws java.lang.Exception;
  public final static native ParsedDocument[] QueryEnvironment_documents__SWIG_0(long jarg1, int[] jarg2) throws java.lang.Exception;
  public final static native ParsedDocument[] QueryEnvironment_documents__SWIG_1(long jarg1, ScoredExtentResult[] jarg2) throws java.lang.Exception;
  public final static native String[] QueryEnvironment_documentMetadata__SWIG_0(long jarg1, int[] jarg2, String jarg3) throws java.lang.Exception;
  public final static native String[] QueryEnvironment_documentMetadata__SWIG_1(long jarg1, ScoredExtentResult[] jarg2, String jarg3) throws java.lang.Exception;
  public final static native int[] QueryEnvironment_documentIDsFromMetadata(long jarg1, String jarg2, String[] jarg3) throws java.lang.Exception;
  public final static native ParsedDocument[] QueryEnvironment_documentsFromMetadata(long jarg1, String jarg2, String[] jarg3) throws java.lang.Exception;
  public final static native long QueryEnvironment_termCount__SWIG_0(long jarg1) throws java.lang.Exception;
  public final static native long QueryEnvironment_termCount__SWIG_1(long jarg1, String jarg2) throws java.lang.Exception;
  public final static native long QueryEnvironment_termFieldCount(long jarg1, String jarg2, String jarg3) throws java.lang.Exception;
  public final static native String[] QueryEnvironment_fieldList(long jarg1) throws java.lang.Exception;
  public final static native long QueryEnvironment_documentCount__SWIG_0(long jarg1) throws java.lang.Exception;
  public final static native long QueryEnvironment_documentCount__SWIG_1(long jarg1, String jarg2) throws java.lang.Exception;
  public final static native DocumentVector[] QueryEnvironment_documentVectors(long jarg1, int[] jarg2) throws java.lang.Exception;
  public final static native double QueryEnvironment_expressionCount__SWIG_0(long jarg1, String jarg2, String jarg3);
  public final static native double QueryEnvironment_expressionCount__SWIG_1(long jarg1, String jarg2);
  public final static native ScoredExtentResult[] QueryEnvironment_expressionList__SWIG_0(long jarg1, String jarg2, String jarg3);
  public final static native ScoredExtentResult[] QueryEnvironment_expressionList__SWIG_1(long jarg1, String jarg2);
  public final static native void delete_QueryEnvironment(long jarg1);
  public final static native void delete_IndexStatus(long jarg1);
  public final static native void IndexStatus_status(long jarg1, int jarg2, String jarg3, String jarg4, int jarg5, int jarg6);
  public final static native long new_IndexStatus();
  public final static native void IndexStatus_director_connect(IndexStatus obj, long cptr, boolean mem_own, boolean weak_global);
  public final static native void IndexStatus_change_ownership(IndexStatus obj, long cptr, boolean take_or_release);
  public final static native long new_IndexEnvironment();
  public final static native void delete_IndexEnvironment(long jarg1);
  public final static native void IndexEnvironment_setDocumentRoot(long jarg1, String jarg2) throws java.lang.Exception;
  public final static native void IndexEnvironment_setAnchorTextPath(long jarg1, String jarg2) throws java.lang.Exception;
  public final static native void IndexEnvironment_setOffsetMetadataPath(long jarg1, String jarg2) throws java.lang.Exception;
  public final static native void IndexEnvironment_setOffsetAnnotationsPath(long jarg1, String jarg2) throws java.lang.Exception;
  public final static native void IndexEnvironment_addFileClass__SWIG_0(long jarg1, String jarg2, String jarg3, String jarg4, String jarg5, String jarg6, String jarg7, String jarg8, String[] jarg9, String[] jarg10, String[] jarg11, String[] jarg12, Map jarg13) throws java.lang.Exception;
  public final static native Specification IndexEnvironment_getFileClassSpec(long jarg1, String jarg2) throws java.lang.Exception;
  public final static native void IndexEnvironment_addFileClass__SWIG_1(long jarg1, Specification jarg2) throws java.lang.Exception;
  public final static native void IndexEnvironment_deleteDocument(long jarg1, int jarg2) throws java.lang.Exception;
  public final static native void IndexEnvironment_setIndexedFields(long jarg1, String[] jarg2) throws java.lang.Exception;
  public final static native void IndexEnvironment_setNumericField__SWIG_0(long jarg1, String jarg2, boolean jarg3, String jarg4) throws java.lang.Exception;
  public final static native void IndexEnvironment_setNumericField__SWIG_1(long jarg1, String jarg2, boolean jarg3) throws java.lang.Exception;
  public final static native void IndexEnvironment_setMetadataIndexedFields(long jarg1, String[] jarg2, String[] jarg3) throws java.lang.Exception;
  public final static native void IndexEnvironment_setStopwords(long jarg1, String[] jarg2) throws java.lang.Exception;
  public final static native void IndexEnvironment_setStemmer(long jarg1, String jarg2) throws java.lang.Exception;
  public final static native void IndexEnvironment_setMemory(long jarg1, long jarg2) throws java.lang.Exception;
  public final static native void IndexEnvironment_setNormalization(long jarg1, boolean jarg2) throws java.lang.Exception;
  public final static native void IndexEnvironment_create__SWIG_0(long jarg1, String jarg2, long jarg3) throws java.lang.Exception;
  public final static native void IndexEnvironment_create__SWIG_1(long jarg1, String jarg2) throws java.lang.Exception;
  public final static native void IndexEnvironment_open__SWIG_0(long jarg1, String jarg2, long jarg3) throws java.lang.Exception;
  public final static native void IndexEnvironment_open__SWIG_1(long jarg1, String jarg2) throws java.lang.Exception;
  public final static native void IndexEnvironment_close(long jarg1) throws java.lang.Exception;
  public final static native void IndexEnvironment_addFile__SWIG_0(long jarg1, String jarg2) throws java.lang.Exception;
  public final static native void IndexEnvironment_addFile__SWIG_1(long jarg1, String jarg2, String jarg3) throws java.lang.Exception;
  public final static native int IndexEnvironment_addString(long jarg1, String jarg2, String jarg3, Map jarg4) throws java.lang.Exception;
  public final static native int IndexEnvironment_addParsedDocument(long jarg1, ParsedDocument jarg2) throws java.lang.Exception;
  public final static native int IndexEnvironment_documentsIndexed(long jarg1) throws java.lang.Exception;
  public final static native int IndexEnvironment_documentsSeen(long jarg1) throws java.lang.Exception;

  static {
    System.loadLibrary("lemur_jni");
  }
  

  /* Director upcall methods: */

public static void SwigDirector_IndexStatus_status(IndexStatus self, int code, String documentPath, String error, int documentsIndexed, int documentsSeen) {
  self.status(code, documentPath, error, documentsIndexed, documentsSeen); 
}


  private final static native void swig_module_init();
  static {
    swig_module_init();
  }
}
