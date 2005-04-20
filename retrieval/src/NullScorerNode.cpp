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
// NullScorerNode
//
// 6 July 2004 -- tds
//

#include "indri/NullScorerNode.hpp"
#include "lemur-compat.hpp"

indri::infnet::NullScorerNode::NullScorerNode( const std::string& name, indri::query::TermScoreFunction& scoreFunction ) :
  _name(name),
  _scoreFunction(scoreFunction),
  _maximumBackgroundScore(scoreFunction.scoreOccurrence(0,100)),
  _maximumScore(scoreFunction.scoreOccurrence(0,100))
{
}

int indri::infnet::NullScorerNode::nextCandidateDocument() {
  return MAX_INT32;
}

double indri::infnet::NullScorerNode::maximumScore() {
  return _maximumScore;
}
  
double indri::infnet::NullScorerNode::maximumBackgroundScore() {
  return _maximumBackgroundScore;
}

bool indri::infnet::NullScorerNode::hasMatch( int documentID ) {
  return false;
}

const indri::utility::greedy_vector<bool>& indri::infnet::NullScorerNode::hasMatch( int documentID, const indri::utility::greedy_vector<indri::index::Extent>& extents ) {
  return _matches;
}

const indri::utility::greedy_vector<indri::api::ScoredExtentResult>& indri::infnet::NullScorerNode::score( int documentID, int begin, int end, int documentLength ) {
  _scores.clear();
  _scores.push_back( indri::api::ScoredExtentResult( _maximumScore, documentID, begin, end ) );

  return _scores;
}

void indri::infnet::NullScorerNode::annotate( indri::infnet::Annotator& annotator, int documentID, int begin, int end ) {
  // no need to annotate; there will never be any matches
}

const std::string& indri::infnet::NullScorerNode::getName() const {
  return _name;
}

void indri::infnet::NullScorerNode::indexChanged( indri::index::Index& index ) {
  // do nothing
}


