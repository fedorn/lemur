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
// QueryResponseUnpacker
//
// 23 March 2004 -- tds
//

#ifndef INDRI_QUERYRESPONSEUNPACKER_HPP
#define INDRI_QUERYRESPONSEUNPACKER_HPP

#include "indri/NetworkMessageStream.hpp"
#include "indri/InferenceNetwork.hpp"
#include "Exception.hpp"

class QueryResponseUnpacker : public MessageStreamHandler {
private:
  NetworkMessageStream* _stream;
  InferenceNetwork::MAllResults _results;
  std::string _exception;
  bool _done;

public:
  QueryResponseUnpacker( NetworkMessageStream* stream ) :
    _stream(stream),
    _done(false)
  {
  }

  InferenceNetwork::MAllResults& getResults() {
    while( !_done && _stream->alive() && !_exception.length() )
      _stream->read(*this);

    if( _exception.length() )
      LEMUR_THROW( LEMUR_RUNTIME_ERROR, _exception );
    
    return _results;
  }

  void reply( XMLNode* node ) {
    assert( false && "Query responses are binary only for now" );
  }

  void reply( const std::string& name, const void* buffer, unsigned int length ) {
    std::string nodeName;
    std::string listName;
    
    nodeName = name.substr( 0, name.find(':') );
    listName = name.substr( name.find(':')+1 );
    
    ScoredExtentResult aligned;
    int count = length / sizeof(ScoredExtentResult);
    std::vector<ScoredExtentResult>& resultVector = _results[nodeName][listName];

    for( int i=0; i<count; i++ ) {
      // copy for alignment
      memcpy( &aligned, (const char*)buffer + i*sizeof(ScoredExtentResult), sizeof(ScoredExtentResult) );

      aligned.begin = ntohl(aligned.begin);
      aligned.end = ntohl(aligned.end);
      aligned.document = ntohl(aligned.document);
      aligned.score = lemur_compat::ntohd(aligned.score);

      resultVector.push_back(aligned);
    }
  }

  void replyDone() {
    _done = true;
  }

  void request( XMLNode* node ) {
    assert( false && "No requests expected from the query server" );
  }

  void error( const std::string& err ) {
    _exception = err;
  }

};

#endif // INDRI_QUERYRESPONSEUNPACKER_HPP


