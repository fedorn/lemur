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
// FilterNode
//
// 21 July 2004 -- tds
//
// Restricts evaluation to only a subset of the documents
// in the collection.
//

#ifndef INDRI_FILTERNODE_HPP
#define INDRI_FILTERNODE_HPP

class FilterNode : public BeliefNode {
private:
  BeliefNode* _belief;
  std::vector<int> _documents;
  std::string _name;
  int _index;

public:
  FilterNode( const std::string& name, BeliefNode* child, const std::vector<int>& documents )
    :
    _documents(documents)
  {
    _name = name;
    _belief = child;
    _index = 0;
    std::sort( _documents.begin(), _documents.end() );
  }

  int nextCandidateDocument() {
    int childNext = _belief->nextCandidateDocument();

    while( _index < _documents.size() && _documents[_index] < childNext )
      _index++;
    
    if( _index == _documents.size() )
      return MAX_INT32;

    return _documents[_index];
  }

  void annotate( Annotator& annotator, int documentID, int begin, int end ) {
    return _belief->annotate( annotator, documentID, begin, end );
  }

  const greedy_vector<ScoredExtentResult>& score( int documentID, int begin, int end, int documentLength ) {
    return _belief->score( documentID, begin, end, documentLength );
  }

  double maximumScore() {
    return _belief->maximumScore();
  }

  double maximumBackgroundScore() {
    return _belief->maximumBackgroundScore();
  }

  bool hasMatch( int documentID ) {
    return _belief->hasMatch( documentID );
  }

  const std::string& getName() const {
    return _name;
  }
};

#endif // INDRI_FILTERNODE_HPP

