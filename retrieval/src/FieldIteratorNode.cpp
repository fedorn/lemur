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
// FieldIteratorNode
//
// 28 July 2004 -- tds
//

#include "indri/FieldIteratorNode.hpp"
#include "indri/FieldListIterator.hpp"
#include "indri/Annotator.hpp"

FieldIteratorNode::FieldIteratorNode( const std::string& name, indri::index::FieldListIterator* field ) :
  _field(field),
  _name(name)
{
  if( _field ) {
    _field->startIteration();
    _field->nextEntry();
  }
}

void FieldIteratorNode::prepare( int documentID ) {
  _extents.clear();
  _numbers.clear();

  if( !_field )
    return;

  const indri::index::FieldExtentInfo* info = _field->currentEntry();

  if( info && info->documentID == documentID ) {
    _extents = info->extents;
    _numbers = info->numbers;
  }
}

/// returns a list of intervals describing positions of children
const greedy_vector<Extent>& FieldIteratorNode::extents() {
  return _extents;
}

const greedy_vector<UINT64>& FieldIteratorNode::numbers() {
  return _numbers;
}

int FieldIteratorNode::nextCandidateDocument() {
  if( !_field )
    return MAX_INT32;

  const indri::index::FieldExtentInfo* info = _field->currentEntry();

  if( !info ) {
    return MAX_INT32;
  } else {
    return info->documentID;
  }
}

const std::string& FieldIteratorNode::getName() const {
  return _name;
}

void FieldIteratorNode::annotate( Annotator& annotator, int documentID, int begin, int end ) {
  annotator.addMatches( _extents, this, documentID, begin, end );
}


