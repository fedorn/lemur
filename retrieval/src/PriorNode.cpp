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
// PriorNode
//
// 6 July 2004 -- tds
//

#include "indri/PriorNode.hpp"
#include "indri/greedy_vector"
#include <math.h>
#include "indri/Annotator.hpp"

PriorNode::PriorNode( const std::string& name, FieldIteratorNode* field, const std::map<int, indri::lang::PriorNode::tuple_type>& table ) :
  _field(field),
  _table(table),
  _name(name)
{
}

PriorNode::~PriorNode() {
}

int PriorNode::nextCandidateDocument() {
  return _field->nextCandidateDocument();
}

bool PriorNode::hasMatch( int documentID ) {
  // priors don't match; they only boost or cut
  return false;
}

const greedy_vector<ScoredExtentResult>& PriorNode::score( int documentID, int begin, int end, int documentLength ) {
  int key;
  double score;

  _scores.clear();

  if( _field ) {
    if( _field->numbers().size() ) {
      // use the first number in this document as our key
      key = _field->numbers()[0];
    } else {
      // no match, return 0 probability
      score = -DBL_MAX;
      _scores.push_back( ScoredExtentResult( score, documentID, begin, end ) );
      return _scores;
    }
  } else {
    // use the document ID as the key
    key = documentID;
  }
  
  std::map<int, indri::lang::PriorNode::tuple_type>::iterator iter;
  iter = _table.lower_bound( key );

  // check to see if the key is within the range of this object
  if( iter != _table.end() &&
      (*iter).second.begin <= key &&
      (*iter).second.end >= key ) {
    score = log( (*iter).second.score );
  } else {
    // if it didn't match, return 0 probability
    score = -DBL_MAX;
  }

  _scores.push_back( ScoredExtentResult( score, documentID, begin, end ) );
  return _scores;
}

void PriorNode::annotate( class Annotator& annotator, int documentID, int begin, int end ) {
  score( documentID, begin, end, end );
  
  for( unsigned int i=0; i<_scores.size(); i++ ) {
    annotator.add( this, documentID, _scores[i].begin, _scores[i].end ); 
  }
}

double PriorNode::maximumScore() {
  return INDRI_HUGE_SCORE;
}

double PriorNode::maximumBackgroundScore() {
  return INDRI_TINY_SCORE;
}

const std::string& PriorNode::getName() const {
  return _name;
}
