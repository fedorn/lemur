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
// ContextSimpleCountCollectorCopier
//
// 5 March 2004 -- tds
//
// This copier uses a IndriIndex to extract context
// counts for certain simple subgraphs.  It can compute
// counts for the following types of expressions:
//
//   dog
//   <dog cat>
//   dog.title
//   <dog cat>.title
//   dog.(title)
//   <dog cat>.(title)
//   
// Notably, it is unable to compute counts when more than
// one field is involved.
//

#ifndef INDRI_CONTEXTSIMPLECOUNTCOLLECTORCOPIER_HPP
#define INDRI_CONTEXTSIMPLECOUNTCOLLECTORCOPIER_HPP

#include "indri/QuerySpec.hpp"
#include "indri/Copier.hpp"
#include "indri/delete_range.hpp"
#include "indri/Repository.hpp"
#include "indri/IndriIndex.hpp"
#include "indri/ListCache.hpp"

class ContextSimpleCountCollectorCopier : public indri::lang::Copier {
private:
  std::vector<indri::lang::Node*> _newNodes;
  Repository& _repository;
  IndriIndex& _index;
  ListCache& _cache;

  class SubtreeWalker : public indri::lang::Walker {
  private:
    bool _computable;
    int _extentOrs;
    bool _hasContext;

    std::vector<indri::lang::IndexTerm*> _terms;
    indri::lang::Field* _field;

  public:
    SubtreeWalker() :
      _computable(true),
      _extentOrs(0),
      _field(0)
    {
    }

    bool isComputable() {
      return _computable && _terms.size();
    }

    std::vector<indri::lang::IndexTerm*>& getTerms() {
      return _terms;
    }

    indri::lang::Field* getField() {
      return _field;
    }

    bool hasContext() const {
      return _hasContext;
    }

    void defaultBefore( indri::lang::Node* node ) {
      // this means that we're seeing some node type that
      // we aren't otherwise trapping--that means this subtree
      // is surely not precomputable
      _computable = false;
    }

    void before( indri::lang::ContextCounterNode* contextNode ) {
      // if the context node has a context, then it must have a field in the context
      // if we find more than one field, we say this isn't computable.  Therefore, if
      // this subtree is computable and it has a context, the single field must be in the context.
      _hasContext = contextNode->getContext() ? true : false;
    }
    
    void before( indri::lang::ExtentOr* extentOrNode ) {
      _extentOrs++;
    }

    void after( indri::lang::ExtentOr* extentOrNode ) {
      _extentOrs--;
    }

    void before( indri::lang::ExtentAnd* extentAndNode ) {
      // we definitely can't deal with any "true" extentAnds
      // however, if this is just an and wrapper around a single
      // field, we won't let it fool us
      if( extentAndNode->getChildren().size() > 1 )
        _computable = false;
    }

    void before( indri::lang::Field* fieldNode ) {
      if( _extentOrs || _field ) {
        // fields can't be or-ed together; only terms can (_extentOr)
        // If we already saw a field, then this one proves that the tree isn't computable (_field)
        _computable = false;
      }

      _field = fieldNode;
    }

    void before( indri::lang::IndexTerm* termNode ) {
      _terms.push_back(termNode);
    }

    void before( indri::lang::ExtentInside* insideNode ) {
      // ignore this; the other checks should catch any bad trees
      // without having to worry about checking here
    }
  };

  void _computeRestrictedCounts( indri::lang::ContextCounterNode* contextNode, SubtreeWalker& subtree ) {
    UINT64 occurrences = 0;
    UINT64 size;

    assert( subtree.isComputable() );
    assert( subtree.getField() );

    // get the fieldID
    int fieldID = _index.field( subtree.getField()->getFieldName().c_str() );

    if( subtree.hasContext() ) {
      size = _index.fieldTermCount( fieldID );
    } else {
      size = _index.termCount();
    }

    std::vector<indri::lang::IndexTerm*>& terms = subtree.getTerms();
    for( unsigned int i=0; i<terms.size(); i++ ) {
      std::string processed = terms[i]->getText();
      if( terms[i]->getStemmed() == false )
        processed = _repository.processTerm( terms[i]->getText() );

      if( processed.length() != 0 ) {
        int termID = _index.term( processed.c_str() );
        occurrences += _index.fieldTermCount( fieldID, termID );
      }
    }

    contextNode->setCounts( occurrences, size );
  }

  void _computeUnrestrictedCounts( indri::lang::ContextCounterNode* contextNode, SubtreeWalker& subtree ) {
    assert( subtree.isComputable() );
    assert( !subtree.getField() );

    UINT64 occurrences = 0;
    UINT64 size = 0;

    UINT64 maxOccurrences = 0;
    UINT64 maxContextSize = 0;
    UINT64 minContextSize = MAX_INT64;
    double maxDocumentFraction = 0;

    std::vector<indri::lang::IndexTerm*>& terms = subtree.getTerms();
    for( unsigned int i=0; i<terms.size(); i++ ) {
      std::string processed = terms[i]->getText();
      if( terms[i]->getStemmed() == false )
        processed = _repository.processTerm( terms[i]->getText() );

      if( processed.length() != 0 ) {
        int termID = _index.term( processed.c_str() );
        occurrences += _index.termCount( termID );

        maxOccurrences += _index.termMaxDocumentFrequency( termID );
        minContextSize = lemur_compat::min<UINT64>( _index.termMinDocumentLength( termID ), minContextSize );

        maxDocumentFraction += _index.termMaxDocumentFraction( termID );
        maxDocumentFraction = lemur_compat::min<double>( 1.0, maxDocumentFraction );
      }
    }

    maxContextSize = _index.maxDocumentLength();

    size = _index.termCount();
    contextNode->setCounts( occurrences, size, maxOccurrences, minContextSize, maxContextSize, maxDocumentFraction );
  }

  void _computeCounts( indri::lang::ContextCounterNode* contextNode, SubtreeWalker& subtree ) {
    assert( subtree.isComputable() );

    if( subtree.getField() ) {
      _computeRestrictedCounts( contextNode, subtree );
    } else {
      _computeUnrestrictedCounts( contextNode, subtree );
    }
  }

  void _setCountsFromList( indri::lang::ContextCounterNode* contextNode, ListCache::CachedList* list ) {
    contextNode->setCounts( list->occurrences,
                            list->contextSize, 
                            list->maximumOccurrences,
                            list->minimumContextSize, 
                            list->maximumContextSize,
                            list->maximumContextFraction );
    contextNode->setRawExtent( 0 );
    contextNode->setContext( 0 );
  }

public:
  ContextSimpleCountCollectorCopier( Repository& repository, ListCache& cache ) :
    _repository(repository),
    _index(*repository.index()),
    _cache(cache)
  {
  }

  ~ContextSimpleCountCollectorCopier() {
    delete_vector_contents<indri::lang::Node*>( _newNodes );
  }

  indri::lang::Node* defaultAfter( indri::lang::Node* oldNode, indri::lang::Node* newNode ) {
    _newNodes.push_back( newNode );
    return newNode;
  }

  indri::lang::Node* after( indri::lang::ContextCounterNode* contextNode, indri::lang::ContextCounterNode* newNode ) {
    ListCache::CachedList* list = _cache.find( newNode->getRawExtent(), newNode->getContext() );

    if( list ) {
      _setCountsFromList( newNode, list );
    } else {
      // first, walk the subtree to find out if it's computable
      SubtreeWalker subtree;
      contextNode->walk(subtree);
    
      if( subtree.isComputable() ) {
        // compute it, then cut off the subtree
        _computeCounts( newNode, subtree );
        newNode->setRawExtent( 0 );
        newNode->setContext( 0 );
      } else if( newNode->getContext() == 0 ) {
        // we can't figure out how many term occurrences
        // there are in this thing, but at least we know
        // how big the context size is, so set it
        newNode->setContextSize( _index.termCount() );
      }
    }

    // if it wasn't computable, keep the subtree around so the
    // inference network code can run it and figure out the counts
    _newNodes.push_back( newNode );
    return newNode;
  }
};

#endif // INDRI_CONTEXTSIMPLECOUNTCOLLECTORCOPIER_HPP

