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
// DocListFileIterator
//
// 9 January 2004 - tds
//

#include "indri/DocListFileIterator.hpp"
#include "File.hpp"
#include "RVLCompress.hpp"

using namespace indri::index;

DocListFileIterator::DocListFileIterator( File* file, int segment, size_t bufferSize, File::offset_type fileOffset ) :
  _file(file),
  _readBuffer(*file, lemur_compat::max<size_t>( INDRI_DOCLIST_BLOCKSIZE, bufferSize ), false),
  _fileOffset(fileOffset)
{
  _segment = segment;
  _finished = false;
}

bool DocListFileIterator::_skipToNextBlock() {
  const char* nextBlock = _readBuffer.read( INDRI_DOCLIST_BLOCKSIZE );
  _reader.setBlock( nextBlock );

  if( !nextBlock ) {
    _finished = true;
    return false;
  }

  return true;
}

bool DocListFileIterator::finished() const {
  return _finished;
}

void DocListFileIterator::startIteration() {
  _info.setDocID( -1 );
  _partialDocument = false;
  _finished = false;
  _readBuffer.seekg( _fileOffset * INDRI_DOCLIST_BLOCKSIZE, std::fstream::beg );
  _skipToNextBlock();
}

bool DocListFileIterator::skip( int termID, int documentID ) {
  if( _finished || _reader.finished() )
    return false;

  // this loop lets us skip whole blocks of the inverted list
  // instead of looking at every posting
  while( _reader.before( termID, documentID ) ) {
    bool result = _skipToNextBlock();

    if( !result ) {
      return false;
    }
  }
    
  if( _reader.finished() || !_reader.skip( termID, documentID ) ) {
    return false;
  }

  // we are now pointing at data for the next document, so
  // we call _fetchDocument() to get that data
  _fetchDocument();
  assert( _info.docID() >= documentID );
  assert( _info.termID() >= termID );
  return true;
}

DocListInfo& DocListFileIterator::currentDocument() {
  return _info;
}

void DocListFileIterator::_fetchDocument() {
  // there's now at least some document information
  // waiting for us, so we'll fetch it
  // note that all return statements after this point
  // should return true (because we got data)

  _info.clear();
  _partialDocument = false;
  _reader.currentDocument( _info );

  int currentTermID = _info.termID();
  int currentDocumentID = _info.docID();
  
  // the document's information may span multiple
  // blocks, so continue to fetch blocks until we
  // find a document entry that isn't the current one
  while( ! _reader.hasMore() ) {
    if( ! _skipToNextBlock() ) {
      _finished = true;
      break;
    }

    if( ! _reader.nextDocument() ) {
      _finished = true;
      break;
    }

    if( _reader.term() == currentTermID &&
        _reader.document() == currentDocumentID ) {
      // add the position information from this block
      // to the _info structure
      _reader.currentDocument( _info );
    } else {
      _partialDocument = true;
      break;
    }
  }
}

//
// Move the iterator pointer to the next document entry
// in the file.  If there is no such entry, this method returns
// false.
//

bool DocListFileIterator::nextDocument() {
  if( _finished )
    return false;

  // we need to move to the next document only
  // if we don't have at least a partial document structure
  // waiting for us from the last call
  if( ! _partialDocument ) {
    if( ! _reader.nextDocument() ) {
      // if the reader doesn't have any more documents, then it's
      // time to go to the next block
      if( ! _skipToNextBlock() ) {
        // no more blocks, time to quit
        return false;
      } else if( !_reader.nextDocument() ) {
        // we got to the next block, but somehow
        // there weren't any documents there
        return false;
      }
    }
  }

  _fetchDocument();
  return true;
}

int DocListFileIterator::document() const {
  return _info.docID();
}

int DocListFileIterator::termID() const {
  return _info.termID();
}

int DocListFileIterator::segment() const {
  return _segment;
}

void DocListFileIterator::remove() {
  _file->close();
  _file->unlink();
  delete _file;
  _file = 0;
}


