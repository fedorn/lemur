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
// ContextCountAccumulator
//
// 24 February 2004 -- tds
//

#ifndef INDRI_CONTEXTCOUNTACCUMULATOR_HPP
#define INDRI_CONTEXTCOUNTACCUMULATOR_HPP

#include "indri/ListIteratorNode.hpp"
#include "lemur-platform.h"
#include "lemur-compat.hpp"
#include <vector>
#include "indri/EvaluatorNode.hpp"
#include "indri/IndriIndex.hpp"
#include "indri/QuerySpec.hpp"
#include "indri/DocumentCount.hpp"
#include "indri/ListCache.hpp"

class ContextCountAccumulator : public EvaluatorNode {
private:
  // this is a ListIteratorNode that contains extents
  // that compose the context of the operation. _contextSize
  // is the sum of the lengths of the extents in _context.
  ListIteratorNode* _context;

  // this is a ListIteratorNode that emits an extent only for a
  // true query match; therefore _occurrences is just a simple count
  // the number of extents in _matches.
  ListIteratorNode* _matches; 

  std::string _name;
  UINT64 _occurrences;
  UINT64 _contextSize;

  UINT64 _maximumContextSize;
  UINT64 _minimumContextSize;
  UINT64 _maximumOccurrences;

  double _maximumContextFraction;

  EvaluatorNode::MResults _results;

  // cache support
  ListCache* _listCache;
  ListCache::CachedList* _cache;
  SimpleCopier _query;

public:
  ContextCountAccumulator( const std::string& name, UINT64 occurrences, UINT64 contextSize );
  ContextCountAccumulator( const std::string& name, UINT64 occurrences, UINT64 contextSize, UINT64 maximumOccurrences, UINT64 minimumContextSize, UINT64 maximumContextSize, double maximumContextFraction );
  ContextCountAccumulator( const std::string& name, ListCache* listCache, ListCache::CachedList* cache, ListIteratorNode* matches, UINT64 collectionSize, UINT64 maxDocumentLength );
  ContextCountAccumulator( const std::string& name, ListCache* listCache, ListCache::CachedList* cache, ListIteratorNode* matches, ListIteratorNode* context );

  ~ContextCountAccumulator();

  UINT64 getOccurrences() const;
  UINT64 getContextSize() const;

  const ListIteratorNode* getContextNode() const;
  const ListIteratorNode* getMatchesNode() const;
  const std::string& getName() const;

  const EvaluatorNode::MResults& getResults();
  void evaluate( int documentID, int documentLength );
  int nextCandidateDocument();
};

#endif // INDRI_CONTEXTCOUNTACCUMULATOR_HPP


