
//
// File
//
// 12 March 2004 -- tds
// (Copied everything out of header)
//

#include "File.hpp"

#include <vector>
#include <sstream>
#include <fstream>
#include <assert.h>
#include "lemur-platform.h"
#include "lemur-compat.hpp"
#include <iostream>

// This value doesn't have to actually be the page size of the hardware,
// but it may make some kernel operations a little faster if it is
// some multiple of the actual page size

#define FILE_PAGE_SIZE              (8192)

// The contortions here happen because some preprocessors don't
// handle 64-bit values well--we're trying to compute 2^32 - FILE_PAGE_SIZE
// without ever generating an intermediate quantity >= 2^32.

#define FILE_1GB                    (1024 * 1024 * 1024)
#define FILE_2GB_MINUS_1            (FILE_1GB + (FILE_1GB - 1))
#define FILE_MAXIMUM_SEGMENT_SIZE   (FILE_2GB_MINUS_1 - FILE_PAGE_SIZE + 1)

///Provide single file interface file date stored in multiple segment 
///files on disk. Keeps each segment size below 2GB for OSes without
///large file support.


std::string File::segmentName( const std::string& fileName, int segment ) {
  std::stringstream segName;
  segName << fileName << "$" << segment;
  return segName.str();
}

void File::_appendSegment() {
  FileSegment* segment = new FileSegment();
  int number = (int)_segments.size();
  std::string name = segmentName( _fileName, number );

  segment->start = size();
  segment->end = size();
  segment->stream.open( name.c_str(), std::ios::out | std::ios::binary );

  segment->stream.close();
  segment->stream.open( name.c_str(), std::ios::out | std::ios::binary | std::ios::in );

  _segments.push_back( segment );
}

File::offset_type File::_absolutePosition( offset_type relativePosition,
  offset_type currentPosition,
  std::fstream::seekdir direction ) const {
  offset_type newPosition;

  switch( direction ) {
    case std::fstream::beg:
      newPosition = relativePosition;
      break;

    case std::fstream::cur:
      newPosition = currentPosition + relativePosition;
      break;

    case std::fstream::end:
      newPosition = size() + relativePosition;
      break;
  }

  // seeking off the end of a file is not currently implemented
  assert( newPosition <= size() );

  return newPosition;
}

File::FileSegment* File::_segmentForPosition( offset_type absolutePosition, FileSegment* guess ) {
  assert( absolutePosition <= size() );
  assert( absolutePosition >= 0 );
  assert( _segments.size() );

  // see if the guess was good
  if( guess->contains( absolutePosition ) ) {
    return guess;
  }

  std::vector<FileSegment*>::iterator low = _segments.begin();
  std::vector<FileSegment*>::iterator high = _segments.end() - 1;
  std::vector<FileSegment*>::iterator middle;

  // if the user wants a position equal to or after the end of the file, 
  // pick the final segment
  if( (*high)->end <= absolutePosition ) {
    return *high;
  }

  assert( _segments.size() > 1 );

  while( high - low > 1 ) {
    middle = low+((high-low)/2);

    if( (*middle)->before(absolutePosition) ) {
      low = middle;
    } else {
      high = middle;
    }
  }

  if( (*high)->contains(absolutePosition) )
    return *high;
  else
    return *low;
}

void File::_validateReadPointer() {
  if( !_readPointerValid ) {
    _readSegment = _segmentForPosition( _readPosition, _readSegment );
    _readSegment->stream.seekg( library_offset_type(_readPosition - _readSegment->start), std::ios::beg );

    _readPointerValid = true;
    _writePointerValid = false;
  }

  assert( _readPosition <= _readSegment->end );
  assert( _readPosition >= _readSegment->start );
  assert( _readPosition == ( library_offset_type(_readSegment->stream.tellg()) + _readSegment->start) );
}

void File::_validateWritePointer() {
  if( !_writePointerValid ) {
    _writeSegment = _segmentForPosition( _writePosition, _writeSegment );
    _writeSegment->stream.seekg( library_offset_type(_writePosition - _writeSegment->start), std::ios::beg );

    _writePointerValid = true;
    _readPointerValid = false;
  }

  assert( _writePosition <= _writeSegment->end );
  assert( _writePosition >= _writeSegment->start );
  assert( _writePosition == ( library_offset_type(_writeSegment->stream.tellp()) + _writeSegment->start) || 
          _writePosition == 0 );
}

File::File() {
}

File::~File() {
  close();
}

void File::open( const std::string& fileName, int mode ) {
  close();
  
  FileSegment* segment;
  _readPosition = 0;
  _writePosition = 0;
  _readCount = 0;

  _readPointerValid = false;
  _writePointerValid = false;

  _mode = mode | std::fstream::binary; // must be in binary mode
  _state = std::fstream::goodbit;
  _fileName = fileName;

  // open all existing segments

  for( int i=0; ; i++ ) {
    segment = new FileSegment();
    std::string name = segmentName( _fileName, i );

    // we only care about a few mode bits, the rest are taken 
    // opening for read only, so that we can tell if the file exists or not (don't want to 
    // open files that don't exist)

    segment->stream.open( name.c_str(), std::ifstream::in | std::ifstream::binary );

    if( segment->stream.rdstate() & std::fstream::failbit ) {
      // segment didn't open, so assume that we're done opening
      delete segment;
      break;
    } else if ( mode & std::fstream::trunc ) {
      // segment opened properly, but we're doing a trunc, so this segment must go away
      segment->stream.close();
      delete segment;
      lemur_compat::remove( name.c_str() );
    } else {
      // segment opened properly and we'd like to keep it
      
      // if we want write access, we need to close and reopen the file
      if( mode & std::fstream::out ) {
        segment->stream.close();
        segment->stream.open( name.c_str(), lemur_compat::ios_mode_cast(mode & (std::fstream::binary | std::fstream::in | std::fstream::out)) );

        if( segment->stream.rdstate() & std::fstream::failbit ) {
          delete segment;
          break;
        }
      }

      // set up segment statistics and add to the segment vector
      offset_type length;

      segment->stream.seekg( 0, std::fstream::end );
      length = segment->stream.tellg();

      segment->start = _readPosition;
      segment->end = _readPosition + length;
      _readPosition += length;

      _segments.push_back( segment );
    }
  }

  // if no segments exist but we plan to write something, make a segment
  if( _segments.size() == 0 && (mode & std::fstream::out) ) {
    _appendSegment();
  }

  // if there are still no segments, quit and fail, otherwise finish setup
  if( _segments.size() == 0 ) {
    _state |= std::fstream::failbit;
  } else {
    // initialize these to something reasonable
    _readPosition = 0;
    _writePosition = 0;
    _readSegment = _segments[0];
    _writeSegment = _segments[0];

    // seek to the end of the file if necessary for certain flags
    // otherwise set up the pointers at file begin
    if( mode & (std::fstream::ate|std::fstream::app) ) {
      seekg( 0, std::fstream::end );
      seekp( 0, std::fstream::end );
    } else {
      seekg( 0, std::fstream::beg );
      seekp( 0, std::fstream::beg );
    }
  }
}

void File::close() {
  std::vector<FileSegment*>::iterator iter;

  for( iter = _segments.begin(); iter != _segments.end(); iter++ ) {
    (*iter)->stream.close();
    delete (*iter);
  }

  _segments.clear();
}

void File::read( void* buffer, offset_type count ) {
  _validateReadPointer();

  offset_type readAmount;
  _readCount = 0;

  while( ((count - _readCount + _readPosition) >= _readSegment->end) &&
          _readPosition != size() ) {
    readAmount = _readSegment->end - _readPosition;
    _readSegment->stream.read( (char*)buffer + _readCount, library_offset_type(readAmount) );
    
    _readPosition += readAmount;
    _readCount += readAmount;

    _readPointerValid = false;
    _validateReadPointer();
  }

  if( _readPosition != size() ) {
    _readSegment->stream.read( (char*)buffer + _readCount, library_offset_type(count - _readCount) );
    readAmount = library_offset_type(_readSegment->stream.gcount());
    _readPosition += readAmount;
    _readCount += readAmount;
  }

  assert((int)_readSegment->stream.tellg() != -1 || size() == _readPosition );
}

void File::write( const void* buffer, offset_type count ) {
  offset_type bytesWritten = 0;
  offset_type writeAmount = 0;
  _validateWritePointer();

  while( ((count - bytesWritten) + (_writePosition - _writeSegment->start)) >= FILE_MAXIMUM_SEGMENT_SIZE ) {
    writeAmount = FILE_MAXIMUM_SEGMENT_SIZE - (_writePosition - _writeSegment->start);

    _writeSegment->stream.write( (const char*)buffer + bytesWritten, library_offset_type(writeAmount) );
    _writePosition += writeAmount;
    bytesWritten += writeAmount;

    if( _writePosition > _writeSegment->end ) {
      _writeSegment->end = _writePosition;
    }

    _appendSegment();

    _writePointerValid = false;
    _validateWritePointer();
  }
  
  writeAmount = count - bytesWritten;
  _writeSegment->stream.write( (const char*)buffer + bytesWritten, library_offset_type(writeAmount) );
  _writePosition += writeAmount;
  bytesWritten += writeAmount;

  if( _writePosition > _writeSegment->end ) {
    _writeSegment->end = _writePosition;
  }

  assert( (int)_writeSegment->stream.tellp() != -1 || _writePosition == size() );
}

void File::seekg( offset_type relativePosition, std::fstream::seekdir direction ) {
  _readPosition = _absolutePosition( relativePosition, _readPosition, direction );
  _readPointerValid = false;
}

void File::seekp( offset_type relativePosition, std::fstream::seekdir direction ) {
  _writePosition = _absolutePosition( relativePosition, _writePosition, direction );
  _writePointerValid = false;
}

File::offset_type File::tellg() {
  return _readPosition;
}

File::offset_type File::tellp() {
  return _writePosition;
}

File::offset_type File::gcount() {
  return _readCount;
}

// only does eof right now
int File::rdstate() {
  if( size() == _readPosition )
    return _state | std::fstream::eofbit;
  else
    return _state;
}

File::offset_type File::size() const {
  if( _segments.size() == 0 ) {
    return 0;
  } else {
    return _segments[ _segments.size()-1 ]->end;
  }
}

void File::unlink() {
  close();
  File::unlink( _fileName );
  _fileName = "";
}

void File::unlink( const std::string& fileName ) {
  for( int i=0; ; i++ ) {
    std::string segment = segmentName( fileName, i );

    if( lemur_compat::remove( segment.c_str() ) != 0 ) {
      break;
    }
  }
}

void File::rename( const std::string& oldName, const std::string& newName ) {
  for( int i=0; ; i++ ) {
    std::string oldSegment = segmentName( oldName, i );
    std::string newSegment = segmentName( newName, i );

    if( ::rename( oldSegment.c_str(), newSegment.c_str() ) != 0 ) {
      break;
    }
  }
}

