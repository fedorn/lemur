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
// ScoredExtentAccumulator
//
// 24 February 2004 -- tds
//

#ifndef INDRI_SCOREDEXTENTACCUMULATOR_HPP
#define INDRI_SCOREDEXTENTACCUMULATOR_HPP

#include "indri/SkippingCapableNode.hpp"

class ScoredExtentAccumulator : public EvaluatorNode {
private:
  BeliefNode* _belief;
  SkippingCapableNode* _skipping;
  std::priority_queue<ScoredExtentResult> _scores;
  std::vector<ScoredExtentResult> _finalScores;
  int _resultsRequested;
  std::string _name;
  EvaluatorNode::MResults _results;

public:
  ScoredExtentAccumulator( std::string name, BeliefNode* belief, int resultsRequested = -1 ) :
    _belief(belief),
    _resultsRequested(resultsRequested),
    _name(name),
    _skipping(0)
  {
    _skipping = dynamic_cast<SkippingCapableNode*>(belief);
  }

  void evaluate( int documentID, int documentLength ) {
    if( _belief->hasMatch( documentID ) ) {
      const greedy_vector<ScoredExtentResult>& documentScores = _belief->score( documentID, 0, documentLength, documentLength );

      for( unsigned int i=0; i<documentScores.size(); i++ ) {
        _scores.push( documentScores[i] );
      }

      while( int(_scores.size()) > _resultsRequested && _resultsRequested > 0 ) {
        _scores.pop();
        if( _skipping ) {
          double worstScore = _scores.top().score;
          _skipping->setThreshold( worstScore - DBL_MIN );
        }
      }
    }
  }
  
  int nextCandidateDocument() {
    return _belief->nextCandidateDocument();
  }

  const std::string& getName() const {
    return _name;
  }

  const EvaluatorNode::MResults& getResults() {
    _results.clear();

    if( !_scores.size() )
      return _results;
    
    // making a copy of the heap here so the method can be const
    std::priority_queue<ScoredExtentResult> heapCopy = _scores;
    std::vector<ScoredExtentResult>& scoreVec = _results["scores"];

    // puts scores into the vector in descending order
    scoreVec.reserve( heapCopy.size() );
    for( int i=(int)heapCopy.size()-1; i>=0; i-- ) {
      scoreVec.push_back( heapCopy.top() );
      heapCopy.pop();
    }

    return _results;
  }
};

#endif // INDRI_SCOREDEXTENTACCUMULATOR_HPP

