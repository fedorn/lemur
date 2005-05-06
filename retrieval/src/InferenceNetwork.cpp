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

#include "indri/InferenceNetwork.hpp"
#include "indri/SkippingCapableNode.hpp"

#include "indri/DocListIterator.hpp"
#include "indri/DocExtentListIterator.hpp"

#include "indri/ScopedLock.hpp"
#include "indri/Thread.hpp"

const static int CLOSE_ITERATOR_RANGE = 5000;

//
// _moveDocListIterators
//

inline void indri::infnet::InferenceNetwork::_moveDocListIterators( int candidate ) {
  if( _closeIterators.size() && candidate < _closeIteratorBound ) {
    // we're close to this document, so only advance the iterators that are close
    indri::utility::greedy_vector<indri::index::DocListIterator*>::iterator iter;

    for( iter = _closeIterators.begin(); iter != _closeIterators.end(); iter++ ) {
      (*iter)->nextEntry( candidate );
    }
  } else {
    std::vector<indri::index::DocListIterator*>::iterator iter;

    _closeIterators.clear();
    _closeIteratorBound = candidate + CLOSE_ITERATOR_RANGE;

    for( iter = _docIterators.begin(); iter != _docIterators.end(); iter++ ) {
      if( *iter ) {
        (*iter)->nextEntry( candidate );

        if( !(*iter)->finished() &&
            (*iter)->currentEntry()->document < _closeIteratorBound ) {
          _closeIterators.push_back( *iter );
        }
      }
    }
  }
}

//
// _moveToDocument
//

void indri::infnet::InferenceNetwork::_moveToDocument( int candidate ) {
  // move all document iterators
  _moveDocListIterators( candidate );

  // move all field iterators
  std::vector<indri::index::DocExtentListIterator*>::iterator fiter;
  for( fiter = _fieldIterators.begin(); fiter != _fieldIterators.end(); fiter++ ) {
    if( *fiter )
      (*fiter)->nextEntry( candidate );
  }

  // prepare all extent iterator nodes
  std::vector<ListIteratorNode*>::iterator diter;
  for( diter = _listIteratorNodes.begin(); diter != _listIteratorNodes.end(); diter++ ) {
    (*diter)->prepare( candidate );
  }
}

//
// _indexFinished
//

void indri::infnet::InferenceNetwork::_indexFinished( indri::index::Index& index ) {
  // doc iterators
  indri::utility::delete_vector_contents<indri::index::DocListIterator*>( _docIterators );

  // field iterators
  indri::utility::delete_vector_contents<indri::index::DocExtentListIterator*>( _fieldIterators );
}

//
// _indexChanged
//

void indri::infnet::InferenceNetwork::_indexChanged( indri::index::Index& index ) {
  _closeIterators.clear();
  _closeIteratorBound = -1;

  // doc iterators
  for( int i=0; i<_termNames.size(); i++ ) {
    indri::index::DocListIterator* iterator = index.docListIterator( _termNames[i] );
    if( iterator )
      iterator->startIteration();

    _docIterators.push_back( iterator );
  }

  // field iterators
  for( int i=0; i<_fieldNames.size(); i++ ) {
    indri::index::DocExtentListIterator* iterator = index.fieldListIterator( _fieldNames[i] );
    if( iterator )
      iterator->startIteration();

    _fieldIterators.push_back( iterator );
  }

  // extent iterator nodes
  std::vector<ListIteratorNode*>::iterator diter;
  for( diter = _listIteratorNodes.begin(); diter != _listIteratorNodes.end(); diter++ ) {
    (*diter)->indexChanged( index );
  }

  // belief nodes
  std::vector<BeliefNode*>::iterator biter;
  for( biter = _beliefNodes.begin(); biter != _beliefNodes.end(); biter++ ) {
    (*biter)->indexChanged( index );
  }

  // evaluator nodes
  std::vector<indri::infnet::EvaluatorNode*>::iterator eiter;
  for( eiter = _evaluators.begin(); eiter != _evaluators.end(); eiter++ ) {
    (*eiter)->indexChanged( index );
  }
}

//
// _nextCandidateDocument
//

int indri::infnet::InferenceNetwork::_nextCandidateDocument( indri::index::DeletedDocumentList::read_transaction* deleted ) {
  int candidate = MAX_INT32;

  for( unsigned int i=0; i<_complexEvaluators.size(); i++ ) {
    candidate = lemur_compat::min( candidate, _complexEvaluators[i]->nextCandidateDocument() );
  }

  return deleted->nextCandidateDocument( candidate );
}

//
// _evaluateDocument
//

void indri::infnet::InferenceNetwork::_evaluateDocument( indri::index::Index& index, int document ) {
  int candidateLength = index.documentLength( document );

  for( unsigned int i=0; i<_complexEvaluators.size(); i++ ) {
    _complexEvaluators[i]->evaluate( document, candidateLength );
  }
}

//
// InferenceNetwork constructor
//

indri::infnet::InferenceNetwork::InferenceNetwork( indri::collection::Repository& repository ) :
  _repository(repository),
  _closeIteratorBound(-1)
{
}

//
// InferenceNetwork destructor
//

indri::infnet::InferenceNetwork::~InferenceNetwork() {
  indri::utility::delete_vector_contents<indri::index::DocExtentListIterator*>( _fieldIterators );
  indri::utility::delete_vector_contents<indri::index::DocListIterator*>( _docIterators );
  indri::utility::delete_vector_contents<indri::infnet::ListIteratorNode*>( _listIteratorNodes );
  indri::utility::delete_vector_contents<indri::infnet::BeliefNode*>( _beliefNodes );
  indri::utility::delete_vector_contents<indri::query::TermScoreFunction*>( _scoreFunctions );
  indri::utility::delete_vector_contents<indri::infnet::EvaluatorNode*>( _evaluators );
}

indri::index::DocListIterator* indri::infnet::InferenceNetwork::getDocIterator( int index ) {
  return _docIterators[index];
}

indri::index::DocExtentListIterator* indri::infnet::InferenceNetwork::getFieldIterator( int index ) {
  return _fieldIterators[index];
}

int indri::infnet::InferenceNetwork::addDocIterator( const std::string& termName ) {
  _termNames.push_back( termName );
  return _termNames.size()-1;
}

int indri::infnet::InferenceNetwork::addFieldIterator( const std::string& fieldName ) {
  _fieldNames.push_back( fieldName );
  return _fieldNames.size()-1;
}

void indri::infnet::InferenceNetwork::addListNode( indri::infnet::ListIteratorNode* listNode ) {
  _listIteratorNodes.push_back( listNode );
}

void indri::infnet::InferenceNetwork::addBeliefNode( indri::infnet::BeliefNode* beliefNode ) {
  _beliefNodes.push_back( beliefNode );
}

void indri::infnet::InferenceNetwork::addEvaluatorNode( EvaluatorNode* evaluatorNode ) {
  _evaluators.push_back( evaluatorNode );
}

void indri::infnet::InferenceNetwork::addScoreFunction( indri::query::TermScoreFunction* scoreFunction ) {
  _scoreFunctions.push_back( scoreFunction );
}

void indri::infnet::InferenceNetwork::addComplexEvaluatorNode( indri::infnet::EvaluatorNode* complexEvaluator ) {
  _complexEvaluators.push_back( complexEvaluator );
}

const std::vector<indri::infnet::EvaluatorNode*>& indri::infnet::InferenceNetwork::getEvaluators() const {
  return _evaluators;
}

void indri::infnet::InferenceNetwork::_evaluateIndex( indri::index::Index& index ) {
  indri::index::DeletedDocumentList::read_transaction* deleted = _repository.deletedList().getReadTransaction();

  int lastCandidate = MAX_INT32;
  int collectionSize = index.documentBase() + index.documentCount();
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
      candidate = _nextCandidateDocument( deleted );
      assert( candidate >= index.documentBase() );

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
      _evaluateDocument( index, candidate );
      scoredDocuments++;

      // if that was the last document, we can quit now
      if( candidate+1 > collectionSize )
        break;

      // move all candidate iterators to candidate+1
      _moveToDocument( candidate+1 );
      lastCandidate = candidate+1;
      assert( candidate >= index.documentBase() );
    }
  }

  delete deleted;
}

//
// evaluate
//

const indri::infnet::InferenceNetwork::MAllResults& indri::infnet::InferenceNetwork::evaluate() {
  // count this query occurrence
  _repository.countQuery();

  // fetch the current index state
  indri::collection::Repository::index_state indexes = _repository.indexes();
  
  for( int i=0; i<indexes->size(); i++ ) {
    indri::index::Index& index = *(*indexes)[i];
    indri::thread::ScopedLock iterators( index.iteratorLock() );

    indri::thread::ScopedLock statistics( index.statisticsLock() );
    _indexChanged( index );
    statistics.unlock();

    // evaluate query against the index
    _evaluateIndex( index );

    // remove all the iterators
    _indexFinished( index );
  }

  _results.clear();
  for( unsigned int i=0; i<_evaluators.size(); i++ ) {
    _results[ _evaluators[i]->getName() ] = _evaluators[i]->getResults();
  }

  return _results;
}
