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
// ListBeliefNode
//
// 26 January 2004 - tds
//

#ifndef INDRI_LISTBELIEFNODE_HPP
#define INDRI_LISTBELIEFNODE_HPP

#include "indri/BeliefNode.hpp"
#include "indri/TermScoreFunction.hpp"
#include "indri/greedy_vector"
#include "indri/ListIteratorNode.hpp"
#include "indri/ScoredExtentResult.hpp"

class ListBeliefNode : public BeliefNode {
private:
  ListIteratorNode& _list;
  ListIteratorNode* _context;
  ListIteratorNode* _raw;
  double _maximumScore;
  double _maximumBackgroundScore;
  TermScoreFunction& _scoreFunction;
  greedy_vector<ScoredExtentResult> _scores;
  std::string _name;
  bool _documentSmoothing;

private:
  // computes the length of the scored context
  inline int _contextLength( int begin, int end );
  inline int _contextOccurrences( int begin, int end );
  inline int _documentOccurrences();

public:
  ListBeliefNode( const std::string& name,
      ListIteratorNode& child,
      ListIteratorNode* context,
      ListIteratorNode* raw,
      TermScoreFunction& scoreFunction,
      double maximumBackgroundScore,
      double maximumScore );

  int nextCandidateDocument();
  double maximumBackgroundScore();
  double maximumScore();
  const greedy_vector<ScoredExtentResult>& score( int documentID, int begin, int end, int documentLength );
  void annotate( class Annotator& annotator, int documentID, int begin, int end );
  bool hasMatch( int documentID );
  const std::string& getName() const;
};

#endif // INDRI_TERMNODE_HPP
