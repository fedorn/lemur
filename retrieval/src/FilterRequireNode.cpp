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
// FilterRequireNode
//
// 6 July 2004 -- tds
//

#include "indri/FilterRequireNode.hpp"
#include "lemur-compat.hpp"
#include "indri/Extent.hpp"
#include "indri/Annotator.hpp"

FilterRequireNode::FilterRequireNode( const std::string& name, ListIteratorNode* filtered, ListIteratorNode* required ) {
  _name = name;
  _filtered = filtered;
  _required = required;
}

void FilterRequireNode::prepare( int documentID ) {
  _extents.clear();

  // if the required term matches, take the filtered term's extents;
  // otherwise don't match
  if( _required->extents().size() != 0 ) {
    _extents = _filtered->extents();
  }
}

const greedy_vector<Extent>& FilterRequireNode::extents() {
  return _extents;
}

int FilterRequireNode::nextCandidateDocument() {
  // both terms have to appear before this matches, so we take the max
  return lemur_compat::max( _filtered->nextCandidateDocument(),
                            _required->nextCandidateDocument() );
}

const std::string& FilterRequireNode::getName() const {
  return _name;
}

void FilterRequireNode::annotate( Annotator& annotator, int documentID, int begin, int end ) {
  annotator.addMatches( _extents, this, documentID, begin, end );
  _required->annotate( annotator, documentID, begin, end );

  if( _required->extents().size() ) {
    _filtered->annotate( annotator, documentID, begin, end );
  }
}
