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

indri::infnet::FilterRequireNode::FilterRequireNode( const std::string& name, 
                                                     ListIteratorNode* filter, 
                                                     BeliefNode* required ) {
  _name = name;
  _filter = filter;
  _required = required;
}

int indri::infnet::FilterRequireNode::nextCandidateDocument() {
  // both terms have to appear before this matches, so we take the max
  return lemur_compat::max( _filter->nextCandidateDocument(),
                            _required->nextCandidateDocument() );
}

double indri::infnet::FilterRequireNode::maximumBackgroundScore() {
  // delegate to the query as if the filter were true
  return _required->maximumBackgroundScore();
}

double indri::infnet::FilterRequireNode::maximumScore() {
  return _required->maximumScore();
}

bool indri::infnet::FilterRequireNode::hasMatch( int documentID ) {
  // delegate to the children.
  return (_filter->extents().size() && _required->hasMatch( documentID ));
}

const indri::utility::greedy_vector<bool>& indri::infnet::FilterRequireNode::hasMatch( int documentID, const indri::utility::greedy_vector<indri::index::Extent>& extents ) {
  if( _filter->extents().size() ) {
    return _required->hasMatch( documentID, extents );
  }

  _matches.resize( extents.size(), false );
  return _matches;
}

const std::string& indri::infnet::FilterRequireNode::getName() const {
  return _name;
}

const indri::utility::greedy_vector<indri::api::ScoredExtentResult>& indri::infnet::FilterRequireNode::score( int documentID, indri::index::Extent &extent, int documentLength ) {
  _extents.clear();
  // if the filter applies, return the child score.
  if (_filter->extents().size() )
    return _required->score( documentID, extent, documentLength );
  else
    return _extents;
}

void indri::infnet::FilterRequireNode::annotate( Annotator& annotator, int documentID, indri::index::Extent &extent ) {
  // mark up the filter
  _filter->annotate( annotator, documentID, extent);
  // if the filter applied, mark up the matches.
  if( _filter->extents().size() ) {
    _required->annotate( annotator, documentID, extent );
  }
}

void indri::infnet::FilterRequireNode::indexChanged( indri::index::Index& index ) {
  // do nothing
}
