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
