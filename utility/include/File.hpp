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
// File.hpp
//
// 1 December 2003 - tds
//

#ifndef LEMUR_FILE_HPP
#define LEMUR_FILE_HPP

#include <vector>
#include <sstream>
#include <fstream>
#include <cassert>

//#define FILE_PAGE_SIZE              (4096)
#define FILE_PAGE_SIZE              (8192)
#define FILE_1GB                    (1024 * 1024 * 1024)
#define FILE_2GB_MINUS_1            (FILE_1GB + (FILE_1GB - 1))
#define FILE_MAXIMUM_SEGMENT_SIZE   (FILE_2GB_MINUS_1 - FILE_PAGE_SIZE + 1)
///Provide single file interface file date stored in multiple segment 
///files on disk. Keeps each segment size below 2GB for OSes without
///large file support.
class File {
public:
#ifdef WIN32
  /// Large file offset type
  typedef __int64 offset_type;
  /// IO library offset type
  typedef int library_offset_type;
  #define _UNLINK ::unlink
#else
  /// Large file offset type
  typedef off_t offset_type;
  /// IO library offset type
  typedef off_t library_offset_type;
  #define _UNLINK std::remove
#endif


private:
  struct FileSegment {
    std::fstream stream;
    offset_type start;
    offset_type end;

    bool contains( offset_type position ) {
      return start <= position && end > position;
    }

    bool before( offset_type position ) {
      return end <= position;
    }
  };

  std::string _fileName;
  std::vector<FileSegment*> _segments;

  FileSegment* _readSegment;
  FileSegment* _writeSegment;
  offset_type _readPosition; // seekg, read
  offset_type _writePosition; // seekp, write
  offset_type _readCount;
  bool _readPointerValid;
  bool _writePointerValid;
  int _mode;
  int _state;

  static std::string segmentName( const std::string& fileName, int segment ) {
    std::stringstream segName;
    segName << fileName << "$" << segment;
    return segName.str();
  }

  void _appendSegment() {
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
  

  offset_type _absolutePosition( offset_type relativePosition,
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
    default:
      break;
    }
    // seeking off the end of a file is not currently implemented
        assert( newPosition <= size() );

    return newPosition;
  }

  FileSegment* _segmentForPosition( offset_type absolutePosition, 
				    FileSegment* guess ) {
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

  void _validateReadPointer() {
    if( !_readPointerValid ) {
      _readSegment = _segmentForPosition( _readPosition, _readSegment );
      library_offset_type toHere = _readPosition - _readSegment->start;
      _readSegment->stream.seekg( toHere , std::ios::beg );
      _readPointerValid = true;
      _writePointerValid = false;
    }

        assert( _readPosition <= _readSegment->end );
        assert( _readPosition >= _readSegment->start );
        assert( _readPosition == ( library_offset_type(_readSegment->stream.tellg()) + _readSegment->start) );
  }

  void _validateWritePointer() {
    if( !_writePointerValid ) {
      _writeSegment = _segmentForPosition( _writePosition, _writeSegment );
      library_offset_type toHere = _writePosition - _writeSegment->start;
      _writeSegment->stream.seekp( toHere , std::ios::beg );
      _writePointerValid = true;
      _readPointerValid = false;
    }

        assert( _writePosition <= _writeSegment->end );
        assert( _writePosition >= _writeSegment->start );    
        assert( _writePosition == ( library_offset_type(_writeSegment->stream.tellp()) + _writeSegment->start) );
  }

public:
  File() {
  }

  ~File() {
    close();
  }
  /// Open the file named fileName with the provided mode flags.
  /// Initializes individual file segments.
  void open( const std::string& fileName, int mode ) {
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
      // opening for read only, so that we can tell if the file 
      // exists or not (don't want to 
      // open files that don't exist)

      segment->stream.open( name.c_str(), 
			    std::ifstream::in | std::ifstream::binary );

      if( segment->stream.rdstate() & std::fstream::failbit ) {
        // segment didn't open, so assume that we're done opening
        delete segment;
        break;
      } else if ( mode & std::fstream::trunc ) {
        // segment opened properly, but we're doing a trunc, so 
	// this segment must go away
        segment->stream.close();
        delete segment;
	_UNLINK(name.c_str());
      } else {
        // segment opened properly and we'd like to keep it
        // if we want write access, we need to close and reopen the file
        if( mode & std::fstream::out ) {
          segment->stream.close();
          segment->stream.open( name.c_str(), 
				// need to move this ifdef out of here
#ifdef WIN32 
				mode & (std::fstream::binary | 
					std::fstream::in | std::fstream::out) );
#else 
	  std::_Ios_Openmode(mode) & 
	    (std::fstream::binary | std::fstream::in | 
	     std::fstream::out) );
#endif
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

  /// Closes the File, closing all segments.
  void close() {
    std::vector<FileSegment*>::iterator iter;

    for( iter = _segments.begin(); iter != _segments.end(); iter++ ) {
      (*iter)->stream.close();
      delete (*iter);
    }

    _segments.clear();
  }
  /// Read count bytes into buffer from File.
  void read( void* buffer, offset_type count ) {
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
  ///Write count bytes from buffer into File.
  void write( const void* buffer, offset_type count ) {
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

       assert( (int)_writeSegment->stream.tellp() != -1 );
  }
  /// Move the get pointer. 
  void seekg( offset_type relativePosition, std::fstream::seekdir direction ) {
    _readPosition = _absolutePosition( relativePosition, _readPosition, direction );
    _readPointerValid = false;
  }
  /// Move the put pointer.
  void seekp( offset_type relativePosition, std::fstream::seekdir direction ) {
    _writePosition = _absolutePosition( relativePosition, _writePosition, direction );
    _writePointerValid = false;
  }
  /// Return the value of the get pointer.
  offset_type tellg() {
    return _readPosition;
  }
  /// Return the value of the put pointer.
  offset_type tellp() {
    return _writePosition;
  }
  /// Return the number of bytes read by last read.
  offset_type gcount() {
    return _readCount;
  }

  /// Returns eof bit set if at end of File.
  int rdstate() {
    if( size() == _readPosition )
      return _state | std::fstream::eofbit;
    else
      return _state;
  }
  /// Returns size of last segment in file.
  offset_type size() const {
    if( _segments.size() == 0 ) {
      return 0;
    } else {
      return _segments[ _segments.size()-1 ]->end;
    }
  }
  /// Close and remove File, and all segments from filesystem
  void unlink() {
    close();
    File::unlink( _fileName );
    _fileName = "";
  }

  /// Remove each segment from filesystem
  static void unlink( const std::string& fileName ) {
    for( int i=0; ; i++ ) {
      std::string segment = segmentName( fileName, i );
      if (_UNLINK( segment.c_str() ) != 0 ) {
        break;
      }
    }
  }
  /// Rename File from oldName to newName. Renames all segments.
  static void rename( const std::string& oldName, const std::string& newName ) {
    for( int i=0; ; i++ ) {
      std::string oldSegment = segmentName( oldName, i );
      std::string newSegment = segmentName( newName, i );

      if( ::rename( oldSegment.c_str(), newSegment.c_str() ) != 0 ) {
        break;
      }
    }
  }
};

#endif // LEMUR_FILE_HPP
