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


