/*==========================================================================
 * Copyright (c) 2003 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */


//
// KeyfileDocListSegmentReader
//
// tds - 13 November 2003
//

#include "KeyfileDocListSegmentReader.hpp"
#include "ReadBuffer.hpp"
#include <sstream>

lemur::file::KeyfileDocListSegmentReader::KeyfileDocListSegmentReader( File* stream, std::string& baseName, int segment, int readBufferSize ) :
  _stream( stream ),
  _segment( segment )
{
  _top = 0;
  _stream->seekg( 0, std::fstream::beg );
  _file = new ReadBuffer( *_stream, readBufferSize );

  std::stringstream nameStream;
  nameStream << baseName << segment;
  _name = nameStream.str();
}

lemur::file::KeyfileDocListSegmentReader::~KeyfileDocListSegmentReader() {
  delete _top;
  delete _file;
}

lemur::file::File* lemur::file::KeyfileDocListSegmentReader::file() {
  return _stream;
}

lemur::index::InvFPDocList* lemur::file::KeyfileDocListSegmentReader::next() {
  if( (_file->rdstate() & std::fstream::eofbit) ) {
    // file is at end, no more streams left
    return 0;
  } else {
    // read enough to find out how long the thing is
    // alignment issues on solaris?
    //    int* header = (int*) _file->peek( sizeof(int) * 4 );
    const char* head = _file->peek( sizeof(int) * 4 );
    int header[4];
    
    //    if( !header ) {
    if( !head ) {
      return 0;
    }

    memcpy(&header, head, sizeof(int) * 4 );

    int dataLength = header[3];
    // alignment issue on solaris.
    //    const char* listData = _file->read( (sizeof(int)*4) + dataLength );
    int *listData = new int[4 + (dataLength/sizeof(int)) + 1];
    _file->read((char *)listData, (sizeof(int)*4) + dataLength );
    // make a list from the data we read
    // dmf FIXME
    lemur::index::InvFPDocList* list = new lemur::index::InvFPDocList( const_cast<lemur::api::LOC_T*>( (const lemur::api::LOC_T*) listData ) );
    //clean up alignment data.
    delete[](listData);
    return list;
  }
}

bool lemur::file::KeyfileDocListSegmentReader::operator<( const KeyfileDocListSegmentReader& other ) const {
  const lemur::index::InvFPDocList* otherTop = other.top();
  const lemur::index::InvFPDocList* thisTop = top();

  // if neither object has data, the smaller segment
  // number is 'better'
  if( !thisTop && !otherTop )
    return segment() > other.segment();

  // if we have data but the other object doesn't,
  // we are bigger (we should go first)
  if( !otherTop )
    return false;

  lemur::api::TERMID_T termID = const_cast<lemur::index::InvFPDocList*>(thisTop)->termID();
  lemur::api::TERMID_T otherTermID = const_cast<lemur::index::InvFPDocList*>(otherTop)->termID();

  // if our term ids are the same, the smaller segment is 'better'
  if( termID == otherTermID ) {
    return segment() > other.segment();
  }

  // if our term ids aren't the same, smaller termID is 'better'
  return termID > otherTermID;
}

const lemur::index::InvFPDocList* lemur::file::KeyfileDocListSegmentReader::top() const {
  return _top;
}

lemur::index::InvFPDocList* lemur::file::KeyfileDocListSegmentReader::top() {
  return _top;
}

void lemur::file::KeyfileDocListSegmentReader::pop() {
  _top = next();
}

int lemur::file::KeyfileDocListSegmentReader::segment() const {
  return _segment;
}

const std::string& lemur::file::KeyfileDocListSegmentReader::name() const {
  return _name;
}
