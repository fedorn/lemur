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
// OrNode
//
// 6 July 2004 -- tds
//

#include "indri/OrNode.hpp"
#include "lemur-compat.hpp"
#include "indri/ScoredExtentResult.hpp"
#include "indri/Annotator.hpp"

OrNode::OrNode( const std::string& name ) :
  _name(name)
{
}

OrNode::OrNode( const std::string& name, const std::vector<BeliefNode*>& children ) :
  _children( children ),
  _name( name )
{
}

const greedy_vector<ScoredExtentResult>& OrNode::score( int documentID, int begin, int end, int documentLength ) {
  double notScore = 1;

  for( unsigned int i=0; i<_children.size(); i++ ) {
    const greedy_vector<ScoredExtentResult>& childResults = _children[i]->score( documentID, begin, end, documentLength );

    for( unsigned int j=0; j<childResults.size(); j++ ) {
      notScore *= 1. - exp( childResults[j].score );
    }
  }
  
  double score = log( 1. - notScore );
  _scores.clear();
  _scores.push_back( ScoredExtentResult( score, documentID, begin, end ) );

  return _scores;
}

void OrNode::annotate( class Annotator& annotator, int documentID, int begin, int end ) {
  annotator.add(this, documentID, begin, end);

  for( unsigned int i=0; i<_children.size(); i++ ) {
    _children[i]->annotate( annotator, documentID, begin, end );
  }
}

double OrNode::maximumScore() {
  double notScore = 1;

  for( unsigned int i=0; i<_children.size(); i++ ) {
    notScore *= 1. - exp( _children[i]->maximumBackgroundScore() );
  }

  return log( 1. - notScore );
}

double OrNode::maximumBackgroundScore() {
  double notScore = 1;

  for( unsigned int i=0; i<_children.size(); i++ ) {
    notScore *= 1. - exp( _children[i]->maximumScore() );
  }

  return log( 1. - notScore );
}

bool OrNode::hasMatch( int documentID ) {
  for( unsigned int i=0; i<_children.size(); i++ ) {
    if( _children[i]->hasMatch( documentID ) )
      return true;
  }

  return false;
}

int OrNode::nextCandidateDocument() {
  int nextCandidate = MAX_INT32;
  
  for( unsigned int i=0; i<_children.size(); i++ ) {
    nextCandidate = lemur_compat::min<int>( nextCandidate, _children[i]->nextCandidateDocument() );
  }

  return nextCandidate;
}

const std::string& OrNode::getName() const {
  return _name;
}

