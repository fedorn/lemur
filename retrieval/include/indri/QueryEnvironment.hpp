/*==========================================================================
 * Copyright (c) 2004 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/


//
// QueryEnvironment
//
// 9 March 2004 -- tds
//

#ifndef INDRI_QUERYENVIRONMENT_HPP
#define INDRI_QUERYENVIRONMENT_HPP

#include "indri/ScoredExtentResult.hpp"
#include "indri/QueryServer.hpp"
#include "indri/NetworkStream.hpp"
#include "indri/NetworkMessageStream.hpp"
#include "indri/Parameters.hpp"
#include "indri/PriorFactory.hpp"
#include "indri/ParsedDocument.hpp"
#include "indri/Repository.hpp"
#include "indri/QueryAnnotation.hpp"

/*! <p>Principal class for interacting with Indri indexes during retrieval. 
  Provides the API for opening one or more Repository servers, either local
  or remote. Provides the API for querying the servers with the Indri
  query language, and additionally requesting aggregate collection 
  statistics.
  <p> QueryEnviroment parameters.
 */

class QueryEnvironment {
private:
  std::vector<QueryServer*> _servers;

  std::vector<Repository*> _repositories;
  std::vector<NetworkStream*> _streams;
  std::vector<NetworkMessageStream*> _messageStreams;

  Parameters _parameters;
  PriorFactory _priorFactory;

  void _mergeQueryResults( InferenceNetwork::MAllResults& results, std::vector<QueryServerResponse*>& responses );
  void _copyStatistics( std::vector<indri::lang::RawScorerNode*>& scorerNodes, InferenceNetwork::MAllResults& statisticsResults );

  std::vector<QueryServerResponse*> _runServerQuery( std::vector<indri::lang::Node*>& roots, int resultsRequested );
  void _sumServerQuery( InferenceNetwork::MAllResults& results, std::vector<indri::lang::Node*>& roots, int resultsRequested );
  void _mergeServerQuery( InferenceNetwork::MAllResults& results, std::vector<indri::lang::Node*>& roots, int resultsRequested );
  void _annotateQuery( InferenceNetwork::MAllResults& results, const std::vector<DOCID_T>& documentIDs, std::string& annotatorName, indri::lang::Node* queryRoot );

  std::vector<ScoredExtentResult> _runQuery( InferenceNetwork::MAllResults& results,
                                             const std::string& q,
                                             int resultsRequested,
                                             const std::vector<DOCID_T>* documentIDs,
                                             QueryAnnotation** annotation );
  void _scoredQuery( InferenceNetwork::MAllResults& results, indri::lang::Node* queryRoot, std::string& accumulatorName, int resultsRequested, const std::vector<DOCID_T>* documentSet );

  QueryEnvironment( QueryEnvironment& other ) {}

public:
  QueryEnvironment();
  ~QueryEnvironment();
  /// \brief Set the amount of memory to use.
  /// @param memory number of bytes to allocate
  void setMemory( UINT64 memory );
  /// \brief Set the scoring rules
  /// @param rules the vector of scoring rules.
  void setScoringRules( const std::vector<std::string>& rules );
  /// \brief Set the stopword list for query processing
  /// @param stopwords the list of stopwords
  void setStopwords( const std::vector<std::string>& stopwords );
  /// \brief Add a remote server
  /// @param hostname the host the server is running on
  void addServer( const std::string& hostname );
  /// \brief Add a local repository
  /// @param pathname the path to the repository.
  void addIndex( const std::string& pathname );
  /// Close the QueryEnvironment.
  void close();
  /// \brief Run an Indri query language query. @see ScoredExtentResult
  /// @param query the query to run
  /// @param resultsRequested maximum number of results to return
  /// @return the vector of ScoredExtentResults for the query
  std::vector<ScoredExtentResult> runQuery( const std::string& query, int resultsRequested );

std::vector<ScoredExtentResult> runQuery( const std::string& query, const std::vector<DOCID_T>& documentSet, int resultsRequested );

  /// \brief Run an Indri query language query. @see QueryAnnotation
  /// @param query the query to run
  /// @param resultsRequested maximum number of results to return
  /// @return pointer to QueryAnnotations for the query
  QueryAnnotation* runAnnotatedQuery( const std::string& query, int resultsRequested );  

  QueryAnnotation* runAnnotatedQuery( const std::string& query, const std::vector<DOCID_T>& documentSet, int resultsRequested );


  /// \brief Fetch the parsed documents for a given list of document ids.
  /// Caller is responsible for deleting the returned elements.
  /// @param documentIDs the list of ids
  /// @return the vector of ParsedDocument pointers.
  std::vector<ParsedDocument*> documents( const std::vector<DOCID_T>& documentIDs );
  /// \brief Fetch the parsed documents for a given list of ScoredExtentResults
  /// Caller is responsible for deleting the returned elements.
  /// @param results the list of ScoredExtentResults
  /// @return the vector of ParsedDocument pointers.
  std::vector<ParsedDocument*> documents( const std::vector<ScoredExtentResult>& results );
  /// \brief Fetch the named metadata attribute for a list of document ids
  /// @param documentIDs the list of ids
  /// @param attributeName the name of the metadata attribute
  /// @return the vector of string values for that attribute
  std::vector<std::string> documentMetadata( const std::vector<DOCID_T>& documentIDs, const std::string& attributeName );
  /// \brief Fetch the named metadata attribute for a list of ScoredExtentResults
  /// @param documentIDs the list of ScoredExtentResults
  /// @param attributeName the name of the metadata attribute
  /// @return the vector of string values for that attribute
  std::vector<std::string> documentMetadata( const std::vector<ScoredExtentResult>& documentIDs, const std::string& attributeName );
  /// \brief Return total number of terms.
  /// @return total number of terms in the aggregated collection
  INT64 termCount();
  /// \brief Return total number of term occurrences.
  /// @param term the term to count
  /// @return total frequency of this term in the aggregated collection
  INT64 termCount( const std::string& term );
  /// \brief Return total number of stem occurrences.
  /// @param term the stem to count
  /// @return total frequency of this stem in the aggregated collection
  INT64 stemCount( const std::string& term );
  /// \brief Return total number of term occurrences within a field.
  /// @param term the term to count
  /// @param field the name of the field
  /// @return total frequency of this term within this field in the 
  /// aggregated collection
  INT64 termFieldCount( const std::string& term, const std::string& field );
  /// \brief Return total number of stem occurrences within a field.
  /// @param term the stem to count
  /// @param field the name of the field
  /// @return total frequency of this stem within this field in the 
  /// aggregated collection
  INT64 stemFieldCount( const std::string& term, const std::string& field );
  /// \brief Return the list of fields.
  /// @return vector of field names.
  std::vector<std::string> fieldList();
  /// \brief Return total number of documents in the collection.
  /// @return total number of documents in the aggregated collection
  INT64 documentCount();
  INT64 documentCount( const std::string& term );
  /// \brief Fetch a document vector for a list of documents. 
  /// Caller responsible for deleting the Vector.
  /// @param documentIDs the vector of document ids.
  /// @return DocumentVector pointer for the specified document.
  std::vector<DocumentVector*> documentVectors( const std::vector<DOCID_T>& documentIDs );
};

#endif // INDRI_QUERYENVIRONMENT_HPP

