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

FilterRejectNode::FilterRejectNode( const std::string& name, 
				    ListIteratorNode* filter, 
				    BeliefNode* disallowed ) {
  _name = name;
  _filter = filter;
  _disallowed = disallowed;
}


int FilterRejectNode::nextCandidateDocument() {
  // it'd be nice to use the information from _filter to 
  // skip documents in the case when _filter->nextCandidate..() == _disallowed->nextCandidate...()
  // but we don't know for sure that _filter will match: we only know that it might match.
  // therefore we have to just go with the next _disallowed document.
  return _disallowed->nextCandidateDocument();
}

double FilterRejectNode::maximumBackgroundScore() {
  // delegate to the query as if the filter were true
  return _disallowed->maximumBackgroundScore();
}

double FilterRejectNode::maximumScore() {
  return _disallowed->maximumScore();
}

bool FilterRejectNode::hasMatch( int documentID ) {
  // delegate to the children.
  return (_filter->extents().size() == 0 &&
	  _disallowed->hasMatch( documentID ));
}

const std::string& FilterRejectNode::getName() const {
  return _name;
}

const greedy_vector<ScoredExtentResult>& FilterRejectNode::score( int documentID, int begin, int end, int documentLength ) {
  _extents.clear();
  // if the filter doesn't apply, return the child score.
  if (_filter->extents().size() == 0 )
    return _disallowed->score( documentID, begin, end, documentLength );
  else
    return _extents;
}

void FilterRejectNode::annotate( Annotator& annotator, int documentID, int begin, int end ) {
  _filter->annotate( annotator, documentID, begin, end );
  if( _filter->extents().size() == 0 ) {
    _disallowed->annotate( annotator, documentID, begin, end );
  }
}


