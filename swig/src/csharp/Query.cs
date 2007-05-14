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

public class Query : IDisposable {
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal Query(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(Query obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  public virtual void Dispose() {
    lock(this) {
      if(swigCPtr.Handle != IntPtr.Zero && swigCMemOwn) {
        swigCMemOwn = false;
        throw new MethodAccessException("C++ destructor does not have public access");
      }
      swigCPtr = new HandleRef(null, IntPtr.Zero);
      GC.SuppressFinalize(this);
    }
  }

  public static Query makeQuery(string query, string stopfile, string stemtype, string datadir, string func) {
    IntPtr cPtr = lemur_csharpPINVOKE.Query_makeQuery__SWIG_0(query, stopfile, stemtype, datadir, func);
    Query ret = (cPtr == IntPtr.Zero) ? null : new Query(cPtr, true);
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static Query makeQuery(string query, string stopfile, string stemtype, string datadir) {
    IntPtr cPtr = lemur_csharpPINVOKE.Query_makeQuery__SWIG_1(query, stopfile, stemtype, datadir);
    Query ret = (cPtr == IntPtr.Zero) ? null : new Query(cPtr, true);
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static Query makeQuery(string query, string stopfile, string stemtype) {
    IntPtr cPtr = lemur_csharpPINVOKE.Query_makeQuery__SWIG_2(query, stopfile, stemtype);
    Query ret = (cPtr == IntPtr.Zero) ? null : new Query(cPtr, true);
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static Query makeQuery(string query, string stopfile) {
    IntPtr cPtr = lemur_csharpPINVOKE.Query_makeQuery__SWIG_3(query, stopfile);
    Query ret = (cPtr == IntPtr.Zero) ? null : new Query(cPtr, true);
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static Query makeQuery(string query) {
    IntPtr cPtr = lemur_csharpPINVOKE.Query_makeQuery__SWIG_4(query);
    Query ret = (cPtr == IntPtr.Zero) ? null : new Query(cPtr, true);
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

}

}
