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
// InferenceNetwork
//
// 24 February 2004 -- tds
//

#include "indri/InferenceNetwork.hpp"
#include "indri/SkippingCapableNode.hpp"
#include "indri/DocPositionInfoList.hpp"
#include "indri/FieldListIterator.hpp"
#include "indri/IndriIndex.hpp"

void InferenceNetwork::_moveToDocument( int candidate ) {
  // move all document iterators
  std::vector<DocPositionInfoList*>::iterator iter;
  for( iter = _docIterators.begin(); iter != _docIterators.end(); iter++ ) {
    (*iter)->nextEntry( candidate );
  }

  // move all frequency iterators
  std::vector<indri::index::DocListFrequencyIterator*>::iterator fqiter;
  for( fqiter = _freqIterators.begin(); fqiter != _freqIterators.end(); fqiter++ ) {
    (*fqiter)->nextEntry( candidate );
  }

  // move all field iterators
  std::vector<indri::index::FieldListIterator*>::iterator fiter;
  for( fiter = _fieldIterators.begin(); fiter != _fieldIterators.end(); fiter++ ) {
    (*fiter)->nextEntry( candidate );
  }

  // prepare all extent iterator nodes
  std::vector<ListIteratorNode*>::iterator diter;
  for( diter = _listIteratorNodes.begin(); diter != _listIteratorNodes.end(); diter++ ) {
    (*diter)->prepare( candidate );
  }
}

int InferenceNetwork::_nextCandidateDocument() {
  int candidate = MAX_INT32;

  for( unsigned int i=0; i<_complexEvaluators.size(); i++ ) {
    candidate = lemur_compat::min( candidate, _complexEvaluators[i]->nextCandidateDocument() );
  }
  
  return candidate;
}

void InferenceNetwork::_evaluateDocument( int document ) {
  int candidateLength = _repository.index()->docLength( document );

  for( unsigned int i=0; i<_complexEvaluators.size(); i++ ) {
    _complexEvaluators[i]->evaluate( document, candidateLength );
  }
}

InferenceNetwork::InferenceNetwork( Repository& repository ) :
  _repository(repository)
{
}

InferenceNetwork::~InferenceNetwork() {
  delete_vector_contents<indri::index::FieldListIterator*>( _fieldIterators );
  delete_vector_contents<DocPositionInfoList*>( _docIterators );
  delete_vector_contents<indri::index::DocListFrequencyIterator*>( _freqIterators );
  delete_vector_contents<ListIteratorNode*>( _listIteratorNodes );
  delete_vector_contents<BeliefNode*>( _beliefNodes );
  delete_vector_contents<TermScoreFunction*>( _scoreFunctions );
  delete_vector_contents<EvaluatorNode*>( _evaluators );
}

void InferenceNetwork::addDocIterator( DocPositionInfoList* posInfoList ) {
  _docIterators.push_back( posInfoList );
}

void InferenceNetwork::addFrequencyIterator( indri::index::DocListFrequencyIterator* freqList ) {
  _freqIterators.push_back( freqList );
}

void InferenceNetwork::addFieldIterator( indri::index::FieldListIterator* fieldIterator ) {
  _fieldIterators.push_back( fieldIterator );
}

void InferenceNetwork::addListNode( ListIteratorNode* listNode ) {
  _listIteratorNodes.push_back( listNode );
}

void InferenceNetwork::addBeliefNode( BeliefNode* beliefNode ) {
  _beliefNodes.push_back( beliefNode );
}

void InferenceNetwork::addEvaluatorNode( EvaluatorNode* evaluatorNode ) {
  _evaluators.push_back( evaluatorNode );
}

void InferenceNetwork::addScoreFunction( TermScoreFunction* scoreFunction ) {
  _scoreFunctions.push_back( scoreFunction );
}

void InferenceNetwork::addComplexEvaluatorNode( EvaluatorNode* complexEvaluator ) {
  _complexEvaluators.push_back( complexEvaluator );
}

const std::vector<EvaluatorNode*>& InferenceNetwork::getEvaluators() const {
  return _evaluators;
}

const EvaluatorNode* InferenceNetwork::getFirstEvaluator() const {
  if( _evaluators.size() )
    return _evaluators[0];
  return 0;
}

const InferenceNetwork::MAllResults& InferenceNetwork::evaluate() {
  int lastCandidate = MAX_INT32;
  int collectionSize = _repository.index()->docCount();
  int scoredDocuments = 0;
  int candidate = 0;

  // don't need to do anything unless there are some
  // evaluators in the network that need full evaluation

  if( _complexEvaluators.size() ) {
    while(1) {
      // ask the root node for a candidate document
      // this asks the whole inference network for the
      // first document that might possibly produce a
      // usable (above the max score threshold) score
      candidate = _nextCandidateDocument();

      // if candidate is MAX_INT32, we're done
      if( candidate == MAX_INT32 || candidate > collectionSize ) {
        break;
      }

      // move all the doc info lists to this new document
      // in preparation for scoring
      if( candidate != lastCandidate ) {
        _moveToDocument( candidate );
      }

      // ask all the evaluators to evaluate this document
      _evaluateDocument( candidate );
      scoredDocuments++;

      // if that was the last document, we can quit now
      if( candidate+1 > collectionSize )
        break;

      // move all candidate iterators to candidate+1
      _moveToDocument( candidate+1 );
      lastCandidate = candidate+1;
    }
  }

  _results.clear();
  for( unsigned int i=0; i<_evaluators.size(); i++ ) {
    _results[ _evaluators[i]->getName() ] = _evaluators[i]->getResults();
  }

  //std::cout << "Scored Documents: " << scoredDocuments << std::endl;

  return _results;
}

