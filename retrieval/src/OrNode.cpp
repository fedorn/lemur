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

