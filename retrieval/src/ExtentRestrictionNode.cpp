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
// ExtentRestrictionNode
//
// 6 July 2004 -- tds
//

#include "indri/ExtentRestrictionNode.hpp"
#include "indri/Annotator.hpp"
#include "lemur-compat.hpp"

ExtentRestrictionNode::ExtentRestrictionNode( const std::string& name, BeliefNode* child, ListIteratorNode* field ) :
  _name(name),
  _child(child),
  _field(field)
{
}

int ExtentRestrictionNode::nextCandidateDocument() {
  return _child->nextCandidateDocument();
}

double ExtentRestrictionNode::maximumBackgroundScore() {
  return _child->maximumBackgroundScore();
}

double ExtentRestrictionNode::maximumScore() {
  return _child->maximumScore();
}

const greedy_vector<ScoredExtentResult>& ExtentRestrictionNode::score( int documentID, int begin, int end, int documentLength ) {
  // we're going to run through the field list, etc.
  greedy_vector<Extent>::const_iterator fieldEnd = _field->extents().end();
  greedy_vector<Extent>::const_iterator fieldBegin = _field->extents().begin();
  greedy_vector<Extent>::const_iterator iter;

  _scores.clear();

  for( iter = fieldBegin; iter != fieldEnd; iter++ ) {
    if( iter->end < begin )
      continue; // this one isn't relevant to our cause

    if( end < iter->begin )
      break; // we've passed all the relevant fields

    if( iter->end - iter->begin == 0 )
      continue; // this field has no text in it

    int scoreBegin = lemur_compat::max( iter->begin, begin );
    int scoreEnd = lemur_compat::min( iter->end, end );

    const greedy_vector<ScoredExtentResult>& childResults = _child->score( documentID, scoreBegin, scoreEnd, documentLength );

    for( int i=0; i<childResults.size(); i++ ) {
      ScoredExtentResult result( childResults[i].score, documentID, scoreBegin, scoreEnd );
      _scores.push_back( result );
    }
  }

  return _scores;
}

void ExtentRestrictionNode::annotate( Annotator& annotator, int documentID, int begin, int end ) {
  annotator.add(this, documentID, begin, end);
  // we're going to run through the field list, etc.
  greedy_vector<Extent>::const_iterator fieldEnd = _field->extents().end();
  greedy_vector<Extent>::const_iterator fieldBegin = _field->extents().begin();
  greedy_vector<Extent>::const_iterator iter;

  for( iter = fieldBegin; iter != fieldEnd; iter++ ) {
    if( iter->end < begin )
      continue; // this one isn't relevant to our cause

    if( end < iter->begin )
      break; // we've passed all the relevant fields

    if( iter->end - iter->begin == 0 )
      continue; // this field has no text in it

    int scoreBegin = lemur_compat::max( iter->begin, begin );
    int scoreEnd = lemur_compat::min( iter->end, end );

    _child->annotate( annotator, documentID, scoreBegin, scoreEnd );
  }
}

bool ExtentRestrictionNode::hasMatch( int documentID ) {
  return _child->hasMatch( documentID );
}


const std::string& ExtentRestrictionNode::getName() const {
  return _name;
}

