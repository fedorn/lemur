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
// Annotator
//
// 26 July 2004 -- tds
//

#include "indri/Annotator.hpp"

indri::infnet::Annotator::Annotator( const std::string& name, BeliefNode* belief )
  :
  _name(name),
  _belief(belief)
{
}

void indri::infnet::Annotator::add( InferenceNetworkNode* node, int documentID, indri::index::Extent &extent ) {
  indri::api::ScoredExtentResult a;

  a.document = documentID;
  a.begin = extent.begin;
  a.end = extent.end;
  a.score = 0;

  _annotations[node->getName()].push_back(a);
}

void indri::infnet::Annotator::addMatches( indri::utility::greedy_vector<indri::index::Extent>& extents, InferenceNetworkNode* node, int documentID, indri::index::Extent &extent ) {
  indri::index::Extent range( extent.begin, extent.end );

  indri::utility::greedy_vector<indri::index::Extent>::const_iterator iter;
  iter = std::lower_bound( extents.begin(), extents.end(), range, indri::index::Extent::begins_before_less() );

  for( size_t i = iter - extents.begin(); i<extents.size(); i++ ) {
    if( extent.begin > extents[i].begin )
      continue;

    if( extent.end < extents[i].end )
      continue;

    add( node, documentID, (indri::index::Extent &)extents[i]);
  }
}

void indri::infnet::Annotator::evaluate( int documentID, int documentLength ) {
  indri::index::Extent tmpExtent(0, documentLength);
  _belief->annotate( *this, documentID, tmpExtent );
}

int indri::infnet::Annotator::nextCandidateDocument() {
  return _belief->nextCandidateDocument();
}

indri::infnet::EvaluatorNode::MResults& indri::infnet::Annotator::getResults() {
  return _annotations;
}

const std::string& indri::infnet::Annotator::getName() const {
  return _name;
}

const indri::infnet::EvaluatorNode::MResults& indri::infnet::Annotator::getResults() const {
  return _annotations;
}

void indri::infnet::Annotator::indexChanged( indri::index::Index& index ) {
  // do nothing
}
