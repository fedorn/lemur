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
