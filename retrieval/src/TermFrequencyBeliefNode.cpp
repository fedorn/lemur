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

#include "indri/TermFrequencyBeliefNode.hpp"

TermFrequencyBeliefNode::TermFrequencyBeliefNode( const std::string& name,
                                                 indri::index::DocListFrequencyIterator& list,
                                                 TopdocsIndex::TopdocsList* topdocs,
                                                 TermScoreFunction& scoreFunction,
                                                 double maximumBackgroundScore,
                                                 double maximumScore )
  :
  _name(name),
  _list(list),
  _topdocs(topdocs),
  _function(scoreFunction),
  _maximumBackgroundScore(maximumBackgroundScore),
  _maximumScore(maximumScore)
{
  _list.startIteration();
}

TermFrequencyBeliefNode::~TermFrequencyBeliefNode() {
  // other lists are deleted by the inference network
  delete _topdocs;
}

const TopdocsIndex::TopdocsList* TermFrequencyBeliefNode::getTopdocsList() const {
  return _topdocs;
}

int TermFrequencyBeliefNode::nextCandidateDocument() {
  const DocumentCount* entry = _list.currentEntry();
  return entry ? entry->document : MAX_INT32;
}

double TermFrequencyBeliefNode::maximumBackgroundScore() {
  return _maximumBackgroundScore;
}

double TermFrequencyBeliefNode::maximumScore() {
  return _maximumScore;
}

const greedy_vector<ScoredExtentResult>& TermFrequencyBeliefNode::score( int documentID, int begin, int end, int documentLength ) {
  assert( begin == 0 && end == documentLength ); // FrequencyListCopier ensures this condition
  const DocumentCount* entry = _list.currentEntry();
  _extents.clear();

  int count = ( entry && entry->document == documentID ) ? entry->count : 0;
  double score = _function.scoreOccurrence( count, documentLength );

  _extents.push_back( ScoredExtentResult( score, documentID, begin, end ) );
  return _extents;
}

bool TermFrequencyBeliefNode::hasMatch( int documentID ) {
  const DocumentCount* entry = _list.currentEntry();
  return ( entry && entry->document == documentID );
}

const std::string& TermFrequencyBeliefNode::getName() const {
  return _name;
}

void TermFrequencyBeliefNode::annotate( Annotator& annotator, int documentID, int begin, int end ) {
  // can't annotate -- don't have position info
}

