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
// CachedFrequencyBeliefNode
//
// 24 September 2004 -- tds
//

#ifndef INDRI_CACHEDFREQUENCYBELIEFNODE_HPP
#define INDRI_CACHEDFREQUENCYBELIEFNODE_HPP

#include "indri/Annotator.hpp"
#include "indri/ScoredExtentResult.hpp"
#include <string>
#include "indri/TermScoreFunction.hpp"
#include "indri/ListBeliefNode.hpp"
#include "indri/ListCache.hpp"

class CachedFrequencyBeliefNode : public BeliefNode {
private:
  TermScoreFunction& _function;
  greedy_vector<ScoredExtentResult> _extents;
  greedy_vector<DocumentContextCount>::iterator _iter;
  ListCache::CachedList* _list;
  double _maximumBackgroundScore;
  double _maximumScore;
  std::string _name;

public:
  CachedFrequencyBeliefNode( const std::string& name,
    ListCache::CachedList* list,
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

#endif // INDRI_CACHEDFREQUENCYBELIEFNODE_HPP

