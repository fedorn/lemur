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
// FilterRejectNode
//
// 6 July 2004 -- tds
//

#include "lemur-compat.hpp"
#include "indri/FilterRejectNode.hpp"
#include "indri/ListIteratorNode.hpp"
#include "indri/greedy_vector"
#include "indri/Extent.hpp"
#include "indri/Annotator.hpp"

FilterRejectNode::FilterRejectNode( const std::string& name, ListIteratorNode* filtered, ListIteratorNode* disallowed ) {
  _name = name;
  _filtered = filtered;
  _disallowed = disallowed;
}

void FilterRejectNode::prepare( int documentID ) {
  _extents.clear();

  // only if the disallowed term doesn't match can we return any matches
  if( _disallowed->extents().size() == 0 ) {
    _extents = _filtered->extents();
  }
}

const greedy_vector<Extent>& FilterRejectNode::extents() {
  return _extents;
}

int FilterRejectNode::nextCandidateDocument() {
  // it'd be nice to use the information from _disallowed to 
  // skip documents in the case when _filtered->nextCandidate..() == _disallowed->nextCandidate...()
  // but we don't know for sure that _disallowed will match: we only know that it might match.
  // therefore we have to just go with the next filtered document.
  return _filtered->nextCandidateDocument();
}

const std::string& FilterRejectNode::getName() const {
  return _name;
}

void FilterRejectNode::annotate( Annotator& annotator, int documentID, int begin, int end ) {
  annotator.addMatches( _extents, this, documentID, begin, end );
  _disallowed->annotate( annotator, documentID, begin, end );

  if( _disallowed->extents().size() == 0 ) {
    _filtered->annotate( annotator, documentID, begin, end );
  }
}


