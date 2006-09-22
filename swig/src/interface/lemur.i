#ifdef SWIGCSHARP
%module lemur_csharp
#else
%module lemur
#endif
%{
  // includes for the lemur classes
  // could be put in the various individual .i files
#include "IndexManager.hpp"
#include "BasicCollectionProps.hpp"
#include "RetMethodManager.hpp"
#include "StructQueryRetMethod.hpp"
#include "TextQueryRetMethod.hpp"
#include "MatchInfo.hpp"
#include "ElemDocMgr.hpp"
  %}

// swig library typemaps
%include "std_string.i"
%include "std_vector.i"
%include "exception.i"
%include "typemaps.i"

#ifdef SWIGJAVA

%pragma(java) jniclasscode=%{
  static {
    System.loadLibrary("lemur_jni");
  }
  %}
#endif

// lemur components
%include "LemurException.i"
%include "typedefs.i"
%include "TermInfoList.i"
%include "DocInfoList.i"
%include "IndexedRealVector.i"
%include "DocumentManager.i"
%include "Index.i"
%include "RetMethod.i"
%include "ScoreAccumulator.i"
%include "MatchInfo.i"
//%include "Parameters.i"
namespace lemur {
  namespace api {
#ifdef SWIGJAVA
    %javamethodmodifiers  "
/**
    Add a parameter value pair. 
    If no parameter file has been pushed,
      a new empty Parameters object is put on the stack.
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

    void ParamSet(const std::string &key, const std::string &value) throw (lemur::api::Exception);
#ifdef SWIGJAVA
    %javamethodmodifiers  "
/**
Clear the parameter stack and deallocate all memory.
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

    void ParamClear()  throw (lemur::api::Exception);
#ifdef SWIGJAVA
    %javamethodmodifiers  "
/**
 Load parameters from a parameter file.
 Adds a set of parameters to the parameter stack.
      @param s the parameter file to load.
      @return true if the file was loaded. false if it was not.
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

    int ParamPushFile (const std::string &s)  throw (lemur::api::Exception);
#ifdef SWIGJAVA
    %javamethodmodifiers  "
/**
 Remove parameters from the parameter stack.
 Removes the most recently pushed parameter file from the stack.
      @return an empty String
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

    std::string ParamPopFile ()  throw (lemur::api::Exception);
  }
}

