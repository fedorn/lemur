/*==========================================================================
  Copyright (c) 2004 University of Massachusetts.  All Rights Reserved.

  Use of the Lemur Toolkit for Language Modeling and Information Retrieval
  is subject to the terms of the software license set forth in the LICENSE
  file included with this software, and also available at
  http://www.cs.cmu.edu/~lemur/license.html 
  as well as the conditions below.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  1. Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in
  the documentation and/or other materials provided with the
  distribution.

  3. The names "Indri", "Center for Intelligent Information Retrieval", 
  "CIIR", and "University of Massachusetts" must not be used to
  endorse or promote products derived from this software without
  prior written permission. To obtain permission, contact
  indri-info@ciir.cs.umass.edu.

  4. Products derived from this software may not be called "Indri" nor 
  may "Indri" appear in their names without prior written permission of 
  the University of Massachusetts. To obtain permission, contact 
  indri-info@ciir.cs.umass.edu.

  THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY OF MASSACHUSETTS AND OTHER
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
  BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
  FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
  THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
  DAMAGE.
  ==========================================================================
*/


//
// NetworkServerStub
//
// 21 May 2004 -- tds
//

#include "indri/NetworkServerStub.hpp"
#include "indri/XMLNode.hpp"
#include "indri/NetworkMessageStream.hpp"
#include "indri/QueryServer.hpp"
#include "indri/QueryResponsePacker.hpp"
#include "indri/ParsedDocument.hpp"
#include "Exception.hpp"

NetworkServerStub::NetworkServerStub( QueryServer* server, NetworkMessageStream* stream ) :
  _server(server),
  _stream(stream)
{
}

void NetworkServerStub::_handleQuery( XMLNode* request ) {
  indri::lang::Unpacker unpacker(request);
  std::vector<indri::lang::Node*> nodes = unpacker.unpack();
  int resultsRequested = (int) string_to_i64( request->getAttribute( "resultsRequested" ) );
  bool optimize = request->getAttribute("optimize") == "1";

  QueryServerResponse* response = _server->runQuery( nodes, resultsRequested, optimize );
  InferenceNetwork::MAllResults results = response->getResults();
  delete response;

  QueryResponsePacker packer( results );
  packer.write( _stream );
}

void NetworkServerStub::_handleDocuments( XMLNode* request ) {
  // rip out all the docIDs:
  std::vector<int> documentIDs;
  for( size_t i=0; i<request->getChildren().size(); i++ ) {
    documentIDs.push_back( (int) string_to_i64( request->getChildren()[i]->getValue() ) );
  }

  // get the documents
  QueryServerDocumentsResponse* docResponse = _server->documents( documentIDs );
  std::vector<ParsedDocument*> documents = docResponse->getResults();
  delete docResponse;

  // send them back
  XMLNode* response = new XMLNode( "documents" );
  for( unsigned int i=0; i<documents.size(); i++ ) {
    XMLNode* docNode = new XMLNode( "document" );
    ParsedDocument* document = documents[i];

    XMLNode* metadata = 0; 
    XMLNode* textNode = 0;
    XMLNode* positions = 0;

    if( documents[i]->metadata.size() ) {
      metadata = new XMLNode( "metadata" );

      for( size_t j=0; j<document->metadata.size(); j++ ) {
        XMLNode* keyNode = new XMLNode( "key", document->metadata[j].key );
        std::string value = base64_encode( document->metadata[j].value, document->metadata[j].valueLength );
        XMLNode* valNode = new XMLNode( "value", value );

        XMLNode* datum = new XMLNode( "datum" );
        datum->addChild( keyNode );
        datum->addChild( valNode );

        metadata->addChild( datum );
      }
    }

    if( documents[i]->text ) {
      std::string text = base64_encode( documents[i]->text, documents[i]->textLength );
      textNode = new XMLNode( "text", text );
    }

    if( documents[i]->positions.size() ) {
      positions = new XMLNode( "positions" );

      for( size_t j=0; j<document->positions.size(); j++ ) {
        XMLNode* position = new XMLNode( "position" );
        XMLNode* begin = new XMLNode( "begin", i64_to_string( document->positions[j].begin ) );
        XMLNode* end = new XMLNode( "end", i64_to_string( document->positions[j].end ) );
        
        position->addChild( begin );
        position->addChild( end );

        positions->addChild( position );
      }
    }

    if( metadata )
      docNode->addChild( metadata );

    if( textNode )
      docNode->addChild( textNode );

    if( positions )
      docNode->addChild( positions );

    response->addChild( docNode );
  }

  _stream->reply( response );
  _stream->replyDone();
  delete response;
}

void NetworkServerStub::_handleDocumentMetadata( XMLNode* request ) {
  std::vector<int> documentIDs;
  std::string fieldAttributeName = "field";
  std::string field = request->getChild( fieldAttributeName )->getValue();

  const XMLNode* documents = request->getChild("documents");

  for( size_t i=0; i<documents->getChildren().size(); i++ ) {
    documentIDs.push_back( (int) string_to_i64( documents->getChildren()[i]->getValue() ) );
  }

  // get the documents
  QueryServerMetadataResponse* metadataResponse = _server->documentMetadata( documentIDs, field );
  std::vector<std::string> metadata = metadataResponse->getResults();
  delete metadataResponse;

  // send them back
  XMLNode* response = new XMLNode( "document-metadata" );
  for( unsigned int i=0; i<metadata.size(); i++ ) {
    std::string value = base64_encode( metadata[i].c_str(), metadata[i].length() );
    XMLNode* datum = new XMLNode( "datum", value );
    response->addChild(datum);
  }

  _stream->reply( response );
  _stream->replyDone();
  delete response;
}

void NetworkServerStub::_handleDocumentVectors( XMLNode* request ) {
  const std::vector<XMLNode*>& children = request->getChildren();
  XMLNode* response = new XMLNode( "document-vector" );

  // convert doc IDs into an array
  std::vector<int> documentIDs;
  for( size_t i=0; i<request->getChildren().size(); i++ ) {
    documentIDs.push_back( (int) string_to_i64( request->getChildren()[i]->getValue() ) );
  }

  // get the document vectors from the index
  QueryServerVectorsResponse* vectorsResponse = _server->documentVectors( documentIDs );

  for( size_t i=0; i<vectorsResponse->getResults().size(); i++ ) {
    DocumentVector* docVector = vectorsResponse->getResults()[i];

    XMLNode* docResponse = new XMLNode( "document" );
    XMLNode* stems = new XMLNode( "stems" );
    XMLNode* positions = new XMLNode( "positions" );
    XMLNode* fields = new XMLNode( "fields" );

    const std::vector<std::string>& stemsVector = docVector->stems();

    for( unsigned int i=0; i<stemsVector.size(); i++ ) {
      const std::string& stem = stemsVector[i];
      std::string encoded = base64_encode( stem.c_str(), stem.length() );
      stems->addChild( new XMLNode( "stem", encoded ) );
    }

    const std::vector<int>& positionsVector = docVector->positions();

    for( unsigned int i=0; i<docVector->positions().size(); i++ ) {
      int position = positionsVector[i];
      positions->addChild( new XMLNode( "position", i64_to_string(position) ) );
    }

    for( unsigned int i=0; i<docVector->fields().size(); i++ ) {
      XMLNode* field = new XMLNode( "field" );

      std::string number = i64_to_string( docVector->fields()[i].number );
      std::string begin = i64_to_string( docVector->fields()[i].begin );
      std::string end = i64_to_string( docVector->fields()[i].end );

      field->addChild( new XMLNode( "name", docVector->fields()[i].name ) );
      field->addChild( new XMLNode( "number", number ) );
      field->addChild( new XMLNode( "begin", begin ) );
      field->addChild( new XMLNode( "end", end ) );

      fields->addChild( field );
    }

    docResponse->addChild(stems);
    docResponse->addChild(positions);
    docResponse->addChild(fields);

    response->addChild( docResponse );

    delete docVector;
  }

  _stream->reply( response );
  _stream->replyDone();

  delete response;
  delete vectorsResponse;
}

void NetworkServerStub::_handleTermCount( XMLNode* request ) {
  INT64 termCount = _server->termCount();
  XMLNode* response = new XMLNode( "term-count", i64_to_string(termCount) );
  _stream->reply( response );
  _stream->replyDone();
  delete response;
}

void NetworkServerStub::_handleStemCountText( XMLNode* request ) {
  INT64 termCount = _server->stemCount( request->getValue().c_str() );
  XMLNode* response = new XMLNode( "stem-count-text", i64_to_string(termCount) );
  _stream->reply( response );
  _stream->replyDone();
  delete response;
}

void NetworkServerStub::_handleTermCountText( XMLNode* request ) {
  INT64 termCount = _server->termCount( request->getValue().c_str() );
  XMLNode* response = new XMLNode( "term-count-text", i64_to_string(termCount) );
  _stream->reply( response );
  _stream->replyDone();
  delete response;
}

void NetworkServerStub::_handleTermCountID( XMLNode* request ) {
  INT64 termCount = _server->termCount( string_to_int(request->getValue()) );
  XMLNode* response = new XMLNode( "term-count-id", i64_to_string(termCount) );
  _stream->reply( response );
  _stream->replyDone();
  delete response;
}

void NetworkServerStub::_handleTermName( XMLNode* request ) {
  std::string name = _server->termName( string_to_int( request->getValue() ) );
  XMLNode* response = new XMLNode( "term-name", name );
  _stream->reply( response );
  _stream->replyDone();
  delete response;
}

void NetworkServerStub::_handleTermID( XMLNode* request ) {
  int termID = _server->termID( request->getValue().c_str() );
  XMLNode* response = new XMLNode( "term-id", i64_to_string(termID) );
  _stream->reply( response );
  _stream->replyDone();
  delete response;
}

void NetworkServerStub::_handleTermFieldCount( XMLNode* request ) {
  const XMLNode* termNode;
  const XMLNode* fieldNode;
  INT64 count;

  fieldNode = request->getChild( "field" );
  const std::string& fieldName = request->getValue();

  if( (termNode = request->getChild( "term-id" )) != 0 ) {
    int termID = string_to_int( termNode->getValue() );
    count = _server->termFieldCount( termID, fieldName );
  } else {
    termNode = request->getChild( "term-text" );
    const std::string& termName = termNode->getValue();
    count = _server->termFieldCount( termName, fieldName );
  }

  XMLNode* response = new XMLNode( "term-field-count", i64_to_string(count) );
  _stream->reply( response );
  _stream->replyDone();
  delete response;
}

void NetworkServerStub::_handleStemFieldCount( XMLNode* request ) {
  const XMLNode* termNode;
  const XMLNode* fieldNode;
  INT64 count;

  fieldNode = request->getChild( "field" );
  const std::string& fieldName = request->getValue();

  termNode = request->getChild( "stem-text" );
  const std::string& termName = termNode->getValue();
  count = _server->stemFieldCount( termName, fieldName );

  XMLNode* response = new XMLNode( "term-field-count", i64_to_string(count) );
  _stream->reply( response );
  _stream->replyDone();
  delete response;
}

void NetworkServerStub::_handleFieldList( XMLNode* request ) {
  XMLNode* response = new XMLNode( "field-list" );
  std::vector<std::string> fieldList = _server->fieldList();

  for( size_t i=0; i<fieldList.size(); i++ ) {
    response->addChild( new XMLNode( "field", fieldList[i] ) );
  }

  _stream->reply( response );
  _stream->replyDone();
  delete response;
}

void NetworkServerStub::_handleDocumentLength( XMLNode* request ) {
  int documentID = string_to_int( request->getValue() );

  INT64 length = _server->documentLength( documentID );
  XMLNode* response = new XMLNode( "document-length", i64_to_string( length ) );

  _stream->reply( response );
  _stream->replyDone();
  delete response;
}

void NetworkServerStub::_handleDocumentCount( XMLNode* request ) {
  INT64 count = _server->documentCount();
  XMLNode* response = new XMLNode( "document-count", i64_to_string( count ) );

  _stream->reply( response );
  _stream->replyDone();
  delete response;
}

void NetworkServerStub::request( XMLNode* input ) {
  try {
    const std::string& type = input->getName();

    if( type == "query" ) {
      _handleQuery( input );
    } else if( type == "documents" ) {
      _handleDocuments( input );
    } else if( type == "document-metadata" ) {
      _handleDocumentMetadata( input );
    } else if( type == "document-vectors" ) {
      _handleDocumentVectors( input );
    } else if( type == "term-field-count" ) {
      _handleTermFieldCount( input );
    } else if( type == "stem-field-count" ) {
      _handleStemFieldCount( input );
    } else if( type == "term-id" ) {
      _handleTermID( input );
    } else if( type == "term-name" ) {
      _handleTermName( input );
    } else if( type == "term-count" ) {
      _handleTermCount( input );
    } else if( type == "term-count-id" ) {
      _handleTermCountID( input );
    } else if( type == "term-count-text" ) {
      _handleTermCountText( input );
    } else if( type == "stem-count-text" ) {
      _handleStemCountText( input );
    } else if( type == "field-list" ) {
      _handleFieldList( input );
    } else if( type == "document-length" ) {
      _handleDocumentLength( input );
    } else if( type == "document-count" ) {
      _handleDocumentCount( input );
    } else {
      _stream->error( std::string() + "Unknown XML message type: " + input->getName() );
    }
  } catch( Exception& e ) {
    _stream->error( e.what() );
  } catch( ... ) {
    _stream->error( "Caught unknown exception while processing request" );
  }
}

void NetworkServerStub::reply( XMLNode* input ) {
  assert( false && "Shouldn't ever get a reply on the server" );
}

void NetworkServerStub::reply( const std::string& name, const void* buffer, unsigned int length ) {
  assert( false && "Shouldn't ever get a reply on the server" );
}

void NetworkServerStub::replyDone() {
  assert( false && "Shouldn't ever get a reply on the server" );
}

void NetworkServerStub::error( const std::string& error ) {
  // TODO: fix this to trap the error and propagate up the chain on the next request.
  std::cout << "Caught error message from client: " << error.c_str() << std::endl;
}

void NetworkServerStub::run() {
  while( _stream->alive() )
    _stream->read(*this);
}
