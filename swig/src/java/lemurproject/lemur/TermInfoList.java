/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.31
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package lemurproject.lemur;

public class TermInfoList {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected TermInfoList(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(TermInfoList obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if(swigCPtr != 0 && swigCMemOwn) {
      swigCMemOwn = false;
      lemurJNI.delete_TermInfoList(swigCPtr);
    }
    swigCPtr = 0;
  }

  
/**
prepare iteration
*/
public void startIteration() {
    lemurJNI.TermInfoList_startIteration(swigCPtr, this);
  }

  
/**
test if there's any entry
*/
public boolean hasMore() {
    return lemurJNI.TermInfoList_hasMore(swigCPtr, this);
  }

  
/**
@return the next entry
*/
public TermInfo nextEntry() {
    long cPtr = lemurJNI.TermInfoList_nextEntry(swigCPtr, this);
    return (cPtr == 0) ? null : new TermInfo(cPtr, false);
  }

}
