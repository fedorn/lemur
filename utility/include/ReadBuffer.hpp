/*==========================================================================
 * Copyright (c) 2003 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/


//
// ReadBuffer
//
// tds - 13 November 2003
//

#ifndef LEMUR_READBUFFER_HPP
#define LEMUR_READBUFFER_HPP

#include "File.hpp"

/// Provide a growable read buffer on a File object.
class ReadBuffer {
private:
  File::offset_type _filePosition;
  char* _buffer;
  File& _file;
  int _bufferSize;
  int _bufferPosition;
  int _bufferDataLength;
  bool _gValid;

public:
  /// wrap <file> in a ReadBuffer with initial
  /// buffer size <bufferSize>.  The buffer
  /// may grow if necessary to support large
  /// peek() and read() requests.
  ReadBuffer( File& file, int bufferSize );
  ~ReadBuffer();

  /// standard read() semantics; will perform
  /// an unbuffered read if the read size is
  /// large enough
  void read( char* data, int length );

  /// return a pointer to a buffer containing 
  /// the next <length> bytes, but do not advance
  /// the read pointer
  const char* peek( int length );

  /// return a pointer to a buffer containing 
  /// the next <length> bytes, and advance
  /// the read pointer
  const char* read( int length );

  /// move the internal read pointer
  /// to <position>, which may be relative to the
  /// beginning, end or current position of the file
  /// as specified by <direction>.
  void seekg( File::offset_type position, std::fstream::seekdir direction );

  /// return the read pointer location with
  /// reference to the beginning of the file
  File::offset_type tellg();

  /// return the current read state bits,
  /// eofbit, badbit, etc. (same
  /// semantics as std::ifstream::rdstate())
  int rdstate();

  /// marks the underlying stream pointer
  /// as invalid--call this if you seek
  /// the underlying file to a new location
  void invalidateg();
};

#endif // LEMUR_READBUFFER_HPP
