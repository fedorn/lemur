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
// FieldGreaterNode
//
// 28 July 2004 -- tds
//

#include "indri/FieldGreaterNode.hpp"
#include "indri/Annotator.hpp"
#include "indri/FieldIteratorNode.hpp"

FieldGreaterNode::FieldGreaterNode( const std::string& name, FieldIteratorNode* iterator, INT64 constant ) {
  _name = name;
  _field = iterator;
  _constant = constant;
}

void FieldGreaterNode::prepare( int documentID ) {
  _extents.clear();

  if( !_field )
    return;

  const greedy_vector<INT64>& numbers = _field->numbers();
  const greedy_vector<Extent>& extents = _field->extents();

  for( unsigned int i=0; i<numbers.size(); i++ ) {
    if( numbers[i] > _constant ) {
      _extents.push_back( extents[i] );
    }
  }
}

greedy_vector<Extent>& FieldGreaterNode::extents() {
  return _extents;
}

int FieldGreaterNode::nextCandidateDocument() {
  return _field->nextCandidateDocument();
}

const std::string& FieldGreaterNode::getName() const {
  return _name;
}

void FieldGreaterNode::annotate( Annotator& annotator, int documentID, int begin, int end ) {
  annotator.addMatches( _extents, this, documentID, begin, end );
}
