/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.29
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package lemurproject.lemur;

public class DocInfo {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected DocInfo(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(DocInfo obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public void delete() {
    if(swigCPtr != 0 && swigCMemOwn) {
      swigCMemOwn = false;
      lemurJNI.delete_DocInfo(swigCPtr);
    }
    swigCPtr = 0;
  }

  public DocInfo(int termID, int termCount) {
    this(lemurJNI.new_DocInfo(termID, termCount), true);
  }

  public int docID() {
    return lemurJNI.DocInfo_docID(swigCPtr);
  }

  public int termCount() {
    return lemurJNI.DocInfo_termCount(swigCPtr);
  }

  public int[] positions() {
  return lemurJNI.DocInfo_positions(swigCPtr);
}

}