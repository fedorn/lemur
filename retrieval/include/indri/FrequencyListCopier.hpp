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
// FrequencyListCopier
//
// 24 August 2004 -- tds
//
// Finds IndexTerm nodes that only need to return frequency information,
// not positions, and inserts IndexFrequencyTerms instead.
//

#ifndef INDRI_FREQUENCYLISTCOPIER_HPP
#define INDRI_FREQUENCYLISTCOPIER_HPP

#include "ListCache.hpp"

class FrequencyListCopier : public indri::lang::Copier {
private:
  std::vector<indri::lang::Node*> _nodes;
  std::stack<indri::lang::Node*> _disqualifiers;
  indri::lang::IndexTerm* _lastTerm;
  bool _disqualifiedTree;

  ListCache* _listCache;

public:
  FrequencyListCopier( ListCache* listCache ) : _listCache(listCache), _lastTerm(0), _disqualifiedTree(false) {}

  indri::lang::Node* defaultAfter( indri::lang::Node* oldNode, indri::lang::Node* newNode ) {
    if( _disqualifiers.size() && oldNode == _disqualifiers.top() )
      _disqualifiers.pop();
    
    _nodes.push_back( newNode );
    return newNode;
  }

  void before( indri::lang::ExtentAnd* exAnd ) {
    _disqualifiers.push(exAnd);
  }

  void before( indri::lang::ExtentOr* exOr ) {
    _disqualifiedTree = true;
  }

  void before( indri::lang::ExtentInside* exInside ) {
    _disqualifiedTree = true;
  }

  void before( indri::lang::ExtentRestriction* exRestrict ) {
    _disqualifiers.push(exRestrict);
  }

  void before( indri::lang::ContextCounterNode* context ) {
    if( context->getContext() != NULL ) {
      _disqualifiedTree = true;
    }
  }

  void before( indri::lang::ODNode* odNode ) {
    _disqualifiedTree = true;
  }

  void before( indri::lang::UWNode* uwNode ) {
    _disqualifiedTree = true;
  }

  indri::lang::Node* after( indri::lang::IndexTerm* oldNode, indri::lang::IndexTerm* newNode ) {
    _lastTerm = newNode;
    return defaultAfter( oldNode, newNode );
  }

  void before( indri::lang::RawScorerNode* oldNode, indri::lang::RawScorerNode* newNode ) {
    _lastTerm = 0;
    _disqualifiedTree = false;
  }

  indri::lang::Node* after( indri::lang::RawScorerNode* oldNode, indri::lang::RawScorerNode* newNode ) {
    indri::lang::Node* result = 0;

    if( _lastTerm && !_disqualifiers.size() && !_disqualifiedTree && oldNode->getContext() == NULL ) {
      indri::lang::TermFrequencyScorerNode* scorerNode;
      // there's a term to score, and nothing to disqualify us from doing frequency scoring
      scorerNode = new indri::lang::TermFrequencyScorerNode( _lastTerm->getText(),
                                                             _lastTerm->getStemmed() );

      scorerNode->setNodeName( oldNode->nodeName() );
      scorerNode->setSmoothing( oldNode->getSmoothing() );
      scorerNode->setStatistics( oldNode->getOccurrences(), oldNode->getContextSize(),
                                 oldNode->getMaxOccurrences(), oldNode->getMinContextLength(),
                                 oldNode->getMaxContextLength(),
                                 oldNode->getMaxContextFraction() );

      delete newNode;
      result = defaultAfter( oldNode, scorerNode );
    } else if( !_disqualifiers.size() ) {
      ListCache::CachedList* list = 0; 

      if( _listCache )
        list = _listCache->find( newNode->getRawExtent(), newNode->getContext() );
      
      if( list ) {
        indri::lang::CachedFrequencyScorerNode* cachedNode;
        cachedNode = new indri::lang::CachedFrequencyScorerNode( newNode->getRawExtent(), newNode->getContext() );
        cachedNode->setNodeName( newNode->nodeName() );
        cachedNode->setSmoothing( newNode->getSmoothing() );
        cachedNode->setList( list );

        delete newNode;
        result = defaultAfter( oldNode, cachedNode );
      } else {
        result = defaultAfter( oldNode, newNode );
      }
    } else {
      result = defaultAfter( oldNode, newNode );
    }

    _disqualifiedTree = false;
    return result; 
  }
};

#endif // INDRI_FREQUENCYLISTCOPIER_HPP

