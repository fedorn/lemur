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

#include "indri/NetworkMessageStream.hpp"
#include "indri/XMLWriter.hpp"
#include "indri/XMLReader.hpp"
#include "Exception.hpp"
#include <iostream>

XMLReplyReceiver::~XMLReplyReceiver() {
  delete _reply;
}

void XMLReplyReceiver::wait( NetworkMessageStream* stream ) {
  while( !done() && stream->alive() && !_exception.size() ) {
    stream->read(*this);
  }
  
  if( _exception.size() )
    LEMUR_THROW( LEMUR_NETWORK_ERROR, _exception );
}

int NetworkMessageStream::_findEOL() {
  for( size_t i=_readPosition; i<_writePosition; i++ ) {
    if( _buffer.front()[i] == '\n' )
      return int(i);
  }

  return -1;
}

void NetworkMessageStream::_cleanBuffer() {
  _buffer.remove( _readPosition );
  _writePosition -= _readPosition;
  _readPosition = 0;
}

int NetworkMessageStream::_bufferLength() {
  return _writePosition - _readPosition;
}

NetworkMessageStream::NetworkMessageStream( NetworkStream* stream ) :
  _stream(stream)
{
  _readPosition = 0;
  _writePosition = 0;

  // give the buffer some space so we don't thrash around
  // allocating in little chunks
  _buffer.grow( 65536 ); 
}

bool NetworkMessageStream::alive() {
  return _stream->alive();
}

void NetworkMessageStream::read( MessageStreamHandler& handler ) {
  int endOfLine = _findEOL();
  int bytesRead = -1;

  while( endOfLine < 0 && bytesRead != 0 ) {
    _cleanBuffer();
    int bytesRead = _stream->read( _buffer.front() + _writePosition, _buffer.size() - _writePosition );
    
    if( bytesRead <= 0 ) {
      bytesRead = 0;
      break;
    }

    _buffer.write( bytesRead );
    _writePosition += bytesRead;
    endOfLine = _findEOL();
  }

  if( endOfLine < 0 ) {
    // broke out of loop because we didn't receive a command, but
    // the socket returned 0 (other side closed connection)
    _stream->close();
    return;
  }

  assert( _readPosition <= _writePosition );

  int length = endOfLine - _readPosition;
  if( length < 4 )
    LEMUR_THROW( LEMUR_NETWORK_ERROR, "Malformed network packet: header line less than 4 bytes" );

  if( strncmp( "XREQ", _buffer.front() + _readPosition, 4 ) == 0 ) {
    int length = strtol( _buffer.front() + _readPosition + 4, 0, 10 );
    _readPosition = endOfLine + 1;
    
    if( _bufferLength() < length ) {
      _cleanBuffer();
      _buffer.grow( length );
      int bytesRead = _stream->blockingRead( _buffer.front() + _writePosition, length - _bufferLength() );

      if( bytesRead <= 0 ) {
         _stream->close();
         return;
      }

      _buffer.write(bytesRead);
      _writePosition += bytesRead;
    }

    XMLReader reader;
    XMLNode* node = reader.read( _buffer.front() + _readPosition, length );
    handler.request( node );
    delete node;
    _readPosition += length;
  } else if( strncmp( "XRPY", _buffer.front() + _readPosition, 4 ) == 0 ) {
    int length = strtol( _buffer.front() + _readPosition + 4, 0, 10 );
    _readPosition = endOfLine + 1;
    
    if( _bufferLength() < length ) {
      _cleanBuffer();
      _buffer.grow( length );
      int bytesRead = _stream->blockingRead( _buffer.front() + _writePosition, length - _bufferLength() );
      if( bytesRead <= 0 ) {
        _stream->close();
        return;
      }
      
      _buffer.write(bytesRead);
      _writePosition += bytesRead;
    }

    XMLReader reader;
    XMLNode* node = reader.read( _buffer.front() + _readPosition, length );
    handler.reply( node );
    _readPosition += length;
  } else if( strncmp( "BRPY", _buffer.front() + _readPosition, 4 ) == 0 ) {
    char* next;
    int length = strtol( _buffer.front() + _readPosition + 4, &next, 10 );
    _readPosition = endOfLine + 1;

    std::string name;
    name.assign( next + 1, _buffer.front() + endOfLine );

    if( _bufferLength() < length ) {
      _cleanBuffer();
      _buffer.grow( length );
      int bytesRead = _stream->blockingRead( _buffer.front() + _writePosition, length - _bufferLength() );
      if( bytesRead <= 0 ) {
        _stream->close();
        return;
      }

      _buffer.write(bytesRead);
      _writePosition += bytesRead;
    }

    handler.reply( name, _buffer.front() + _readPosition, length );
    _readPosition += length;
  } else if( strncmp( "RFIN", _buffer.front() + _readPosition, 4 ) == 0 ) {
    _readPosition = endOfLine + 1;
    handler.replyDone();
  } else if( strncmp( "ERR", _buffer.front() + _readPosition, 3 ) == 0 ) {
    std::string errorString;
    errorString.assign( _buffer.front() + _readPosition + 4, _buffer.front() + endOfLine );
    handler.error( errorString );
    _readPosition = endOfLine + 1;
    // close on error
    _stream->close();
  } else {
    _readPosition = endOfLine + 1;
    // unrecognized command
    _stream->close();
  }

  assert( _readPosition <= _writePosition );
}

void NetworkMessageStream::request( XMLNode* messageNode ) {
  XMLWriter writer(messageNode);
  std::string body;
  writer.write( body );
  
  std::string header = "XREQ ";
  header += i64_to_string( body.size() );
  header += "\n";

  _stream->write( header.c_str(), header.length() );
  _stream->write( body.c_str(), body.length() );
}

void NetworkMessageStream::reply( XMLNode* replyNode ) {
  XMLWriter writer(replyNode);
  std::string body;
  writer.write(body);

  std::string header = "XRPY ";
  header += i64_to_string( body.size() );
  header += "\n";

  _stream->write( header.c_str(), header.size() );
  _stream->write( body.c_str(), body.length() );
}

void NetworkMessageStream::reply( const std::string& name, const void* buffer, unsigned int size ) {
  std::string header = "BRPY ";
  header += i64_to_string( size );
  header += " ";
  header += name;
  header += "\n";

  _stream->write( header.c_str(), header.length() );
  _stream->write( buffer, size );
}

void NetworkMessageStream::replyDone() {
  _stream->write( "RFIN\n", 5 );
}

void NetworkMessageStream::error( const std::string& errorMessage ) {
  std::string fullMessage;
  fullMessage = "ERR ";
  fullMessage += errorMessage;
  fullMessage += "\n";

  _stream->write( fullMessage.c_str(), fullMessage.length() );
}
