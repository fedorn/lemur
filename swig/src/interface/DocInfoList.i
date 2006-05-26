#ifdef SWIGJAVA
%typemap(jni) const lemur::api::LOC_T* "jintArray"
%typemap(jtype) const lemur::api::LOC_T* "int[]"
%typemap(jstype) const lemur::api::LOC_T* "int[]"

%typemap(out) const lemur::api::LOC_T* {
  const lemur::api::LOC_T* tmp = $1;
  int count = arg1->termCount();
  $result = jenv->NewIntArray(count);
  if (!$result) {
    return 0;
  }
  jint * body = jenv->GetIntArrayElements($result, 0);
  for( jsize i=0; i<count; i++ ) {
    body[i] = tmp[i];
  }
  jenv->ReleaseIntArrayElements($result, body, 0);
}

%typemap(javaout) const lemur::api::LOC_T* {
  return $jnicall;
}
#endif

#ifdef SWIGCSHARP
// need the right intermediate type here.
%typemap(ctype) const lemur::api::LOC_T* "int *"
%typemap(imtype) const lemur::api::LOC_T* "int[]"
%typemap(cstype) const lemur::api::LOC_T* "int[]"

%typemap(out) const lemur::api::LOC_T* {
  $result = $1;
}

%typemap(csout, excode=SWIGEXCODE) const lemur::api::LOC_T* {
  int[] ret= $imcall;$excode
                       return ret;
}
#endif

namespace lemur {
  namespace api {

    class DocInfo {
    public:

      DocInfo( lemur::api::DOCID_T termID, lemur::api::COUNT_T termCount);
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
@return the document id.
*/
public";
#endif

      lemur::api::DOCID_T docID() const;
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
@return term count in document.
*/
public";
#endif

      lemur::api::COUNT_T termCount() const;
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
@return list of positions this term occurs in this document
*/
public";
#endif

      // needs a map
      const lemur::api::LOC_T* positions() const;
    };
    class DocInfoList {
    public:
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
prepare iteration
*/
public";
#endif

      void startIteration() const=0;
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
test if there's any entry
*/
public";
#endif

      bool hasMore() const=0;
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
@return the next entry
*/
public";
#endif

      lemur::api::DocInfo *nextEntry() const=0;
    };
  }
}
