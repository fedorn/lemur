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
// WeightedSumNode
//
// 6 July 2004 -- tds
//

#include "indri/WeightedSumNode.hpp"
#include "lemur-compat.hpp"
#include <math.h>
#include "indri/Annotator.hpp"

WeightedSumNode::WeightedSumNode( const std::string& name ) : _name(name)
{
}

int WeightedSumNode::nextCandidateDocument() {
  int candidate = MAX_INT32;

  for( unsigned int i=0; i<_children.size(); i++ ) {
    candidate = lemur_compat::min<int>( _children[i]->nextCandidateDocument(), candidate );
  }

  return candidate;
}

double WeightedSumNode::maximumScore() {
  double s = 0;

  for( unsigned i=0; i<_children.size(); i++ ) {
    s += _weights[i] * exp( _children[i]->maximumScore() );
  }

  return log(s);
}

double WeightedSumNode::maximumBackgroundScore() {
  double s = 0;

  for( unsigned i=0; i<_children.size(); i++ ) {
    s += _weights[i] * exp( _children[i]->maximumBackgroundScore() );
  }

  return log(s);
}

const greedy_vector<ScoredExtentResult>& WeightedSumNode::score( int documentID, int begin, int end, int documentLength ) {
  double s = 0;

  for( unsigned i=0; i<_children.size(); i++ ) {
    const greedy_vector<ScoredExtentResult>& childResults = _children[i]->score( documentID, begin, end, documentLength );

    for( unsigned int j=0; j<childResults.size(); j++ ) {
      s += _weights[i] * exp( childResults[j].score );
    }
  }

  _scores.clear();
  s = log(s); // return to log space
  _scores.push_back( ScoredExtentResult(s, documentID, begin, end) );

  return _scores;
}

void WeightedSumNode::annotate( Annotator& annotator, int documentID, int begin, int end ) {
  annotator.add(this, documentID, begin, end);

  for( unsigned i=0; i<_children.size(); i++ ) {
    _children[i]->annotate( annotator, documentID, begin, end );
  }
}

bool WeightedSumNode::hasMatch( int documentID ) {
  for( unsigned int i=0; i<_children.size(); i++ ) {
    if( _children[i]->hasMatch( documentID ) )
      return true;
  }

  return false;
}

void WeightedSumNode::addChild( double weight, BeliefNode* child ) {
  _children.push_back(child);
  _weights.push_back(weight);
}

const std::string& WeightedSumNode::getName() const {
  return _name;
}
