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

indri::infnet::FilterRejectNode::FilterRejectNode( const std::string& name, 
                                                   indri::infnet::ListIteratorNode* filter, 
                                                   indri::infnet::BeliefNode* disallowed ) {
  _name = name;
  _filter = filter;
  _disallowed = disallowed;
}


int indri::infnet::FilterRejectNode::nextCandidateDocument() {
  // it'd be nice to use the information from _filter to 
  // skip documents in the case when _filter->nextCandidate..() == _disallowed->nextCandidate...()
  // but we don't know for sure that _filter will match: we only know that it might match.
  // therefore we have to just go with the next _disallowed document.
  return _disallowed->nextCandidateDocument();
}

double indri::infnet::FilterRejectNode::maximumBackgroundScore() {
  // delegate to the query as if the filter were true
  return _disallowed->maximumBackgroundScore();
}

double indri::infnet::FilterRejectNode::maximumScore() {
  return _disallowed->maximumScore();
}

bool indri::infnet::FilterRejectNode::hasMatch( int documentID ) {
  // delegate to the children.
  return (_filter->extents().size() == 0 &&
          _disallowed->hasMatch( documentID ));
}

const indri::utility::greedy_vector<bool>& indri::infnet::FilterRejectNode::hasMatch( int documentID, const indri::utility::greedy_vector<indri::index::Extent>& extents ) {
  if( _filter->extents().size() == 0 ) {
    return _disallowed->hasMatch( documentID, extents );
  }

  _matches.resize( extents.size(), false );
  return _matches;
}

const std::string& indri::infnet::FilterRejectNode::getName() const {
  return _name;
}

const indri::utility::greedy_vector<indri::api::ScoredExtentResult>& indri::infnet::FilterRejectNode::score( int documentID, indri::index::Extent &extent, int documentLength ) {
  _extents.clear();
  // if the filter doesn't apply, return the child score.
  if ( _filter->extents().size() == 0 )
    return _disallowed->score( documentID, extent, documentLength );
  else
    return _extents;
}

void indri::infnet::FilterRejectNode::annotate( Annotator& annotator, int documentID, indri::index::Extent &extent ) {
  _filter->annotate( annotator, documentID, extent);
  if( _filter->extents().size() == 0 ) {
    _disallowed->annotate( annotator, documentID, extent );
  }
}

void indri::infnet::FilterRejectNode::indexChanged( indri::index::Index& index ) {
  // do nothing
}

