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
// TermFrequencyBeliefNode
//
// 25 August 2004 -- tds
//

#ifndef INDRI_TERMFREQUENCYBELIEFNODE_HPP
#define INDRI_TERMFREQUENCYBELIEFNODE_HPP

#include "indri/Annotator.hpp"
#include "indri/ScoredExtentResult.hpp"
#include <string>
#include "indri/TermScoreFunction.hpp"
#include "indri/ListBeliefNode.hpp"
#include "indri/DocListFrequencyIterator.hpp"
#include "indri/TopdocsIndex.hpp"

class TermFrequencyBeliefNode : public BeliefNode {
private:
  TermScoreFunction& _function;
  greedy_vector<ScoredExtentResult> _extents;
  indri::index::DocListFrequencyIterator& _list;
  TopdocsIndex::TopdocsList* _topdocs;
  double _maximumBackgroundScore;
  double _maximumScore;
  std::string _name;

public:
  TermFrequencyBeliefNode( const std::string& name,
    indri::index::DocListFrequencyIterator& list,
    TopdocsIndex::TopdocsList* topdocs,
    TermScoreFunction& scoreFunction,
    double maximumBackgroundScore,
    double maximumScore );

  ~TermFrequencyBeliefNode();

  const TopdocsIndex::TopdocsList* getTopdocsList() const;
  int nextCandidateDocument();
  double maximumBackgroundScore();
  double maximumScore();
  const greedy_vector<ScoredExtentResult>& score( int documentID, int begin, int end, int documentLength );
  void annotate( class Annotator& annotator, int documentID, int begin, int end );
  bool hasMatch( int documentID );
  const std::string& getName() const;
};

#endif // INDRI_TERMFREQUENCYBELIEFNODE_HPP


