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

Annotator::Annotator( const std::string& name, BeliefNode* belief )
  :
  _name(name),
  _belief(belief)
{
}

void Annotator::add( InferenceNetworkNode* node, int documentID, int begin, int end ) {
  ScoredExtentResult a;

  a.document = documentID;
  a.begin = begin;
  a.end = end;
  a.score = 0;

  _annotations[node->getName()].push_back(a);
}

void Annotator::addMatches( greedy_vector<Extent>& extents, InferenceNetworkNode* node, int documentID, int begin, int end ) {
  for( size_t i=0; i<extents.size(); i++ ) {
    if( begin > extents[i].begin )
      continue;

    if( end < extents[i].end )
      continue;

    add( node, documentID, extents[i].begin, extents[i].end );
  }
}

void Annotator::evaluate( int documentID, int documentLength ) {
  _belief->annotate( *this, documentID, 0, documentLength );
}

int Annotator::nextCandidateDocument() {
  return _belief->nextCandidateDocument();
}

EvaluatorNode::MResults& Annotator::getResults() {
  return _annotations;
}

const std::string& Annotator::getName() const {
  return _name;
}

const EvaluatorNode::MResults& Annotator::getResults() const {
  return _annotations;
}
