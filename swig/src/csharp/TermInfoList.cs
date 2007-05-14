/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.31
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

namespace Lemur {

using System;
using System.Runtime.InteropServices;

public class TermInfoList : IDisposable {
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal TermInfoList(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(TermInfoList obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~TermInfoList() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if(swigCPtr.Handle != IntPtr.Zero && swigCMemOwn) {
        swigCMemOwn = false;
        lemur_csharpPINVOKE.delete_TermInfoList(swigCPtr);
      }
      swigCPtr = new HandleRef(null, IntPtr.Zero);
      GC.SuppressFinalize(this);
    }
  }

  public void startIteration() {
    lemur_csharpPINVOKE.TermInfoList_startIteration(swigCPtr);
  }

  public bool hasMore() {
    bool ret = lemur_csharpPINVOKE.TermInfoList_hasMore(swigCPtr);
    return ret;
  }

  public TermInfo nextEntry() {
    IntPtr cPtr = lemur_csharpPINVOKE.TermInfoList_nextEntry(swigCPtr);
    TermInfo ret = (cPtr == IntPtr.Zero) ? null : new TermInfo(cPtr, false);
    return ret;
  }

}

}
