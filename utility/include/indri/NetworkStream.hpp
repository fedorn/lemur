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
// NetworkStream
//
// 23 March 2004 -- tds
//

#ifndef INDRI_NETWORKSTREAM_HPP
#define INDRI_NETWORKSTREAM_HPP

#include "lemur-platform.h"
#include "lemur-compat.hpp"
#include <string>

class NetworkStream {
private:
  socket_t _socket;

  struct sockaddr_in _getSockaddr( const char* name, unsigned int port ) {
    long address;

    if( name && isdigit(name[0]) ) {
      address = inet_addr(name);
    } else {
      hostent* host = gethostbyname(name);
      
      if( host && host->h_length ) {
        address = *( (long*) host->h_addr );
      }
    }

    struct sockaddr_in sa;

    sa.sin_addr.s_addr = address;
    sa.sin_port = htons(port);
    sa.sin_family = AF_INET;
    memset(&sa.sin_zero, 0, sizeof sa.sin_zero );

    return sa;
  }

public:
  NetworkStream() : _socket(-1) {}
  NetworkStream( socket_t s ) : _socket(s) {}
  ~NetworkStream() {
    close();
  }

  bool connect( const std::string& name, unsigned int port ) {
    return connect(name.c_str(), port);
  }

  bool connect( const char* name, unsigned int port ) {
    lemur_compat::initializeNetwork();

    _socket = ::socket( AF_INET, SOCK_STREAM, 0 );
    struct sockaddr_in sa = _getSockaddr( name, port );
    int result = ::connect( _socket, (const sockaddr*) &sa, sizeof sa );

    if( result ) {
      close();
    }

    return !result;
  }

  void close() {
    lemur_compat::closesocket( _socket );
    _socket = -1;
  }

  int write( const void* buffer, size_t length ) {
    return ::send( _socket, (const char*) buffer, int(length), 0 );
  }

  int read( void* buffer, size_t length ) {
    return ::recv( _socket, (char*) buffer, int(length), 0 );
  }

  int blockingRead( void* buffer, size_t length ) {
    size_t bytesRead = 0;
    size_t chunkRead = 0;
    int result;
    
    while( bytesRead < (int) length ) {
      chunkRead = length - bytesRead;
      result = read( (char*)buffer + bytesRead, chunkRead );

      if( result <= 0 ) {
        close();
        return int(bytesRead);
      }

      bytesRead += result;
    }

    return int(bytesRead);
  }

  int blockingWrite( const void* buffer, unsigned int length ) {
    size_t bytesWritten = 0;

    while( bytesWritten < (int) length ) {
      size_t chunkWrite = length - bytesWritten;
      int result = write( (const char*)buffer + bytesWritten, chunkWrite );

      if( result <= 0 ) {
        close();
        return int(bytesWritten);
      }

      bytesWritten += result;
    }

    return int(bytesWritten);
  }

  bool alive() {
    return _socket != -1;
  }
};

#endif // INDRI_NETWORKSTREAM_HPP
