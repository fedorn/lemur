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

NullScorerNode::NullScorerNode( const std::string& name, TermScoreFunction& scoreFunction ) :
  _name(name),
  _scoreFunction(scoreFunction),
  _maximumBackgroundScore(scoreFunction.scoreOccurrence(0,MAX_INT32)),
  _maximumScore(scoreFunction.scoreOccurrence(0,1))
{
}

int NullScorerNode::nextCandidateDocument() {
  return MAX_INT32;
}

double NullScorerNode::maximumScore() {
  return _maximumScore;
}
  
double NullScorerNode::maximumBackgroundScore() {
  return _maximumBackgroundScore;
}

bool NullScorerNode::hasMatch( int documentID ) {
  return false;
}

const greedy_vector<ScoredExtentResult>& NullScorerNode::score( int documentID, int begin, int end, int documentLength ) {
  double score = _scoreFunction.scoreOccurrence( 0, end - begin );
  
  assert( score >= _maximumBackgroundScore && score <= _maximumScore );
  _scores.clear();
  _scores.push_back( ScoredExtentResult( score, documentID, begin, end ) );

  return _scores;
}

void NullScorerNode::annotate( Annotator& annotator, int documentID, int begin, int end ) {
  // no need to annotate; there will never be any matches
}

const std::string& NullScorerNode::getName() const {
  return _name;
}
