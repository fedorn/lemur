#ifdef SWIGJAVA
%typemap(jni) lemur::api::IndexedRealVector* "jobjectArray"
%typemap(jtype) lemur::api::IndexedRealVector* "IndexedReal[]"
%typemap(jstype) lemur::api::IndexedRealVector* "IndexedReal[]"

%typemap(jni) lemur::api::IndexedRealVector "jobjectArray"
%typemap(jtype) lemur::api::IndexedRealVector "IndexedReal[]"
%typemap(jstype) lemur::api::IndexedRealVector "IndexedReal[]"

%typemap(jni) lemur::api::IndexedRealVector& "jobjectArray"
%typemap(jtype) lemur::api::IndexedRealVector& "IndexedReal[]"
%typemap(jstype) lemur::api::IndexedRealVector& "IndexedReal[]"

%{

  void java_fill_indexedrealvector( JNIEnv* jenv, const lemur::api::IndexedRealVector& input, jobjectArray result ) {
    if (!result) {
      return ;
    }
    jclass clazz = jenv->FindClass("lemurproject/lemur/IndexedReal");
    jmethodID constructor = jenv->GetMethodID(clazz, "<init>", "()V" );
    jfieldID scoreField = jenv->GetFieldID(clazz, "val", "D" );
    jfieldID idField = jenv->GetFieldID(clazz, "ind", "I" );
    jsize size = jenv->GetArrayLength(result);
    jsize limit = std::min((size_t)size, input.size());

    for( jsize i=0; i<limit; i++ ) {
      jobject ser = jenv->NewObject(clazz, constructor);
      jenv->SetDoubleField(ser, scoreField, input[i].val );
      jenv->SetIntField(ser, idField, input[i].ind );
      jenv->SetObjectArrayElement(result, i, ser);
    }
  }
  jobjectArray java_build_indexedrealvector( JNIEnv* jenv, const lemur::api::IndexedRealVector& input ) {
    jclass clazz = jenv->FindClass("lemurproject/lemur/IndexedReal");
    jobjectArray result;
    result = jenv->NewObjectArray(input.size(), clazz, NULL);
    java_fill_indexedrealvector(jenv, input, result);
    return result;
  }

  %}

%typemap(out) lemur::api::IndexedRealVector* {
  $result = java_build_indexedrealvector( jenv, *($1) );
  delete($1);
}

%typemap(javaout) lemur::api::IndexedRealVector* {
  return $jnicall;
}

%typemap(out) lemur::api::IndexedRealVector& {
  $result = java_build_indexedrealvector( jenv, $1 );
}

%typemap(javaout) const lemur::api::IndexedRealVector& {
  return $jnicall;
}

%typemap(in) lemur::api::IndexedRealVector& ( lemur::api::IndexedRealVector resin )
{
  jsize size = jenv->GetArrayLength($input);
  $1 = &resin;
  $1->resize(size);
  // ought ot actually fill in the values here.
}

%typemap(argout) lemur::api::IndexedRealVector& {
  java_fill_indexedrealvector( jenv, *($1), $input );
}

%typemap(javain) lemur::api::IndexedRealVector& "$javainput"
#endif

#ifdef SWIGCSHARP

%typemap(ctype) lemur::api::IndexedReal "void *"
%typemap(imtype, out="IntPtr") lemur::api::IndexedReal "HandleRef"
%typemap(cstype) lemur::api::IndexedReal "IndexedReal"

%typemap(ctype) lemur::api::IndexedReal* "void *"
%typemap(imtype, out="IntPtr") lemur::api::IndexedReal* "HandleRef"
%typemap(cstype) lemur::api::IndexedReal* "IndexedReal"

%typemap(ctype) lemur::api::IndexedReal& "void *"
%typemap(imtype, out="IntPtr") lemur::api::IndexedReal& "HandleRef"
%typemap(cstype) lemur::api::IndexedReal& "IndexedReal"

SWIG_STD_VECTOR_SPECIALIZE_MINIMUM(IndexedReal, lemur::api::IndexedReal)
  %template(IRVector) std::vector<lemur::api::IndexedReal>;

namespace lemur {
  namespace api 
  {
    struct IndexedReal 
    {
    public:
      int ind;
      double val;
    };

    class IndexedRealVector : public std::vector<lemur::api::IndexedReal> {
    };
  }
}
    
#endif
