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
// WriteBuffer
//
// tds - 13 November 2003
//

#ifndef LEMUR_WRITEBUFFER_HPP
#define LEMUR_WRITEBUFFER_HPP

#include "File.hpp"
///Provide a write buffer for a File object, enabling explicit 
/// buffer management..
class WriteBuffer {
private:
  char* _buffer;
  int _bufferSize;
  File& _file;
  int _position;
  File::offset_type _filePos;

public:
  WriteBuffer( File& file, int bufferSize );
  ~WriteBuffer();
  
  /// standard write semantics; will
  /// perform an unbuffered write if
  /// <length> is long enough to warrant it
  void write( const char* data, int length );
  
  /// flushes the internal buffer out to 
  /// the ofstream. does not explicitly
  /// flush the ofstream.
  void flush();

  /// returns the current write pointer
  /// position
  File::offset_type tellp() const;
};

#endif // LEMUR_WRITEBUFFER_HPP
