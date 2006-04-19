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

#include <map>
#include "indri/ScoredExtentResult.hpp"
#include "indri/QueryServer.hpp"
#include "indri/NetworkStream.hpp"
#include "indri/NetworkMessageStream.hpp"
#include "indri/Parameters.hpp"
#include "indri/ParsedDocument.hpp"
#include "indri/Repository.hpp"
#include "indri/QueryAnnotation.hpp"
#include "IndexTypes.hpp"

namespace indri 
{
  namespace api 
  {
    
    /*! \brief Principal class for interacting with Indri indexes during retrieval. 
      Provides the API for opening one or more Repository servers, either local
      or remote. Provides the API for querying the servers with the Indri
      query language, and additionally requesting aggregate collection 
      statistics.
      <p> QueryEnviroment parameters.
    */
    class QueryEnvironment {
    private:
      // first is entry in _servers, second is entry _streams
      // derive idx to erase from those.
      std::map<std::string, std::pair<indri::server::QueryServer *, indri::net::NetworkStream *> > _serverNameMap;
      std::vector<indri::server::QueryServer*> _servers;
      // first is entry _servers, second is entry in _repositories.
      // derive idx to erase from those.
      std::map<std::string, std::pair<indri::server::QueryServer *, indri::collection::Repository *> > _repositoryNameMap;
      std::vector<indri::collection::Repository*> _repositories;
      std::vector<indri::net::NetworkStream*> _streams;
      std::vector<indri::net::NetworkMessageStream*> _messageStreams;

      Parameters _parameters;

      void _mergeQueryResults( indri::infnet::InferenceNetwork::MAllResults& results, std::vector<indri::server::QueryServerResponse*>& responses );
      void _copyStatistics( std::vector<indri::lang::RawScorerNode*>& scorerNodes, indri::infnet::InferenceNetwork::MAllResults& statisticsResults );

      std::vector<indri::server::QueryServerResponse*> _runServerQuery( std::vector<indri::lang::Node*>& roots, int resultsRequested );
      void _sumServerQuery( indri::infnet::InferenceNetwork::MAllResults& results, std::vector<indri::lang::Node*>& roots, int resultsRequested );
      void _mergeServerQuery( indri::infnet::InferenceNetwork::MAllResults& results, std::vector<indri::lang::Node*>& roots, int resultsRequested );
      void _annotateQuery( indri::infnet::InferenceNetwork::MAllResults& results, const std::vector<lemur::api::DOCID_T>& documentIDs, std::string& annotatorName, indri::lang::Node* queryRoot );

      std::vector<indri::api::ScoredExtentResult> _runQuery( indri::infnet::InferenceNetwork::MAllResults& results,
                                                             const std::string& q,
                                                             int resultsRequested,
                                                             const std::vector<lemur::api::DOCID_T>* documentIDs,
                                                             QueryAnnotation** annotation,
                                                             const std::string &queryType = "indri"  );
      void _scoredQuery( indri::infnet::InferenceNetwork::MAllResults& results, indri::lang::Node* queryRoot, std::string& accumulatorName, int resultsRequested, const std::vector<lemur::api::DOCID_T>* documentSet );

      QueryEnvironment( QueryEnvironment& other ) {}

    public:
      QueryEnvironment();
      ~QueryEnvironment();
      /// \brief Set the amount of memory to use.
      /// @param memory number of bytes to allocate
      void setMemory( UINT64 memory );
      /// \brief Set whether there should be one single background model or context sensitive models
      /// @param background true for one background model false for context sensitive models
      void setSingleBackgroundModel( bool background );
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
      /// Add an IndexEnvironment object.  Unlike the other add calls, this one
      /// will not close the index when QueryEnvironment::close is called.
      /// @param environment an IndexEnvironment instance
      void addIndex( class IndexEnvironment& environment );
      /// Close the QueryEnvironment.
      void close();
      /// \brief Remove a remote server
      /// @param hostname the host the server is running on
      void removeServer( const std::string& hostname );
      /// \brief Remove a local repository
      /// @param pathname the path to the repository.
      void removeIndex( const std::string& pathname );

      /// \brief Run an Indri query language query. @see ScoredExtentResult
      /// @param query the query to run
      /// @param resultsRequested maximum number of results to return
      /// @return the vector of ScoredExtentResults for the query
      std::vector<indri::api::ScoredExtentResult> runQuery( const std::string& query, int resultsRequested, const std::string &queryType = "indri" );

      std::vector<indri::api::ScoredExtentResult> runQuery( const std::string& query, const std::vector<lemur::api::DOCID_T>& documentSet, int resultsRequested, const std::string &queryType = "indri" );

      /// \brief Run an Indri query language query. @see QueryAnnotation
      /// @param query the query to run
      /// @param resultsRequested maximum number of results to return
      /// @return pointer to QueryAnnotations for the query
      QueryAnnotation* runAnnotatedQuery( const std::string& query, int resultsRequested, const std::string &queryType = "indri" );  

      QueryAnnotation* runAnnotatedQuery( const std::string& query, const std::vector<lemur::api::DOCID_T>& documentSet, int resultsRequested, const std::string &queryType = "indri" );


      /// \brief Fetch the parsed documents for a given list of document ids.
      /// Caller is responsible for deleting the returned elements.
      /// @param documentIDs the list of ids
      /// @return the vector of ParsedDocument pointers.
      std::vector<indri::api::ParsedDocument*> documents( const std::vector<lemur::api::DOCID_T>& documentIDs );
      /// \brief Fetch the parsed documents for a given list of ScoredExtentResults
      /// Caller is responsible for deleting the returned elements.
      /// @param results the list of ScoredExtentResults
      /// @return the vector of ParsedDocument pointers.
      std::vector<indri::api::ParsedDocument*> documents( const std::vector<indri::api::ScoredExtentResult>& results );
      /// \brief Fetch the named metadata attribute for a list of document ids
      /// @param documentIDs the list of ids
      /// @param attributeName the name of the metadata attribute
      /// @return the vector of string values for that attribute
      std::vector<std::string> documentMetadata( const std::vector<lemur::api::DOCID_T>& documentIDs, const std::string& attributeName );
      /// \brief Fetch the named metadata attribute for a list of ScoredExtentResults
      /// @param documentIDs the list of ScoredExtentResults
      /// @param attributeName the name of the metadata attribute
      /// @return the vector of string values for that attribute
      std::vector<std::string> documentMetadata( const std::vector<indri::api::ScoredExtentResult>& documentIDs, const std::string& attributeName );

      /// \brief Fetch the XPath names of extents for a list of ScoredExtentResults
      /// @param results the list of ScoredExtentResults
      /// @return the vector of string XPath names for the extents
      std::vector<std::string> pathNames( const std::vector<indri::api::ScoredExtentResult>& results );


      /// \brief Fetch all documents with a metadata key that matches attributeName, with a value matching one of the attributeValues.
      /// @param attributeName the name of the metadata attribute (e.g. 'url' or 'docno')
      /// @param attributeValues values that the metadata attribute should match
      /// @return a vector of ParsedDocuments that match the given metadata criteria
      std::vector<indri::api::ParsedDocument*> documentsFromMetadata( const std::string& attributeName, const std::vector<std::string>& attributeValues );

      /// \brief Return a list of document IDs where the document has a metadata key that matches attributeName, with a value matching one of the attributeValues.
      /// @param attributeName the name of the metadata attribute (e.g. 'url' or 'docno')
      /// @param attributeValue values that the metadata attribute should match
      /// @return a vector of ParsedDocuments that match the given metadata criteria
      std::vector<lemur::api::DOCID_T> documentIDsFromMetadata( const std::string& attributeName, const std::vector<std::string>& attributeValue );

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
      /// \brief Return the total number of times this expression appears in the collection.
      /// @param expression The expression to evaluate, probably an ordered or unordered window expression
      double expressionCount( const std::string& expression,
                              const std::string &queryType = "indri" );
      /// \brief Return all the occurrences of this expression in the collection.
      /// Note that the returned vector may be quite large for large collections, and therefore
      /// has the very real possibility of exhausting the memory of the machine.  Use this method
      /// with discretion.
      /// @param expression The expression to evaluate, probably an ordered or unordered window expression
      std::vector<ScoredExtentResult> expressionList( const std::string& expression, 
                                                      const std::string& queryType = "indri" );
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
      std::vector<DocumentVector*> documentVectors( const std::vector<lemur::api::DOCID_T>& documentIDs );
    };
  }
}

#endif // INDRI_QUERYENVIRONMENT_HPP

