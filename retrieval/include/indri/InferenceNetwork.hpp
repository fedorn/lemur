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
// InferenceNetwork
//
// 24 February 2004 -- tds
//

#ifndef INDRI_INFERENCENETWORK_HPP
#define INDRI_INFERENCENETWORK_HPP

#include "indri/BeliefNode.hpp"
#include "indri/EvaluatorNode.hpp"
#include "indri/ListIteratorNode.hpp"
#include "indri/TermScoreFunction.hpp"
#include "indri/Repository.hpp"
#include "indri/FieldListIterator.hpp"
#include "indri/DocListFrequencyIterator.hpp"

class InferenceNetwork {
public:
  typedef std::map< std::string, EvaluatorNode::MResults > MAllResults;

  //
  // MAllResults stores results indexed first by node name, then second by the node's 
  // result name.  For instance, to retrieve occurrence counts from a 
  // ContextCountAccumulator named "bd45a", you'd type 'results["bd45a"]["occurrences"]'.
  //

private:
  std::vector<class indri::index::FieldListIterator*> _fieldIterators;
  std::vector<class DocPositionInfoList*> _docIterators;
  std::vector<class indri::index::DocListFrequencyIterator*> _freqIterators;
  std::vector<ListIteratorNode*> _listIteratorNodes;
  std::vector<BeliefNode*> _beliefNodes;
  std::vector<EvaluatorNode*> _evaluators;
  std::vector<EvaluatorNode*> _complexEvaluators;
  std::vector<TermScoreFunction*> _scoreFunctions;
  Repository& _repository;
  MAllResults _results;

  void _moveToDocument( int candidate );
  int _nextCandidateDocument();
  void _evaluateDocument( int document );

public:
  InferenceNetwork( Repository& repository );
  ~InferenceNetwork();

  const std::vector<EvaluatorNode*>& getEvaluators() const;
  const EvaluatorNode* getFirstEvaluator() const;

  void addDocIterator( DocPositionInfoList* posInfoList );
  void addFieldIterator( indri::index::FieldListIterator* fieldIterator );
  void addFrequencyIterator( indri::index::DocListFrequencyIterator* frequencyIterator );
  void addListNode( ListIteratorNode* listNode );
  void addBeliefNode( BeliefNode* beliefNode );
  void addEvaluatorNode( EvaluatorNode* evaluatorNode );
  void addComplexEvaluatorNode( EvaluatorNode* complexEvaluator );
  void addScoreFunction( TermScoreFunction* scoreFunction );
  const MAllResults& evaluate();
};

#endif // INDRI_INFERENCENETWORK_HPP

