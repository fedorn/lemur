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
// WeightedAndNode
//
// 26 January 2004 - tds
//

#include "indri/WeightedAndNode.hpp"
#include <algorithm>
#include "lemur-platform.h"
#include <iostream>
#include "indri/Annotator.hpp"

double WeightedAndNode::_computeMaxScore( unsigned int start ) {
  // first, find the maximum score of the first few columns
  double maxScoreSum = 0;

  for( unsigned int i=0; i<start+1; i++ ) {
    maxScoreSum += _children[i].maximumWeightedScore;
  }

  // then add in the background score of the last columns
  double minScoreSum = 0;

  for( unsigned int i=start+1; i<_children.size(); i++ ) {
    minScoreSum += _children[i].backgroundWeightedScore;
  }

  return maxScoreSum + minScoreSum;
}

void WeightedAndNode::_computeQuorum() {
  double maximumScore = -DBL_MAX;
  unsigned int i;

  // keep going until we find a necessary term
  for( i=0; i<_children.size() && maximumScore < _threshold; i++ ) {
    maximumScore = _computeMaxScore(i);
  }

  _quorumIndex = i-1;
  if( _quorumIndex < 0 )
    _quorumIndex = 0;

  if( _quorumIndex > _children.size()-1 )
    _recomputeThreshold = DBL_MAX;
  else
    _recomputeThreshold = maximumScore;
}

void WeightedAndNode::addChild( double weight, BeliefNode* node ) {
  child_type child;

  child.node = node;
  child.weight = weight;
  child.backgroundWeightedScore = node->maximumBackgroundScore() * weight;
  child.maximumWeightedScore = node->maximumScore() * weight;

  _children.push_back( child );
  std::sort( _children.begin(), _children.end(), child_type::maxscore_less() );
  _computeQuorum();
}

void WeightedAndNode::setThreshold( double threshold ) {
  _threshold = threshold;

  if( _threshold >= _recomputeThreshold ) {
    _computeQuorum();
  }
}
  
int WeightedAndNode::nextCandidateDocument() {
  std::vector<child_type>::iterator iter;
  int minDocument = MAX_INT32;
  int currentCandidate;

  for( iter = _children.begin() + _quorumIndex; iter != _children.end(); iter++ ) {
    currentCandidate = (*iter).node->nextCandidateDocument();

    if( currentCandidate < minDocument ) {
      minDocument = currentCandidate;
    }
  }

  return minDocument;
}

void WeightedAndNode::annotate( Annotator& annotator, int documentID, int begin, int end ) {
  std::vector<child_type>::iterator iter;
  annotator.add( this, documentID, begin, end );

  for( iter = _children.begin(); iter != _children.end(); iter++ ) {
    (*iter).node->annotate( annotator, documentID, begin, end );
  }
}

double WeightedAndNode::maximumBackgroundScore() {
  std::vector<child_type>::iterator iter;
  double minimum = 0.0;

  for( iter = _children.begin(); iter != _children.end(); iter++ ) {
    minimum += (*iter).weight * (*iter).node->maximumBackgroundScore();
  }

  return minimum;
}

double WeightedAndNode::maximumScore() {
  std::vector<child_type>::iterator iter;
  double maximum = 0.0;

  for( iter = _children.begin(); iter != _children.end(); iter++ ) {
    maximum += (*iter).weight * (*iter).node->maximumScore();
  }

  return maximum;
}

greedy_vector<ScoredExtentResult>& WeightedAndNode::score( int documentID, int begin, int end, int documentLength ) {
  std::vector<child_type>::iterator iter;
  double score = 0;

  for( iter = _children.begin(); iter != _children.end(); iter++ ) {
    const greedy_vector<ScoredExtentResult>& childResults = (*iter).node->score( documentID, begin, end, documentLength );

    double childScore = 0;
    for( unsigned int j=0; j<childResults.size(); j++ ) {
      childScore += (*iter).weight * childResults[j].score;
    }

    assert( childScore <= iter->maximumWeightedScore );
    score += childScore;
  }

  _scores.clear();
  _scores.push_back( ScoredExtentResult(score, documentID, begin, end) );

  return _scores;
}

bool WeightedAndNode::hasMatch( int documentID ) {
  for( unsigned int i=0; i<_children.size(); i++ ) {
    if( _children[i].node->hasMatch( documentID ) )
      return true;
  }

  return false;
}

const std::string& WeightedAndNode::getName() const {
  return _name;
}

