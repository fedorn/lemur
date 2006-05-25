%typemap(javaimports) indri::api::QueryEnvironment "import java.util.Map;"
namespace indri {
  namespace api {
class QueryEnvironment {

  %newobject runAnnotatedQuery;

public:
        QueryEnvironment();
  void addServer( const std::string& hostname ) throw (lemur::api::Exception);
  void addIndex( const std::string& pathname ) throw (lemur::api::Exception);
  void removeServer( const std::string& hostname ) throw (lemur::api::Exception);
  void removeIndex( const std::string& pathname ) throw (lemur::api::Exception);
  void close() throw (lemur::api::Exception);
  
  void setMemory( UINT64 memory ) throw (lemur::api::Exception);
  void setScoringRules( const std::vector<std::string>& rules ) throw (lemur::api::Exception);
  void setStopwords( const std::vector<std::string>& stopwords ) throw (lemur::api::Exception);
  
  std::vector<indri::api::ScoredExtentResult> runQuery( const std::string& query, int resultsRequested ) throw (lemur::api::Exception) ;
  std::vector<indri::api::ScoredExtentResult> runQuery( const std::string& query, const std::vector<int>& documentSet, int resultsRequested ) throw (lemur::api::Exception);
  indri::api::QueryAnnotation* runAnnotatedQuery( const std::string& query, int resultsRequested ) throw (lemur::api::Exception);
  indri::api::QueryAnnotation* runAnnotatedQuery( const std::string& query, const std::vector<int>& documentSet, int resultsRequested ) throw (lemur::api::Exception);
  
  std::vector<indri::api::ParsedDocument*> documents( const std::vector<int>& documentIDs ) throw (lemur::api::Exception);
  std::vector<indri::api::ParsedDocument*> documents( const std::vector<indri::api::ScoredExtentResult>& results ) throw (lemur::api::Exception);

  std::vector<std::string> documentMetadata( const std::vector<int>& documentIDs, const std::string& attributeName ) throw (lemur::api::Exception);
  std::vector<std::string> documentMetadata( const std::vector<indri::api::ScoredExtentResult>& documentIDs, const std::string& attributeName ) throw (lemur::api::Exception);

  std::vector<int> documentIDsFromMetadata( const std::string& attributeName, const std::vector<std::string>& attributeValue ) throw (lemur::api::Exception);
  std::vector<indri::api::ParsedDocument*> documentsFromMetadata( const std::string& attributeName, const std::vector<std::string>& attributeValue ) throw (lemur::api::Exception);

  INT64 termCount() throw (lemur::api::Exception);
  INT64 termCount( const std::string& term ) throw (lemur::api::Exception);
  INT64 termFieldCount( const std::string& term, const std::string& field ) throw (lemur::api::Exception);
  std::vector<std::string> fieldList() throw (lemur::api::Exception);
  INT64 documentCount() throw (lemur::api::Exception);
  INT64 documentCount( const std::string& term ) throw (lemur::api::Exception);

  std::vector<indri::api::DocumentVector*> documentVectors( const std::vector<int>& documentIDs ) throw (lemur::api::Exception);

  double expressionCount( const std::string& expression, const std::string &queryType = "indri" );
  std::vector<indri::api::ScoredExtentResult> expressionList( const std::string& expression,  const std::string& queryType = "indri" );

};

}}
