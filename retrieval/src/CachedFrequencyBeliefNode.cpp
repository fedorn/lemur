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

#include "indri/CachedFrequencyBeliefNode.hpp"

CachedFrequencyBeliefNode::CachedFrequencyBeliefNode( const std::string& name,
                                                      ListCache::CachedList* list,
                                                      TermScoreFunction& scoreFunction,
                                                      double maximumBackgroundScore,
                                                      double maximumScore )
  :
  _name(name),
  _list(list),
  _function(scoreFunction),
  _maximumBackgroundScore(maximumBackgroundScore),
  _maximumScore(maximumScore)
{
  _iter = _list->entries.begin();
}

int CachedFrequencyBeliefNode::nextCandidateDocument() {
  return _iter < _list->entries.end() ? _iter->document : MAX_INT32;
}

double CachedFrequencyBeliefNode::maximumBackgroundScore() {
  return _maximumBackgroundScore;
}

double CachedFrequencyBeliefNode::maximumScore() {
  return _maximumScore;
}

const greedy_vector<ScoredExtentResult>& CachedFrequencyBeliefNode::score( int documentID, int begin, int end, int documentLength ) {
  assert( begin == 0 && end == documentLength ); // FrequencyListCopier ensures this condition
  const DocumentContextCount* entry = _iter < _list->entries.end() ? _iter : 0;
  _extents.clear();

  int count = 0;
  int contextSize = 0;

  if( entry && entry->document == documentID ) {
    count = entry->count;
    contextSize = entry->contextSize;

  // advance this pointer so we're looking at the next document
  _iter++;
  } else {
    count = 0;
    contextSize = documentLength;
  }

  double score = _function.scoreOccurrence( count, contextSize );

  assert( score <= _maximumScore );
  _extents.push_back( ScoredExtentResult( score, documentID, begin, end ) );

  return _extents;
}

bool CachedFrequencyBeliefNode::hasMatch( int documentID ) {
  return ( _iter < _list->entries.end() && _iter->document == documentID );
}

const std::string& CachedFrequencyBeliefNode::getName() const {
  return _name;
}

void CachedFrequencyBeliefNode::annotate( Annotator& annotator, int documentID, int begin, int end ) {
  // can't annotate -- don't have position info
}

