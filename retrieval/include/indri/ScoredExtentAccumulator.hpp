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

