/*==========================================================================
  Copyright (c) 2004 University of Massachusetts.  All Rights Reserved.

  Use of the Lemur Toolkit for Language Modeling and Information Retrieval
  is subject to the terms of the software license set forth in the LICENSE
  file included with this software, and also available at
  http://www.cs.cmu.edu/~lemur/license.html 
  as well as the conditions below.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  1. Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in
  the documentation and/or other materials provided with the
  distribution.

  3. The names "Indri", "Center for Intelligent Information Retrieval", 
  "CIIR", and "University of Massachusetts" must not be used to
  endorse or promote products derived from this software without
  prior written permission. To obtain permission, contact
  indri-info@ciir.cs.umass.edu.

  4. Products derived from this software may not be called "Indri" nor 
  may "Indri" appear in their names without prior written permission of 
  the University of Massachusetts. To obtain permission, contact 
  indri-info@ciir.cs.umass.edu.

  THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY OF MASSACHUSETTS AND OTHER
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
  BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
  FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
  THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
  DAMAGE.
  ==========================================================================
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
