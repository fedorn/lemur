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
// SequentialWriteBuffer
//
// 15 November 2004 -- tds
//

#ifndef INDRI_SEQUENTIALWRITEBUFFER_HPP
#define INDRI_SEQUENTIALWRITEBUFFER_HPP

#include "indri/File.hpp"
#include "indri/InternalFileBuffer.hpp"
namespace indri
{
  namespace file
  {
    
    class SequentialWriteBuffer {
    private:
      File& _file;
      InternalFileBuffer _current;
      UINT64 _position;
      UINT64 _eof;

    public:
      // Note: we assume that the file is empty
      SequentialWriteBuffer( File& file, size_t length ) :
        _file(file),
        _current(length),
        _position(0),
        _eof(0)
      {
      }

      void seek( UINT64 position ) {
        _position = position;
      }
  
      char* write( size_t length ) {
        UINT64 endBuffer = _current.filePosition + _current.buffer.size();
        UINT64 endBufferData = _current.filePosition + _current.buffer.position();
        UINT64 endWrite = length + _position;
        UINT64 startWrite = _position;
        UINT64 startBuffer = _current.filePosition;
        char* writeSpot;

        if( startBuffer > startWrite || endBuffer < endWrite || (endBufferData < _eof && startWrite > endBufferData) ) {
          flush();
          _current.filePosition = _position;

          startBuffer = _current.filePosition;
          endBuffer = _current.filePosition + _current.buffer.size();
          endBufferData = _current.filePosition + _current.buffer.position();
        }

        if( endWrite > endBufferData ) {
          // need to move the buffer pointer to the end, potentially resizing buffer
          _current.buffer.write( endWrite - endBufferData );
          endBufferData = _current.filePosition + _current.buffer.position();
        }

        assert( endWrite <= endBufferData && startWrite >= startBuffer );
        writeSpot = _current.buffer.front() + (_position - _current.filePosition);
        assert( writeSpot + length <= _current.buffer.front() + _current.buffer.position() );
        _position += length;

        return writeSpot;
      }

      void write( const void* buffer, size_t length ) {
        memcpy( write( length ), buffer, length );
      }
  
      void unwrite( size_t length ) {
        assert( length <= _current.buffer.position() );
        _current.buffer.unwrite( length );
        _position -= length;
      }
  
      UINT64 tell() const {
        return _position;
      }

      void flush() {
        size_t bytes = _current.buffer.position();
        _file.write( _current.buffer.front(), _current.filePosition, _current.buffer.position() );
        _current.buffer.clear();
        _current.filePosition += bytes;
        _eof = lemur_compat::max( _current.filePosition, _eof );
      }

      void flushRegion( UINT64 start, UINT64 length ) {
        if( (start+length) >= _current.filePosition &&
            start <= _current.filePosition + _current.buffer.position() )
        {
          flush();
        }
      }
    };
  }
}

#endif // INDRI_SEQUENTIALWRITEBUFFER_HPP

