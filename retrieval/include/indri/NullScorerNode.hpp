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
// NullScorerNode
//
// 10 March 2004 -- tds
//

#ifndef INDRI_NULLSCORERNODE_HPP
#define INDRI_NULLSCORERNODE_HPP

#include "indri/greedy_vector"
#include "indri/ScoredExtentResult.hpp"
#include "indri/TermScoreFunction.hpp"
#include "indri/BeliefNode.hpp"

class NullScorerNode : public BeliefNode {
private:
  TermScoreFunction& _scoreFunction;
  greedy_vector<ScoredExtentResult> _scores;
  std::string _name;
  double _maximumBackgroundScore;
  double _maximumScore;

public:
  NullScorerNode( const std::string& name, TermScoreFunction& scoreFunction );

  int nextCandidateDocument();
  double maximumScore();
  double maximumBackgroundScore();
  bool hasMatch( int documentID );
  const greedy_vector<ScoredExtentResult>& score( int documentID, int begin, int end, int documentLength );
  void annotate( class Annotator& annotator, int documentID, int begin, int end );
  const std::string& getName() const;
};

#endif // INDRI_NULLSCORERNODE_HPP

