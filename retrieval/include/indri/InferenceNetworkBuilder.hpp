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
// InferenceNetworkBuilder
//
// 8 March 2004 -- tds
//

#ifndef INDRI_INFERENCENETWORKBUILDER_HPP
#define INDRI_INFERENCENETWORKBUILDER_HPP

#include "indri/QuerySpec.hpp"
#include "indri/Repository.hpp"
#include "indri/InferenceNetwork.hpp"
#include "indri/ListCache.hpp"
#include <map>
#include <vector>

class InferenceNetworkBuilder : public indri::lang::Walker {
private:
  std::map< indri::lang::Node*, InferenceNetworkNode* > _nodeMap;
  InferenceNetwork* _network;
  Repository& _repository;
  ListCache& _cache;
  int _resultsRequested;
  
  template<typename _To, typename _From>
  std::vector<_To*> _translate( const std::vector<_From*>& children ) {
    std::vector<_To*> translation;

    for( unsigned int i=0; i<children.size(); i++ ) {
      translation.push_back( dynamic_cast<_To*>(_nodeMap[children[i]]) );  
    }

    return translation;
  }

  TermScoreFunction* _buildTermScoreFunction( const std::string& smoothing, UINT64 occurrences, UINT64 contextSize ) const;

public:
  InferenceNetworkBuilder( Repository& repository, ListCache& cache, int resultsRequested );
  ~InferenceNetworkBuilder();

  InferenceNetwork* getNetwork();

  void defaultAfter( indri::lang::Node* node );
  void after( indri::lang::IndexTerm* term );
  void after( indri::lang::Field* field );
  void after( indri::lang::ExtentAnd* extentAnd );
  void after( indri::lang::ExtentOr* extentOr );
  void after( indri::lang::ExtentInside* extentInside );
  void after( indri::lang::ODNode* odNode );
  void after( indri::lang::UWNode* uwNode );
  void after( indri::lang::FilRejNode* uwNode );
  void after( indri::lang::FilReqNode* uwNode );
  void after( indri::lang::ContextCounterNode* contextCounterNode );
  void after( indri::lang::ScoreAccumulatorNode* scoreAccumulatorNode );
  void after( indri::lang::AnnotatorNode* annotatorNode );
  void after( indri::lang::TermFrequencyScorerNode* termScorerNode );
  void after( indri::lang::CachedFrequencyScorerNode* cachedScorerNode );
  void after( indri::lang::RawScorerNode* rawScorerNode );
  void after( indri::lang::ExtentRestriction* erNode );
  void after( indri::lang::PriorNode* priorNode );
  void after( indri::lang::WeightNode* weightNode );
  void after( indri::lang::CombineNode* combineNode );
  void after( indri::lang::OrNode* orNode );
  void after( indri::lang::NotNode* notSpecNode );
  void after( indri::lang::MaxNode* maxNode );
  void after( indri::lang::WAndNode* wandNode );
  void after( indri::lang::WSumNode* wsumNode );
  void after( indri::lang::FieldLessNode* flNode );
  void after( indri::lang::FieldGreaterNode* fgNode );
  void after( indri::lang::FieldBetweenNode* fbNode );
  void after( indri::lang::FilterNode* filterNode );
};

#endif // INDRI_INFERENCENETWORKBUILDER


