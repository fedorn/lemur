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

FilterRequireNode::FilterRequireNode( const std::string& name, 
				      ListIteratorNode* filter, 
				      BeliefNode* required ) {
  _name = name;
  _filter = filter;
  _required = required;
}

int FilterRequireNode::nextCandidateDocument() {
  // both terms have to appear before this matches, so we take the max
  return lemur_compat::max( _filter->nextCandidateDocument(),
                            _required->nextCandidateDocument() );
}

double FilterRequireNode::maximumBackgroundScore() {
  // delegate to the query as if the filter were true
  return _required->maximumBackgroundScore();
}

double FilterRequireNode::maximumScore() {
  return _required->maximumScore();
}

bool FilterRequireNode::hasMatch( int documentID ) {
  // delegate to the children.
  return (_filter->extents().size() && _required->hasMatch( documentID ));
}

const std::string& FilterRequireNode::getName() const {
  return _name;
}

const greedy_vector<ScoredExtentResult>& FilterRequireNode::score( int documentID, int begin, int end, int documentLength ) {
  _extents.clear();
  // if the filter applies, return the child score.
  if (_filter->extents().size() )
    return _required->score( documentID, begin, end, documentLength );
  else
    return _extents;
}

void FilterRequireNode::annotate( Annotator& annotator, int documentID, int begin, int end ) {
  // mark up the filter
  _filter->annotate( annotator, documentID, begin, end );
  // if the filter applied, mark up the matches.
  if( _filter->extents().size() ) {
    _required->annotate( annotator, documentID, begin, end );
  }
}
