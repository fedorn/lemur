#ifdef SWIGJAVA
%typemap(jni) lemur::api::RetrievalMethod *lemur::api::RetMethodManager::createModel "jobject"
%typemap(jtype) lemur::api::RetrievalMethod *lemur::api::RetMethodManager::createModel "RetrievalMethod"
%typemap(jstype) lemur::api::RetrievalMethod *lemur::api::RetMethodManager::createModel "RetrievalMethod"

%typemap(javaout) lemur::api::RetrievalMethod * {
  return $jnicall;
}

%typemap(out) lemur::api::RetrievalMethod * lemur::api::RetMethodManager::createModel
{
  lemur::api::TextQueryRetMethod *t = dynamic_cast<lemur::api::TextQueryRetMethod*>($1);
  lemur::api::StructQueryRetMethod *s = dynamic_cast<lemur::api::StructQueryRetMethod*>($1);
  lemur::retrieval::IndriRetMethod *ind = dynamic_cast<lemur::retrieval::IndriRetMethod*>($1);
  if (t) {
    jclass clazz = jenv->FindClass("lemurproject/lemur/TextQueryRetMethod");
    if (clazz) {
      jmethodID mid = jenv->GetMethodID(clazz, "<init>", "(JZ)V");
      if (mid) {
        jlong cptr = 0;
        *(lemur::api::TextQueryRetMethod **)&cptr = t;
        $result = jenv->NewObject(clazz, mid, cptr, true);
      }
    }
  } else if (s) {
    jclass clazz = jenv->FindClass("lemurproject/lemur/StructQueryRetMethod");
    if (clazz) {
      jmethodID mid = jenv->GetMethodID(clazz, "<init>", "(JZ)V");
      if (mid) {
        jlong cptr = 0;
        *(lemur::api::StructQueryRetMethod **)&cptr = s;
        $result = jenv->NewObject(clazz, mid, cptr, true);
      }
    }
  }  else if (ind) {
    jclass clazz = jenv->FindClass("lemurproject/lemur/IndriRetMethod");
    if (clazz) {
      jmethodID mid = jenv->GetMethodID(clazz, "<init>", "(JZ)V");
      if (mid) {
        jlong cptr = 0;
        *(lemur::retrieval::IndriRetMethod **)&cptr = ind;
        $result = jenv->NewObject(clazz, mid, cptr, true);
      }
    }
  } else {
    $result = NULL;
  }
}
#endif
    
namespace lemur {
  namespace utility 
  {
    %nodefault WeightedIDSet;
    
    class WeightedIDSet {
    };
  }
  
  namespace api {
    
    class QueryRep 
    {
    };

    // needs a downcast typemap?
    class Query
    {
      %newobject makeQuery;
    public:
      %extend {
#ifdef SWIGJAVA
        %javamethodmodifiers  "
/** Create a stopped, stemmed Query from a string.
@param query the input query
@param stopfile optional file containing a list of stopwords
@param stemtype optional stemmer type
@param datadir optional ignored
@param func optional stemmer function for arabic stemming.
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

        // should get the params off the param stack
        static lemur::api::Query *makeQuery(std::string query, const std::string& stopfile="", const std::string& stemtype="", const std::string& datadir="", const std::string& func="") throw (lemur::api::Exception) {
          lemur::api::Stopper* stopper = lemur::api::TextHandlerManager::createStopper(stopfile);
          lemur::api::Stemmer* stemmer = lemur::api::TextHandlerManager::createStemmer(stemtype, datadir, func);
          lemur::parse::StringQuery* parser = NULL;
          lemur::parse::StringQuery* qryterms;
          lemur::api::TextHandler* chain;

          if (stopper || stemmer) {
            parser = new lemur::parse::StringQuery();
            chain = parser;
            qryterms = new lemur::parse::StringQuery();

            if (stopper) {
              chain->setTextHandler(stopper);
              chain=stopper;
            }
    
            if (stemmer) {
              chain->setTextHandler(stemmer);    
              chain=stemmer;
            }
    
            chain->setTextHandler(qryterms);
            parser->parse(query);

            delete(parser);
    
          } else {
            qryterms = new lemur::parse::StringQuery(query);
          }
          return qryterms;
        }
      }
    };

    %typemap(javaclassmodifiers) RetrievalMethod "public abstract class"
    
    class RetrievalMethod {
    public:
      // needs a typemap
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
Score all documents in the collection
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      virtual void scoreCollection(const lemur::api::QueryRep &qry, lemur::api::IndexedRealVector &results) throw (lemur::api::Exception);
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
update the query, feedback support
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      virtual void updateQuery (lemur::api::QueryRep &qryRep, const lemur::api::DocIDSet &relDocs) throw (lemur::api::Exception) = 0;      
      %typemap(javacode) RetrievalMethod %{
        /**
           Run a string query. Specialized by RetrievalMethod subclasses.
           @param searchQuery the query to run
           @return array of IndexedReal results
           @throws Exception if a lemur::api::Exception was thrown by the JNI library.
        */

public  abstract IndexedReal[] runQuery(String searchQuery) throws Exception;
 %}
    };

    class TextQueryRetMethod : public RetrievalMethod {
      %typemap(javacode) TextQueryRetMethod %{
        /**
           Run a string query.
           @param searchQuery the query to run
           @return array of IndexedReal results
           @throws Exception if a lemur::api::Exception was thrown by the JNI library.
        */

public  IndexedReal[] runQuery(String searchQuery)  throws Exception {
  return RetMethodManager.runQuery(searchQuery, this);
}
 %}
      // this doesn't address the need for dynamic_casting. Bleah.
#ifdef SWIGCSHARP
      %typemap(cscode) TextQueryRetMethod %{
  public TextQueryRetMethod(RetrievalMethod m) : this((IntPtr)RetrievalMethod.getCPtr(m), false) {}
  %};
#endif
    };
    class StructQueryRetMethod : public RetrievalMethod {
      %typemap(javacode) StructQueryRetMethod %{
        /**
           Run a InQuery structured query language query.
           @param searchQuery the query to run
           @return array of IndexedReal results
           @throws Exception if a lemur::api::Exception was thrown by the JNI library.
        */

public IndexedReal[] runQuery(String searchQuery) throws Exception {
  return RetMethodManager.runQuery(searchQuery, this);
}
 %}
#ifdef SWIGCSHARP
      %typemap(cscode) StructQueryRetMethod %{
  public StructQueryRetMethod(RetrievalMethod m) : this((IntPtr)RetrievalMethod.getCPtr(m), false) {}
  %};
#endif

    };

  }
  namespace retrieval 
  {
    class IndriRetMethod : public lemur::api::RetrievalMethod {
      %typemap(javacode)  IndriRetMethod %{
        /**
           Run an Indri structured query.
           @param searchQuery the query to run
           @return array of IndexedReal results
           @throws Exception if a lemur::api::Exception was thrown by the JNI library.
        */

public IndexedReal[] runQuery(String searchQuery) throws Exception {
  return RetMethodManager.runQuery(searchQuery, this);
}
 %}
    };
  }
  namespace api 
  {
    
    %nodefault RetMethodManager;
    %newobject RetMethodManager::createModel;    
    class RetMethodManager {
    public:
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
Create a retrieval model
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      static lemur::api::RetrievalMethod* createModel(const lemur::api::Index* ind, lemur::api::ScoreAccumulator* accum, std::string type = "") throw (lemur::api::Exception);
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
      Automatically generate query models from the given query and run through
      given retrieval model. Stopword file and stemmer are optional
      query terms delimited by space
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      static lemur::api::IndexedRealVector* runTextQuery(const std::string& query, lemur::api::TextQueryRetMethod* model, const std::string& stopfile="", const std::string& stemtype="", const std::string& datadir="", const std::string& func="") throw (lemur::api::Exception);
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
      Automatically generate query models from the given query and run through
      given retrieval model. Stopword file and stemmer are optional
      query terms delimited by space
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      static lemur::api::IndexedRealVector* runStructQuery(const std::string& query, lemur::api::StructQueryRetMethod* model, const std::string& stopfile="", const std::string& stemtype="", const std::string& datadir="", const std::string& func="") throw (lemur::api::Exception);

      %extend { 
#ifdef SWIGJAVA
        %javamethodmodifiers  "
/**
      Automatically generate query models from the given query and run through
      given retrieval model. Stopword file and stemmer are optional
      query terms delimited by space
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

        static lemur::api::IndexedRealVector* runIndriQuery(const std::string& query, lemur::retrieval::IndriRetMethod* model) throw (lemur::api::Exception) {
          lemur::api::IndexedRealVector *tmp = new lemur::api::IndexedRealVector();
          model->scoreCollection(query, *tmp);
          return tmp;
        }
#ifdef SWIGCSHARP
        static lemur::api::IndexedRealVector* runQuery(const std::string& query, lemur::api::RetrievalMethod* model) {
          lemur::api::TextQueryRetMethod *t = dynamic_cast<lemur::api::TextQueryRetMethod*>(model);
          lemur::api::StructQueryRetMethod *s = dynamic_cast<lemur::api::StructQueryRetMethod*>(model);
          lemur::retrieval::IndriRetMethod *ind = dynamic_cast<lemur::retrieval::IndriRetMethod*>(model);
          if (t)
            return lemur::api::RetMethodManager::runTextQuery(query, t);
          if (s)
            return lemur::api::RetMethodManager::runStructQuery(query, s);
          if (ind) {
            lemur::api::IndexedRealVector *tmp = new lemur::api::IndexedRealVector();
            ind->scoreCollection(query, *tmp);
            return tmp;
          }
          return NULL;
        }
#endif
      }

      %typemap(javacode) RetMethodManager %{
        /**
           Run a string query with a given retrieval method instance.
           @param searchQuery the query to run
           @param m The retrieval method to use.
           @return array of IndexedReal results
           @throws Exception if a lemur::api::Exception was thrown by the JNI library.
        */

public static IndexedReal[] runQuery(String searchQuery, IndriRetMethod m) throws Exception
  {
    return RetMethodManager.runIndriQuery(searchQuery, m);
  }
/**
   Run a string query with a given retrieval method instance.
   @param searchQuery the query to run
   @param m The retrieval method to use.
   @return array of IndexedReal results
   @throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/

public static IndexedReal[] runQuery(String searchQuery, StructQueryRetMethod m) throws Exception
  {
    return RetMethodManager.runStructQuery(searchQuery, m);
  }
/**
   Run a string query with a given retrieval method instance.
   @param searchQuery the query to run
   @param m The retrieval method to use.
   @return array of IndexedReal results
   @throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/

public static IndexedReal[] runQuery(String searchQuery, TextQueryRetMethod m) throws Exception
  {
    return RetMethodManager.runTextQuery(searchQuery, m);
  }
 
 %};
    };    
  }
}
