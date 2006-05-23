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

public class QueryEnvironment : IDisposable {
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal QueryEnvironment(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(QueryEnvironment obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~QueryEnvironment() {
    Dispose();
  }

  public virtual void Dispose() {
    if(swigCPtr.Handle != IntPtr.Zero && swigCMemOwn) {
      swigCMemOwn = false;
      indriPINVOKE.delete_QueryEnvironment(swigCPtr);
    }
    swigCPtr = new HandleRef(null, IntPtr.Zero);
    GC.SuppressFinalize(this);
  }

  public QueryEnvironment() : this(indriPINVOKE.new_QueryEnvironment(), true) {
  }

  public void addServer(string hostname) {
    indriPINVOKE.QueryEnvironment_addServer(swigCPtr, hostname);
    if (indriPINVOKE.SWIGPendingException.Pending) throw indriPINVOKE.SWIGPendingException.Retrieve();
  }

  public void addIndex(string pathname) {
    indriPINVOKE.QueryEnvironment_addIndex(swigCPtr, pathname);
    if (indriPINVOKE.SWIGPendingException.Pending) throw indriPINVOKE.SWIGPendingException.Retrieve();
  }

  public void removeServer(string hostname) {
    indriPINVOKE.QueryEnvironment_removeServer(swigCPtr, hostname);
    if (indriPINVOKE.SWIGPendingException.Pending) throw indriPINVOKE.SWIGPendingException.Retrieve();
  }

  public void removeIndex(string pathname) {
    indriPINVOKE.QueryEnvironment_removeIndex(swigCPtr, pathname);
    if (indriPINVOKE.SWIGPendingException.Pending) throw indriPINVOKE.SWIGPendingException.Retrieve();
  }

  public void close() {
    indriPINVOKE.QueryEnvironment_close(swigCPtr);
    if (indriPINVOKE.SWIGPendingException.Pending) throw indriPINVOKE.SWIGPendingException.Retrieve();
  }

  public void setMemory(long memory) {
    indriPINVOKE.QueryEnvironment_setMemory(swigCPtr, memory);
    if (indriPINVOKE.SWIGPendingException.Pending) throw indriPINVOKE.SWIGPendingException.Retrieve();
  }

  public void setScoringRules(StringVector rules) {
    indriPINVOKE.QueryEnvironment_setScoringRules(swigCPtr, StringVector.getCPtr(rules));
    if (indriPINVOKE.SWIGPendingException.Pending) throw indriPINVOKE.SWIGPendingException.Retrieve();
  }

  public void setStopwords(StringVector stopwords) {
    indriPINVOKE.QueryEnvironment_setStopwords(swigCPtr, StringVector.getCPtr(stopwords));
    if (indriPINVOKE.SWIGPendingException.Pending) throw indriPINVOKE.SWIGPendingException.Retrieve();
  }

  public ScoredExtentResultVector runQuery(string query, int resultsRequested) {
    ScoredExtentResultVector ret = new ScoredExtentResultVector(indriPINVOKE.QueryEnvironment_runQuery__SWIG_0(swigCPtr, query, resultsRequested), true);
    if (indriPINVOKE.SWIGPendingException.Pending) throw indriPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public ScoredExtentResultVector runQuery(string query, IntVector documentSet, int resultsRequested) {
    ScoredExtentResultVector ret = new ScoredExtentResultVector(indriPINVOKE.QueryEnvironment_runQuery__SWIG_1(swigCPtr, query, IntVector.getCPtr(documentSet), resultsRequested), true);
    if (indriPINVOKE.SWIGPendingException.Pending) throw indriPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public QueryAnnotation runAnnotatedQuery(string query, int resultsRequested) {
    IntPtr cPtr = indriPINVOKE.QueryEnvironment_runAnnotatedQuery__SWIG_0(swigCPtr, query, resultsRequested);
    QueryAnnotation ret = (cPtr == IntPtr.Zero) ? null : new QueryAnnotation(cPtr, true);
    if (indriPINVOKE.SWIGPendingException.Pending) throw indriPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public QueryAnnotation runAnnotatedQuery(string query, IntVector documentSet, int resultsRequested) {
    IntPtr cPtr = indriPINVOKE.QueryEnvironment_runAnnotatedQuery__SWIG_1(swigCPtr, query, IntVector.getCPtr(documentSet), resultsRequested);
    QueryAnnotation ret = (cPtr == IntPtr.Zero) ? null : new QueryAnnotation(cPtr, true);
    if (indriPINVOKE.SWIGPendingException.Pending) throw indriPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public ParsedDocumentVector documents(IntVector documentIDs) {
    ParsedDocumentVector ret = new ParsedDocumentVector(indriPINVOKE.QueryEnvironment_documents__SWIG_0(swigCPtr, IntVector.getCPtr(documentIDs)), true);
    if (indriPINVOKE.SWIGPendingException.Pending) throw indriPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public ParsedDocumentVector documents(ScoredExtentResultVector results) {
    ParsedDocumentVector ret = new ParsedDocumentVector(indriPINVOKE.QueryEnvironment_documents__SWIG_1(swigCPtr, ScoredExtentResultVector.getCPtr(results)), true);
    if (indriPINVOKE.SWIGPendingException.Pending) throw indriPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public StringVector documentMetadata(IntVector documentIDs, string attributeName) {
    StringVector ret = new StringVector(indriPINVOKE.QueryEnvironment_documentMetadata__SWIG_0(swigCPtr, IntVector.getCPtr(documentIDs), attributeName), true);
    if (indriPINVOKE.SWIGPendingException.Pending) throw indriPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public StringVector documentMetadata(ScoredExtentResultVector documentIDs, string attributeName) {
    StringVector ret = new StringVector(indriPINVOKE.QueryEnvironment_documentMetadata__SWIG_1(swigCPtr, ScoredExtentResultVector.getCPtr(documentIDs), attributeName), true);
    if (indriPINVOKE.SWIGPendingException.Pending) throw indriPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public IntVector documentIDsFromMetadata(string attributeName, StringVector attributeValue) {
    IntVector ret = new IntVector(indriPINVOKE.QueryEnvironment_documentIDsFromMetadata(swigCPtr, attributeName, StringVector.getCPtr(attributeValue)), true);
    if (indriPINVOKE.SWIGPendingException.Pending) throw indriPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public ParsedDocumentVector documentsFromMetadata(string attributeName, StringVector attributeValue) {
    ParsedDocumentVector ret = new ParsedDocumentVector(indriPINVOKE.QueryEnvironment_documentsFromMetadata(swigCPtr, attributeName, StringVector.getCPtr(attributeValue)), true);
    if (indriPINVOKE.SWIGPendingException.Pending) throw indriPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public long termCount() {
    long ret = indriPINVOKE.QueryEnvironment_termCount__SWIG_0(swigCPtr);
    if (indriPINVOKE.SWIGPendingException.Pending) throw indriPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public long termCount(string term) {
    long ret = indriPINVOKE.QueryEnvironment_termCount__SWIG_1(swigCPtr, term);
    if (indriPINVOKE.SWIGPendingException.Pending) throw indriPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public long termFieldCount(string term, string field) {
    long ret = indriPINVOKE.QueryEnvironment_termFieldCount(swigCPtr, term, field);
    if (indriPINVOKE.SWIGPendingException.Pending) throw indriPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public StringVector fieldList() {
    StringVector ret = new StringVector(indriPINVOKE.QueryEnvironment_fieldList(swigCPtr), true);
    if (indriPINVOKE.SWIGPendingException.Pending) throw indriPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public long documentCount() {
    long ret = indriPINVOKE.QueryEnvironment_documentCount__SWIG_0(swigCPtr);
    if (indriPINVOKE.SWIGPendingException.Pending) throw indriPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public long documentCount(string term) {
    long ret = indriPINVOKE.QueryEnvironment_documentCount__SWIG_1(swigCPtr, term);
    if (indriPINVOKE.SWIGPendingException.Pending) throw indriPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public DocumentVectorVector documentVectors(IntVector documentIDs) {
    DocumentVectorVector ret = new DocumentVectorVector(indriPINVOKE.QueryEnvironment_documentVectors(swigCPtr, IntVector.getCPtr(documentIDs)), true);
    if (indriPINVOKE.SWIGPendingException.Pending) throw indriPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

}

}