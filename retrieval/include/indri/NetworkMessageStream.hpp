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
// NetworkMessageStream
//
// 23 March 2004 -- tds
//

#ifndef INDRI_NETWORKMESSAGESTREAM_HPP
#define INDRI_NETWORKMESSAGESTREAM_HPP

#include "indri/NetworkStream.hpp"
#include "indri/Buffer.hpp"

class MessageStreamHandler {
public:
  virtual void request( class XMLNode* node ) = 0;
  virtual void reply( class XMLNode* node ) = 0;
  virtual void reply( const std::string& name, const void* buffer, unsigned int length ) = 0;
  virtual void replyDone() = 0;
  virtual void error( const std::string& e ) = 0;
};

class XMLReplyReceiver : public MessageStreamHandler {
private:
  bool _done;
  std::string _exception;
  class XMLNode* _reply;

public:
  XMLReplyReceiver() : _done(false), _reply(0) {}
  ~XMLReplyReceiver();

  void request( class XMLNode* node ) {
    _exception = "XMLReplyReceiver: doesn't accept requests";
    _done = true;
  }

  void reply( class XMLNode* reply ) {
    _reply = reply;
  }

  void reply( const std::string& name, const void* buffer, unsigned int length ) {
    _exception = "XMLReplyReceiver: should only get xml replies";
    _done = true;
  }

  void replyDone() {
    _done = true;
  }

  void error( const std::string& e ) {
    _exception = e;
    _done = true;
  }

  class XMLNode* getReply() {
    return _reply;
  }

  bool done() {
    return _done;
  }

  void wait( class NetworkMessageStream* stream );
};

class NetworkMessageStream {
private:
  Buffer _buffer;
  NetworkStream* _stream;
  int _readPosition;
  int _writePosition;

  int _findEOL();
  void _cleanBuffer();
  int _bufferLength();

public:
  NetworkMessageStream( NetworkStream* stream );
  void read( MessageStreamHandler& handler );
  void request( class XMLNode* messageNode );
  void reply( class XMLNode* replyNode );
  void reply( const std::string& name, const void* buffer, unsigned int size );
  void replyDone();
  void error( const std::string& errorMessage );
  bool alive();
};

#endif // INDRI_NETWORKMESSAGESTREAM_HPP
