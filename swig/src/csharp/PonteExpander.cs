/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.29
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

namespace Indri {

using System;
using System.Runtime.InteropServices;

public class PonteExpander : QueryExpander {
  private HandleRef swigCPtr;

  internal PonteExpander(IntPtr cPtr, bool cMemoryOwn) : base(indri_csharpPINVOKE.PonteExpanderUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(PonteExpander obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~PonteExpander() {
    Dispose();
  }

  public override void Dispose() {
    if(swigCPtr.Handle != IntPtr.Zero && swigCMemOwn) {
      swigCMemOwn = false;
      indri_csharpPINVOKE.delete_PonteExpander(swigCPtr);
    }
    swigCPtr = new HandleRef(null, IntPtr.Zero);
    GC.SuppressFinalize(this);
    base.Dispose();
  }

  public PonteExpander(QueryEnvironment env, Parameters param) : this(indri_csharpPINVOKE.new_PonteExpander(QueryEnvironment.getCPtr(env), Parameters.getCPtr(param)), true) {
    if (indri_csharpPINVOKE.SWIGPendingException.Pending) throw indri_csharpPINVOKE.SWIGPendingException.Retrieve();
  }

  public override string expand(string originalQuery, ScoredExtentResultVector results) {
    string ret = indri_csharpPINVOKE.PonteExpander_expand(swigCPtr, originalQuery, ScoredExtentResultVector.getCPtr(results));
    if (indri_csharpPINVOKE.SWIGPendingException.Pending) throw indri_csharpPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

}

}
