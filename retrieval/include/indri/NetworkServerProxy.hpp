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
// NetworkServerProxy
//
// 23 March 2004 -- tds
//

#ifndef INDRI_NETWORKSERVERPROXY_HPP
#define INDRI_NETWORKSERVERPROXY_HPP

#include "indri/XMLNode.hpp"
#include "indri/XMLWriter.hpp"
#include "indri/QueryResponseUnpacker.hpp"
#include "indri/QueryServer.hpp"
#include "indri/Packer.hpp"
#include "indri/NetworkMessageStream.hpp"
#include "indri/Buffer.hpp"

class NetworkServerProxy : public QueryServer {
private:
  NetworkMessageStream* _stream;

public:
  NetworkServerProxy( NetworkMessageStream* stream );

  QueryServerResponse* runQuery( std::vector<indri::lang::Node*>& roots, int resultsRequested, bool optimize );
  QueryServerDocumentsResponse* documents( const std::vector<int>& documentIDs );
  QueryServerMetadataResponse* documentMetadata( const std::vector<int>& documentIDs, const std::string& attributeName );

  // terms -- implemented (but not on stub)
  INT64 termCount();
  INT64 termCount( int term );
  INT64 termCount( const std::string& term );
  INT64 stemCount( const std::string& stem );
  std::string termName( int term );
  int termID( const std::string& term );

  // fields
  std::vector<std::string> fieldList(); // unimpl
  INT64 termFieldCount( int term, const std::string& field );
  INT64 termFieldCount( const std::string& term, const std::string& field );
  INT64 stemFieldCount( const std::string& stem, const std::string& field );

  // documents
  int documentLength( int documentID );
  INT64 documentCount();
  INT64 documentCount( const std::string& term );

  // document vector
  QueryServerVectorsResponse* documentVectors( const std::vector<int>& documentIDs );
};

#endif // INDRI_NETWORKSERVERPROXY_HPP
