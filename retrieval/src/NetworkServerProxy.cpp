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
// 20 May 2004 -- tds
//

#include "indri/NetworkServerProxy.hpp"
#include "indri/ParsedDocument.hpp"
#include <iostream>

//
// NetworkServerProxyResponse
//

class NetworkServerProxyResponse : public QueryServerResponse {
private:
  QueryResponseUnpacker _unpacker;

public:
  NetworkServerProxyResponse( NetworkMessageStream* stream ) :
    _unpacker( stream )
  {
  }

  InferenceNetwork::MAllResults& getResults() {
    return _unpacker.getResults();
  }
};

//
// NetworkServerProxyDocumentsResponse
//

class NetworkServerProxyDocumentsResponse : public QueryServerDocumentsResponse {
private:
  NetworkMessageStream* _stream;
  std::vector<ParsedDocument*> _documents;

public:
  NetworkServerProxyDocumentsResponse( NetworkMessageStream* stream ) :
    _stream(stream)
  {
  }

  // caller deletes the ParsedDocuments
  std::vector<ParsedDocument*>& getResults() {
    XMLReplyReceiver r;
    r.wait(_stream);

    if( r.getReply() ) {
      XMLNode* reply = r.getReply();
      size_t numChildren = reply->getChildren().size();

      for( size_t i=0; i<numChildren; i++ ) {
        Buffer buffer;
        int textOffset = 0;
        greedy_vector<int> metadataKeyOffset;
        greedy_vector<int> metadataValueOffset;

        const XMLNode* child = reply->getChildren()[i];
        const XMLNode* metadata = child->getChild( "metadata" );

        // allocate room for the ParsedDocument
        buffer.write( sizeof(ParsedDocument) );

        if( metadata ) {
          for( size_t j=0; j<metadata->getChildren().size(); j++ ) {
            std::string key = metadata->getChildren()[j]->getChildValue("key");
            std::string value = metadata->getChildren()[j]->getChildValue("value");           
            
            metadataKeyOffset.push_back( buffer.position() );
            strcpy( buffer.write( key.size()+1 ), key.c_str() );

            metadataValueOffset.push_back( buffer.position() );
            int length = base64_decode( buffer.write( value.size() ), value.size(), value );
            buffer.unwrite( value.size()-length );
          }
        }

        const XMLNode* textNode = child->getChild("text");
        std::string text;

        if( textNode ) {
          text = textNode->getValue();
          textOffset = buffer.position();
          int length = base64_decode( buffer.write( text.size() ), text.size(), text );
          buffer.unwrite( text.size()-length );
        }

        // now all of our data is in the buffer, so we can allocate a return structure
        new(buffer.front()) ParsedDocument;
        ParsedDocument* parsedDocument = (ParsedDocument*) buffer.front();

        const XMLNode* positions = child->getChild( "positions" );

        if( positions ) {
          const std::vector<XMLNode*>& children = positions->getChildren();
          
 
          for( size_t j=0; j<children.size(); j++ ) {
            TermExtent extent;
            std::string begin = children[j]->getChildValue("begin");
            std::string end = children[j]->getChildValue("end");    

            extent.begin = (int) string_to_i64( begin );
            extent.end = (int) string_to_i64( end );

            parsedDocument->positions.push_back( extent );
          }
        }

        for( size_t j=0; j<metadataKeyOffset.size(); j++ ) {
          MetadataPair pair;

          pair.key = buffer.front() + metadataKeyOffset[j];
          pair.value = buffer.front() + metadataValueOffset[j];

          if( metadataKeyOffset.size() > j+1 )
            pair.valueLength = metadataKeyOffset[j+1] - metadataValueOffset[j];
          else
            pair.valueLength = textOffset;

          parsedDocument->metadata.push_back( pair );
        }

        parsedDocument->text = buffer.front() + textOffset;
        parsedDocument->textLength = buffer.position() - textOffset;
        buffer.detach();

        _documents.push_back( parsedDocument );
      }
    }
  
    return _documents;
  }
};

//
// NetworkServerProxyMetadataResponse
//

class NetworkServerProxyMetadataResponse : public QueryServerMetadataResponse {
private:
  std::vector<std::string> _metadata;
  NetworkMessageStream* _stream;

public:
  NetworkServerProxyMetadataResponse( NetworkMessageStream* stream ) :
    _stream(stream)
  {
  }

  std::vector<std::string>& getResults() {
    XMLReplyReceiver r;
    r.wait(_stream);

    // parse the result
    XMLNode* reply = r.getReply();
    Buffer metadataBuffer;

    for( unsigned int i=0; i<reply->getChildren().size(); i++ ) {
      const XMLNode* meta = reply->getChildren()[i];
      const std::string& input = meta->getValue();

      std::string value;
      base64_decode_string( value, input );
      _metadata.push_back(value);

      metadataBuffer.clear();
    }

    return _metadata;
  }
};

//
// NetworkServerProxyVectorsResponse
//

class NetworkServerProxyVectorsResponse : public QueryServerVectorsResponse {
public:
  std::vector<DocumentVector*> _vectors;
  NetworkMessageStream* _stream;
  bool _readResponse;

public:
  NetworkServerProxyVectorsResponse( NetworkMessageStream* stream ) 
    :
    _stream(stream),
    _readResponse(false)
  {
  }

  std::vector<DocumentVector*>& getResults() {
    if( !_readResponse ) {
      XMLReplyReceiver r;
      r.wait( _stream );

      const XMLNode* reply = r.getReply();
      const std::vector<XMLNode*>& children = reply->getChildren();

      for( size_t i=0; i<children.size(); i++ ) {
        const XMLNode* stems = children[i]->getChild("stems");
        const XMLNode* positions = children[i]->getChild("positions");
        const XMLNode* fields = children[i]->getChild("fields");

        DocumentVector* result = new DocumentVector;

        for( unsigned int i=0; i<stems->getChildren().size(); i++ ) {
          // have to use base64 coding, in case the stem contains '<', '>', etc.
          std::string stem;
          base64_decode_string(stem, stems->getChildren()[i]->getValue());
          result->stems().push_back( stem );
        }

        std::vector<int>& positionsVector = result->positions();

        for( unsigned int i=0; i<positions->getChildren().size(); i++ ) {
          const std::string& stringText = positions->getChildren()[i]->getValue();
          INT64 position = string_to_i64( stringText );
          positionsVector.push_back( int(position) );
        }

        std::vector<DocumentVector::Field>& fieldVector = result->fields();

        for( unsigned int i=0; i<fields->getChildren().size(); i++ ) {
          const XMLNode* field = fields->getChildren()[i];

          const XMLNode* nameField = field->getChild("name");
          const XMLNode* numberField = field->getChild("number");
          const XMLNode* beginField = field->getChild("begin");
          const XMLNode* endField = field->getChild("end");

          DocumentVector::Field f;

          f.name = nameField->getValue();
          f.number = string_to_i64( numberField->getValue() );
          f.begin = int(string_to_i64( beginField->getValue() ));
          f.end = int(string_to_i64( endField->getValue() ));

          fieldVector.push_back(f);
        }

        _vectors.push_back(result);
      }

      _readResponse = true;
    }

    return _vectors;
  }
};

//
// NetworkServerProxy code
//


NetworkServerProxy::NetworkServerProxy( NetworkMessageStream* stream ) :
  _stream(stream)
{
}

QueryServerResponse* NetworkServerProxy::runQuery( std::vector<indri::lang::Node*>& roots, int resultsRequested, bool optimize ) {
  indri::lang::Packer packer;

  for( unsigned int i=0; i<roots.size(); i++ ) {
    packer.pack( roots[i] );
  }

  XMLNode* query = packer.xml();
  query->addAttribute( "resultsRequested", i64_to_string(resultsRequested) );
  query->addAttribute( "optimize", optimize ? "1" : "0" );
  _stream->request( query );

  return new NetworkServerProxyResponse( _stream );
}

QueryServerMetadataResponse* NetworkServerProxy::documentMetadata( const std::vector<int>& documentIDs, const std::string& attributeName ) {
  XMLNode* request = new XMLNode( "document-metadata" );
  XMLNode* field = new XMLNode( "field", attributeName );
  XMLNode* documents = new XMLNode( "documents" );

  // build request
  for( unsigned int i=0; i<documentIDs.size(); i++ ) {
    documents->addChild( new XMLNode( "document", i64_to_string( documentIDs[i] ) ) );
  }
  request->addChild( field );
  request->addChild( documents );

  // send request
  _stream->request( request );
  delete request;

  return new NetworkServerProxyMetadataResponse( _stream );
}

QueryServerDocumentsResponse* NetworkServerProxy::documents( const std::vector<int>& documentIDs ) {
  XMLNode* docRequest = new XMLNode( "documents" );

  for( unsigned int i=0; i<documentIDs.size(); i++ ) {
    docRequest->addChild( new XMLNode("doc", i64_to_string(documentIDs[i])) );
  }
  _stream->request( docRequest );
  delete docRequest;

  return new NetworkServerProxyDocumentsResponse( _stream );
}

INT64 NetworkServerProxy::termCount() {
  std::auto_ptr<XMLNode> request( new XMLNode( "term-count" ) );
  _stream->request( request.get() );

  XMLReplyReceiver r;
  r.wait( _stream );

  XMLNode* reply = r.getReply();
  return string_to_i64( reply->getValue() );
}

INT64 NetworkServerProxy::termCount( const std::string& term ) {
  std::auto_ptr<XMLNode> request( new XMLNode( "term-count-text", term ) );
  _stream->request( request.get() );

  XMLReplyReceiver r;
  r.wait( _stream );

  XMLNode* reply = r.getReply();
  return string_to_i64( reply->getValue() );
}

INT64 NetworkServerProxy::stemCount( const std::string& term ) {
  std::auto_ptr<XMLNode> request( new XMLNode( "stem-count-text", term ) );
  _stream->request( request.get() );

  XMLReplyReceiver r;
  r.wait( _stream );

  XMLNode* reply = r.getReply();
  return string_to_i64( reply->getValue() );
}

INT64 NetworkServerProxy::termCount( int term ) {
  std::auto_ptr<XMLNode> request( new XMLNode( "term-count-id", i64_to_string(term) ) );
  _stream->request( request.get() );

  XMLReplyReceiver r;
  r.wait( _stream );

  XMLNode* reply = r.getReply();
  return string_to_i64( reply->getValue() );
}

std::string NetworkServerProxy::termName( int term ) {
  std::auto_ptr<XMLNode> request( new XMLNode( "term-name", i64_to_string(term) ) );
  _stream->request( request.get() );

  XMLReplyReceiver r;
  r.wait( _stream );

  XMLNode* reply = r.getReply();
  return reply->getValue();
}

int NetworkServerProxy::termID( const std::string& term ) {
  std::auto_ptr<XMLNode> request( new XMLNode( "term-id", term ) );
  _stream->request( request.get() );

  XMLReplyReceiver r;
  r.wait( _stream );

  XMLNode* reply = r.getReply();
  return string_to_int(reply->getValue());
}

INT64 NetworkServerProxy::termFieldCount( int term, const std::string& field ) {
  std::auto_ptr<XMLNode> request( new XMLNode( "term-field-count" ) );
  XMLNode* termNode = new XMLNode( "term-id", i64_to_string(term) );
  XMLNode* fieldNode = new XMLNode( "field", field );
  request->addChild( termNode );
  request->addChild( fieldNode );
  _stream->request( request.get() );
  
  XMLReplyReceiver r;
  r.wait( _stream );

  XMLNode* reply = r.getReply();
  return string_to_i64(reply->getValue());
}

INT64 NetworkServerProxy::termFieldCount( const std::string& term, const std::string& field ) {
  std::auto_ptr<XMLNode> request( new XMLNode( "term-field-count" ) );
  XMLNode* termNode = new XMLNode( "term-text", term );
  XMLNode* fieldNode = new XMLNode( "field", field );
  request->addChild( termNode );
  request->addChild( fieldNode );
  _stream->request( request.get() );

  XMLReplyReceiver r;
  r.wait( _stream );

  XMLNode* reply = r.getReply();
  return string_to_i64(reply->getValue());
}

INT64 NetworkServerProxy::stemFieldCount( const std::string& stem, const std::string& field ) {
  std::auto_ptr<XMLNode> request( new XMLNode( "stem-field-count" ) );
  XMLNode* stemNode = new XMLNode( "stem-text", stem );
  XMLNode* fieldNode = new XMLNode( "field", field );
  request->addChild( stemNode );
  request->addChild( fieldNode );
  _stream->request( request.get() );

  XMLReplyReceiver r;
  r.wait( _stream );

  XMLNode* reply = r.getReply();
  return string_to_i64(reply->getValue());
}

std::vector<std::string> NetworkServerProxy::fieldList() {
  std::auto_ptr<XMLNode> request( new XMLNode( "field-list" ) );
  _stream->request( request.get() );

  XMLReplyReceiver r;
  r.wait( _stream );

  XMLNode* reply = r.getReply();
  const std::vector<XMLNode*>& children = reply->getChildren();
  std::vector<std::string> result;

  for( size_t i=0; i<children.size(); i++ ) {
    result.push_back( children[i]->getValue() );
  }

  return result;
}

int NetworkServerProxy::documentLength( int documentID ) {
  XMLNode* request = new XMLNode( "document-length", i64_to_string(documentID) );
  _stream->request( request );
  delete request;

  XMLReplyReceiver r;
  r.wait( _stream );

  XMLNode* reply = r.getReply();
  return string_to_int(reply->getValue());
}

INT64 NetworkServerProxy::documentCount() {
  XMLNode* request = new XMLNode( "document-count" );
  _stream->request( request );
  delete request;

  XMLReplyReceiver r;
  r.wait( _stream );

  XMLNode* reply = r.getReply();
  return string_to_int(reply->getValue());
}

INT64 NetworkServerProxy::documentCount( const std::string& term ) {
  XMLNode* request = new XMLNode( "document-term-count" );
  _stream->request( request );
  delete request;

  XMLReplyReceiver r;
  r.wait( _stream );

  XMLNode* reply = r.getReply();
  return string_to_int(reply->getValue());
}

QueryServerVectorsResponse* NetworkServerProxy::documentVectors( const std::vector<int>& documentIDs ) {
  XMLNode* request = new XMLNode( "document-vectors" );

  for( size_t i=0; i<documentIDs.size(); i++ ) {
    request->addChild( new XMLNode( "document", i64_to_string(documentIDs[i]) ) );
  }

  _stream->request( request );
  delete request;

  return new NetworkServerProxyVectorsResponse( _stream );
}

