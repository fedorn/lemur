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
// NetworkServerStub
//
// 23 March 2004 -- tds
//

#ifndef INDRI_NETWORKSERVERSTUB_HPP
#define INDRI_NETWORKSERVERSTUB_HPP

#include "indri/XMLNode.hpp"
#include "indri/NetworkMessageStream.hpp"
#include "indri/QueryServer.hpp"
#include "indri/QueryResponsePacker.hpp"

class NetworkServerStub : public MessageStreamHandler {
private:
  QueryServer* _server;
  NetworkMessageStream* _stream;

  void _handleDocuments( XMLNode* input );
  void _handleDocumentMetadata( XMLNode* request );
  void _handleDocumentVectors( XMLNode* request );

  void _handleQuery( XMLNode* input );

  void _handleTermCount( XMLNode* request );
  void _handleStemCountText( XMLNode* request );
  void _handleTermCountText( XMLNode* request );
  void _handleTermCountID( XMLNode* request );

  void _handleTermName( XMLNode* request );
  void _handleTermID( XMLNode* request );

  void _handleTermFieldCount( XMLNode* request );
  void _handleStemFieldCount( XMLNode* request );
  void _handleDocumentCount( XMLNode* request );
  void _handleDocumentTermCount( XMLNode* request );
  void _handleDocumentLength( XMLNode* request );
  void _handleFieldList( XMLNode* request );

public:
  NetworkServerStub( QueryServer* server, NetworkMessageStream* stream );
  void request( XMLNode* input );
  void reply( XMLNode* input );
  void reply( const std::string& name, const void* buffer, unsigned int length );
  void replyDone();
  void error( const std::string& error );
  void run();
};

#endif // INDRI_NETWORKSERVERSTUB_HPP
