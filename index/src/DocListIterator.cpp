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
// DocListIterator
//
// 9 January 2004 - tds
//

#include "indri/DocListIterator.hpp"

using namespace indri::index;

DocListIterator::DocListIterator( File& segment,
                                                int termID,
                                                File::offset_type seekLocation,
                                                INT64 estimatedDataLength )
  :
  _fileIterator( &segment,
                 0,
                 static_cast<size_t>(lemur_compat::min<INT64>( estimatedDataLength, KEYFILE_DOCLISTITERATOR_MAX_BUFFERSIZE )),
                 seekLocation ),
  _termID(termID)
{
  _finished = true;
  _atNext = false;
}

DocListIterator::~DocListIterator() {
}

// get the iterator ready to return data; call this before calling currentEntry or nextEntry
void DocListIterator::startIteration() const {
  _atNext = false;
  _finished = false;

  _fileIterator.startIteration();
  
  if( !_fileIterator.skip( _termID, 0 ) ) {
    _finished = true;
    return;
  } else {
    _atNext = true;
  }
}

/// Important note: In this implementation, hasMore() invalidates any
/// structures returned by nextEntry() or nextEntry( docID ).
bool DocListIterator::hasMore() const {
  if( _finished )
    return false;

  if( _atNext )
    return true;

  _atNext = true;
  _finished = !_fileIterator.nextDocument() || _fileIterator.termID() != _termID;
  return !_finished;
}

// move to the next document in the list; return null if there are no more valid documents
DocInfo* DocListIterator::nextEntry() const {
  if( _finished ) {
    return 0;
  }

  // the hasNext call moves the fileIterator forward, so
  // we need to see if we need to move it forward ourselves
  if( ! _atNext ) {
    if( !_fileIterator.nextDocument() || _fileIterator.termID() != _termID ) {
      _finished = true;
      return 0;
    }
  }
  
  _atNext = false;
  DocListInfo& result = _fileIterator.currentDocument();
  return &result;
}

// find the first document that contains this term that has an id >= documentID.
// returns null if no such document exists.
DocInfo* DocListIterator::nextEntry( DOCID_T documentID ) {
  if( _finished )
    return 0;

  _atNext = false;

  if( documentID <= _fileIterator.currentDocument().docID() ) {
    return currentEntry();
  }

  if( !_fileIterator.skip( _termID, documentID ) ) {
    _finished = true;
    return 0;
  } else {
    _atNext = true;
    return nextEntry();
  }
}

// return the current document entry if we're not finished, null otherwise.
DocInfo* DocListIterator::currentEntry() {
  if( !_finished )
    return &_fileIterator.currentDocument();
  else
    return 0;
}

/// set element from position, returns pointer to the element
DocInfo* DocListIterator::getElement(DocInfo* elem, POS_T position) const {
  DocListInfo *tmp = dynamic_cast<DocListInfo *>(elem);
  DocListInfo *cur = &_fileIterator.currentDocument();
  tmp->clear();
  tmp->setDocID(cur->docID());
  tmp->setTermID(cur->termID());
  tmp->addPositions((LOC_T *)(cur->positions()),cur->termCount());
  return elem;
}

/// Position at beginning of list
// changes the state of the underlying fileIterator.
// Warning warning, Danger Will Robinson.
// This will break if someone calls begin() multiple times.
POS_T DocListIterator::beginPosition() const {
  _finished = false;
  _fileIterator.startIteration();
  if( !_fileIterator.skip( _termID, 0 ) )
    _finished = true;
  if (_finished)
    return (POS_T) -2;
  else 
    return (POS_T) 0;  
}

/// Position at end of list
POS_T DocListIterator::endPosition() const {
  return (POS_T) -2;  
}

/// Advance to next position
POS_T DocListIterator::nextPosition(POS_T position) const{
  _finished = _finished || !_fileIterator.nextDocument() || 
              _fileIterator.termID() != _termID;
  if( _finished )
    return (POS_T) -2;
  else {
    unsigned int p = (unsigned int) position;
    return (POS_T)++p;
  }
}

