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
// FieldBetweenNode
//
// 28 July 2004 -- tds
//

#include "indri/FieldBetweenNode.hpp"
#include "indri/Annotator.hpp"
#include "indri/FieldIteratorNode.hpp"

FieldBetweenNode::FieldBetweenNode( const std::string& name, FieldIteratorNode* iterator, INT64 low, INT64 high ) {
  _name = name;
  _field = iterator;
  _low = low;
  _high = high;
}

void FieldBetweenNode::prepare( int documentID ) {
  _extents.clear();
  
  if( !_field )
    return;

  const greedy_vector<INT64>& numbers = _field->numbers();
  const greedy_vector<Extent>& extents = _field->extents();

  for( unsigned int i=0; i<numbers.size(); i++ ) {
    if( numbers[i] >= _low && numbers[i] < _high ) {
      _extents.push_back( extents[i] );
    }
  }
}

greedy_vector<Extent>& FieldBetweenNode::extents() {
  return _extents;
}

int FieldBetweenNode::nextCandidateDocument() {
  return _field->nextCandidateDocument();
}

const std::string& FieldBetweenNode::getName() const { 
  return _name;
}

void FieldBetweenNode::annotate( class Annotator& annotator, int documentID, int begin, int end ) {
  annotator.addMatches( _extents, this, documentID, begin, end );
}


