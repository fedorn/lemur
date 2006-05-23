#ifdef SWIGJAVA
%typemap(jni) const lemur::api::LOC_T* "jintArray"
%typemap(jtype) const lemur::api::LOC_T* "int[]"
%typemap(jstype) const lemur::api::LOC_T* "int[]"

%typemap(out) const lemur::api::LOC_T* {
  const lemur::api::LOC_T* tmp = $1;
  int count = arg1->count();
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
  int[] ret = $imcall;$excode
  return ret;
}
#endif

namespace lemur {
  namespace api {

    class TermInfo {
    public:
      TermInfo( lemur::api::TERMID_T termID, lemur::api::COUNT_T termCount);
      lemur::api::TERMID_T termID() const;
      lemur::api::COUNT_T count() const;
      // needs a map
      const lemur::api::LOC_T* positions() const;
      lemur::api::LOC_T position() const;
    };
    class TermInfoList {
    public:
      void startIteration() const=0;
      bool hasMore() const=0;
      lemur::api::TermInfo *nextEntry() const=0;
    };
  }
}
