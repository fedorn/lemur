#ifdef SWIGJAVA
%module (directors="1") indri
#endif
%{
#include "indri/indri-platform.h"
#include "lemur-compat.hpp"
#include "indri/QueryEnvironment.hpp"
#include "indri/ScoredExtentResult.hpp"
#include "indri/ParsedDocument.hpp"
#include "indri/IndexEnvironment.hpp"
#include "indri/Parameters.hpp"
#include "indri/ConflationPattern.hpp"
#include "Exception.hpp"
%}

typedef long long INT64;
typedef long long UINT64;

%include "std_string.i"
%include "exception.i"

#ifdef SWIGCSHARP
%module (directors="1") indri

%apply const std::string & {std::string &};

%include "std_map.i"
%include "std_vector.i"

%include "Exception.i"
%include "IntVector.i"
%include "StringVector.i"
%include "ConflationPattern.i"
%include "MResults.i"
%include "MetadataPairVector.i"
%include "ParsedDocument.i"
%include "QueryAnnotationNode.i"
%include "ScoredExtentArray.i"
%include "Parameters.i"
%include "DocumentVector.i"
%include "StringMap.i"
%include "Specification.i"
%include "QueryEnvironment.i"
%include "IndexEnvironment.i"
#endif

#ifdef SWIGJAVA
%pragma(java) jniclassimports="import java.util.Map;";

%include "Exception.i"
%include "ConflationPattern.i"
%include "MResults.i"
%include "ScoredExtentArray.i"
%include "ParsedDocument.i"
%include "QueryAnnotationNode.i"

%include "IntVector.i"
%include "StringVector.i"
%include "Parameters.i"
%include "DocumentVector.i"
%include "MetadataPairVector.i"
%include "StringMap.i"
%include "Specification.i"
%include "QueryEnvironment.i"
%include "IndexEnvironment.i"
#ifdef INDRI_STANDALONE
%pragma(java) jniclasscode=%{
  static {
    System.loadLibrary("indri_jni");
  }
%}
#else
%pragma(java) jniclasscode=%{
  static {
    System.loadLibrary("lemur_jni");
  }
%}
#endif
#endif
#ifdef SWIGPHP4
%module indri
%{
#include "indri/indri-platform.h"
#include "lemur-compat.hpp"
#include "indri/QueryEnvironment.hpp"
#include "Exception.hpp"
// remap overloaded method names.
#define onetermCount termCount
#define onedocumentCount documentCount
#define runQuerydocset runQuery
#define runAnnotatedQuerydocset runAnnotatedQuery
#define documentsdocids documents
#define documentMetadatadocids documentMetadata
%}

%include "typemaps.i"
%include "std_string.i"
%include "exception.i"
%include "indritypemaps.i"
%include "Exception.i"

namespace indri{
namespace parse{
struct TermExtent {
  int begin;
  int end;
public:
  ~TermExtent();
};

}

namespace api{

class ScoredExtentResult {
  double score;
  int document;
  int begin;
  int end;
public:
  ~ScoredExtentResult();
};


class ParsedDocument {
  const char* text;
  size_t textLength;

  const char* content;
  size_t contentLength;

  greedy_vector<char*> terms;
  greedy_vector<indri::parse::TagExtent> tags;
  greedy_vector<indri::parse::TermExtent> positions;
  greedy_vector<indri::parse::MetadataPair> metadata;
public:
  ~ParsedDocument();
  std::string getContent();
};


class QueryAnnotationNode {
  std::string name;
  std::string type;
  std::string queryText;
  std::vector<QueryAnnotationNode*> children;
};


class QueryAnnotation {
public:
  const QueryAnnotationNode* getQueryTree() const;
  const indri::infnet::EvaluatorNode::MResults& getAnnotations() const;
  const std::vector<ScoredExtentResult>& getResults() const;
  ~QueryAnnotation();
};

setEx(QueryEnvironment::runQuery);
setEx(QueryEnvironment::runQuerydocset);
setEx(QueryEnvironment::runAnnotatedQuery);
setEx(QueryEnvironment::runAnnotatedQuerydocset);
setEx(QueryEnvironment::documents);
setEx(QueryEnvironment::documentsdocids);
setEx(QueryEnvironment::documentMetadata)
setEx(QueryEnvironment::documentMetadatadocids);

class QueryEnvironment {
public:
  ~QueryEnvironment();
  void addServer( const std::string& hostname );
  void addIndex( const std::string& pathname );
  void close();
  
  void setMemory( UINT64 memory );
  void setScoringRules( const std::vector<std::string>& rules );
  void setStopwords( const std::vector<std::string>& stopwords );

  std::vector<ScoredExtentResult> runQuery( const std::string& query, int resultsRequested );
  std::vector<ScoredExtentResult> runQuerydocset( const std::string& query, const std::vector<lemur::api::DOCID_T>& documentSet, int resultsRequested );

  %newobject runAnnotatedQuery;
  %newobject runAnnotatedQuerydocset;
  QueryAnnotation* runAnnotatedQuery( const std::string& query, int resultsRequested );
  QueryAnnotation* runAnnotatedQuerydocset( const std::string& query, const std::vector<lemur::api::DOCID_T>& documentSet, int resultsRequested );

  std::vector<ParsedDocument*> documentsdocids( const std::vector<lemur::api::DOCID_T>& documentIDs );
  std::vector<ParsedDocument*> documents( const std::vector<ScoredExtentResult>& results );

  std::vector<std::string> documentMetadatadocids( const std::vector<lemur::api::DOCID_T>& documentIDs, const std::string& attributeName );
  std::vector<std::string> documentMetadata( const std::vector<ScoredExtentResult>& documentIDs, const std::string& attributeName );

  INT64 termCount();
  INT64 onetermCount( const std::string& term );
  INT64 stemCount( const std::string& term );
  INT64 termFieldCount( const std::string& term, const std::string& field );
  INT64 stemFieldCount( const std::string& term, const std::string& field );
  std::vector<std::string> fieldList();
  INT64 documentCount();
  INT64 onedocumentCount( const std::string& term );
  double expressionCount( const std::string& expression, const std::string &queryType = "indri" );
  std::vector<indri::api::ScoredExtentResult> expressionList( const std::string& expression,  const std::string& queryType = "indri" );
};
}}
#endif
