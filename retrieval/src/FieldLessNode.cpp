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
// FieldLessNode
//
// 28 July 2004 -- tds
//

#include "indri/FieldLessNode.hpp"
#include "indri/FieldIteratorNode.hpp"
#include "indri/Annotator.hpp"

FieldLessNode::FieldLessNode( const std::string& name, FieldIteratorNode* iterator, UINT64 constant ) {
  _field = iterator;
  _constant = constant;
  _name = name;
}

void FieldLessNode::prepare( int documentID ) {
  _extents.clear();

  if( !_field )
    return;

  const greedy_vector<UINT64>& numbers = _field->numbers();
  const greedy_vector<Extent>& extents = _field->extents();

  for( unsigned int i=0; i<numbers.size(); i++ ) {
    if( numbers[i] < _constant ) {
      _extents.push_back( extents[i] );
    }
  }
}

greedy_vector<Extent>& FieldLessNode::extents() {
  return _extents;
}

int FieldLessNode::nextCandidateDocument() {
  return _field->nextCandidateDocument();
}

const std::string& FieldLessNode::getName() const {
  return _name;
}

void FieldLessNode::annotate( class Annotator& annotator, int documentID, int begin, int end ) {
  annotator.addMatches( _extents, this, documentID, begin, end );
}


