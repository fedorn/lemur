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
#include <assert.h>
#include "lemur-platform.h"
#include "lemur-compat.hpp"

///Provide single file interface file date stored in multiple segment 
///files on disk. Keeps each segment size below 2GB for OSes without
///large file support.

class File {
public:
  typedef INT64 offset_type;
  typedef FILE_OFFSET library_offset_type;

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

  static std::string segmentName( const std::string& fileName, int segment );
  void _appendSegment();

  offset_type _absolutePosition( offset_type relativePosition,
    offset_type currentPosition,
    std::fstream::seekdir direction ) const;
  FileSegment* _segmentForPosition( offset_type absolutePosition, FileSegment* guess );
  void _validateReadPointer();
  void _validateWritePointer();

public:
  File();
  ~File();
  void open( const std::string& fileName, int mode );
  void close();
  void read( void* buffer, offset_type count );
  void write( const void* buffer, offset_type count );
  void seekg( offset_type relativePosition, std::fstream::seekdir direction );
  void seekp( offset_type relativePosition, std::fstream::seekdir direction ) ;
  offset_type tellg();
  offset_type tellp();
  offset_type gcount();

  // only does eof right now
  int rdstate();
  offset_type size() const;
  void unlink();

  static void unlink( const std::string& fileName );
  static void rename( const std::string& oldName, const std::string& newName );
};

#endif // LEMUR_FILE_HPP
