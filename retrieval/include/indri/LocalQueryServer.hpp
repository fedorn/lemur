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
// LocalQueryServer
//
// 15 March 2004 -- tds
//
// Allows a QueryEnvironment to run queries against a
// local connection (without doing a network hop).
// This is especially useful for testing.
//

#ifndef INDRI_LOCALQUERYSERVER_HPP
#define INDRI_LOCALQUERYSERVER_HPP

#include "indri/QueryServer.hpp"
#include "indri/Collection.hpp"
#include "indri/Repository.hpp"
#include "indri/DocumentVector.hpp"
#include "indri/ListCache.hpp"

class LocalQueryServer : public QueryServer {
private:
  Repository& _repository;
  ListCache _cache;

public:
  LocalQueryServer( Repository& repository );

  // query
  QueryServerResponse* runQuery( std::vector<indri::lang::Node*>& roots, int resultsRequested, bool optimize );

  // single document queries
  ParsedDocument* document( int documentID );
  std::string documentMetadatum( int documentID, const std::string& attributeName );

  // batch queries
  QueryServerDocumentsResponse* documents( const std::vector<int>& documentIDs );
  QueryServerMetadataResponse* documentMetadata( const std::vector<int>& documentIDs, const std::string& attributeName );

  // terms
  INT64 termCount();
  INT64 termCount( int term );
  INT64 termCount( const std::string& term );
  INT64 stemCount( const std::string& stem );
  std::string termName( int term );
  int termID( const std::string& term );
  
  // fields
  std::vector<std::string> fieldList();
  INT64 termFieldCount( int term, const std::string& field );
  INT64 termFieldCount( const std::string& term, const std::string& field );
  INT64 stemFieldCount( const std::string& stem, const std::string& field );

  // documents
  int documentLength( int documentID );
  INT64 documentCount();
  INT64 documentCount( const std::string& term );

  // vector
  QueryServerVectorsResponse* documentVectors( const std::vector<int>& documentIDs );
};

#endif // INDRI_LOCALQUERYSERVER_HPP
