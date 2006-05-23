#ifdef SWIGCSHARP
%module lemur_csharp
#else
%module lemur
#endif
%{
  // includes for the lemur classes
  // could be put in the various individual .i files
#include "IndexManager.hpp"
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
%include "Exception.i"
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
    void ParamSet(const std::string &key, const std::string &value)  throw (lemur::api::Exception);
    void ParamClear()  throw (lemur::api::Exception);
    int ParamPushFile (const std::string &s)  throw (lemur::api::Exception);
    std::string ParamPopFile ()  throw (lemur::api::Exception);
  }
}

