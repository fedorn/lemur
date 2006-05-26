#ifdef SWIGJAVA
%typemap(jni) lemur::api::TMatch* "jobjectArray"
%typemap(jtype) lemur::api::TMatch* "TMatch[]"
%typemap(jstype) lemur::api::TMatch* "TMatch[]"

%{
  jobjectArray java_build_matchinfo( JNIEnv* jenv, const lemur::api::MatchInfo* input ) {
    jclass clazz = jenv->FindClass("lemurproject/lemur/TMatch");
    jmethodID constructor = jenv->GetMethodID(clazz, "<init>", "()V" );
    jobjectArray result;

    result = jenv->NewObjectArray(input->size(), clazz, NULL);
    if (!result) {
      return 0;
    }

    jfieldID idField = jenv->GetFieldID(clazz, "tid", "I" );
    jfieldID startField = jenv->GetFieldID(clazz, "start", "I" );
    jfieldID endField = jenv->GetFieldID(clazz, "end", "I" );
    jfieldID positionField = jenv->GetFieldID(clazz, "position", "I" );

    for( jsize i=0; i<input->size(); i++ ) {
      // make a new scored extent result object
      jobject ser = jenv->NewObject(clazz, constructor);

      // fill in the fields
      jenv->SetIntField(ser, idField, (*input)[i].tid );
      jenv->SetIntField(ser, startField, (*input)[i].start );
      jenv->SetIntField(ser, endField, (*input)[i].end );
      jenv->SetIntField(ser, positionField, (*input)[i].position );

      jenv->SetObjectArrayElement(result, i, ser);
    }
    return result;
  }
  %}

%typemap(out) lemur::api::TMatch* {
  $result = java_build_matchinfo( jenv, (const lemur::api::MatchInfo*)$1 );
  // clean up
  delete($1);
}

%typemap(javaout) lemur::api::TMatch* {
  return $jnicall;
}

#endif

#ifdef SWIGCSHARP
SWIG_STD_VECTOR_SPECIALIZE_MINIMUM(TMatch, lemur::api::TMatch)
  %template(TMatchVector) std::vector<lemur::api::TMatch>;

#endif

namespace lemur {
  namespace api {
#ifdef SWIGCSHARP
    %nodefault TMatch;
    struct TMatch {
    public:
      int tid;
      int start;
      int end;
      int position;
    };
    %nodefault MatchInfo;
    class MatchInfo : public std::vector<lemur::api::TMatch> {
    public:
      static lemur::api::MatchInfo* getMatches(const lemur::api::Index
                                               &ind, const lemur::api::Query &qry, lemur::api::DOCID_T docID) ;
    };
#endif
#ifdef SWIGJAVA
    %nodefault MatchInfo;
    class MatchInfo {
    public:
      // this gets generated with the wrong intermediate type. Bleah
      // punning the type will work, but the cast is bogus.
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
       Construct the match list for a given query/document pair.
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      static lemur::api::TMatch* getMatches(const lemur::api::Index
                                            &ind, const lemur::api::Query &qry, lemur::api::DOCID_T docID) throw (lemur::api::Exception);
    };
#endif
  }
}
