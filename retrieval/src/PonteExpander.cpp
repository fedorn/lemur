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
// PonteExpander
// 
// 18 Aug 2004 -- dam
//

#include "indri/PonteExpander.hpp"
#include <math.h>
#include <stdio.h>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include "indri/count_iterator"
#include "indri/DirichletTermScoreFunction.hpp"

struct PonteTerm {
  struct relevance_greater {
    bool operator () ( const PonteTerm& one, const PonteTerm& two ) const {
      return one.relevance > two.relevance;
    }
  };

  struct collection_greater {
    bool operator () ( const PonteTerm& one, const PonteTerm& two ) const {
      return one.collection > two.collection;
    }
  };

  struct relevance_projection {
    std::pair< std::string, double > operator () ( const PonteTerm& pterm ) {
      return std::make_pair( pterm.stem, pterm.relevance );
    }
  };

  struct pair_projection {
    PonteTerm operator () ( const std::pair<std::string, PonteTerm>& p ) const {
      return p.second;
    }
  };

  std::string stem;
  double relevance;
  double collection;
};

PonteExpander::PonteExpander( QueryEnvironment * env , Parameters& param ) : QueryExpander( env, param ) { }

//
// I'm trying to do something like Ponte expansion here, which ranks terms based on:
//    s(w) = \sum_{D \in R} [P(w|D) / P(w)]
// However, we just end up with terms with no way to pick the "good" ones.
// Therefore, we use weights on the terms based on RM.
//

std::string PonteExpander::expand( std::string originalQuery , std::vector<ScoredExtentResult>& results ) {
  int fbDocs = _param.get( "fbDocs" , 10 );
  int fbTerms = _param.get( "fbTerms" , 10 );
  double fbOrigWt = _param.get( "fbOrigWt", 0.5 );
  double mu = _param.get( "fbMu", 0 );

  std::vector<DocumentVector*> docVectors = getDocumentVectors( results, fbDocs );
  std::vector<std::string> * rm_vocab = getVocabulary( docVectors );
  size_t vocabSize = rm_vocab->size();
  UINT64 colLen = _env->termCount();

  std::map<std::string, PonteTerm> query_model;

  // initialize all PonteTerm structures
  for( int i = 0; i < rm_vocab->size(); i++ ) {
    PonteTerm pterm;

    pterm.stem = (*rm_vocab)[i];
    pterm.relevance = 0;
    pterm.collection = 0;

    query_model[ pterm.stem ] = pterm;
  }
  delete rm_vocab;

  // gather document vectors / statistics for top fbDocs ranked documents
  for( int doc = 0; doc < fbDocs && doc < results.size(); doc++ ) {
    DocumentVector * docVec = docVectors[ doc ];
    greedy_vector<int> positions = docVec->positions();
    const std::vector<std::string>& stems = docVec->stems();
    count_iterator<int> iter( positions.begin(), positions.end() );
    int docLen = int(positions.size());

    // find probabiliy of each term in the document
    for( ; iter != positions.end(); ++iter ) {
      const std::string& stem = stems[ (*iter).object ];
      UINT64 cf = getCF( stem );

      if( (*iter).count < 2 )
        continue;

      // P(w|D) / P(w) [Ponte]
      double documentProbability = double( (*iter).count ) / double(docLen);
      double collectionProbability = double(cf) / double(colLen);
      double logOdds = log( documentProbability / collectionProbability );

      // s(D) * P(w|D) [Lavrenko]
      DirichletTermScoreFunction f( mu, collectionProbability );
      double relevance = exp( results[doc].score ) * exp( f.scoreOccurrence( (*iter).count, docLen ) );

      // update the PonteTerm structure with computed probabilities
      PonteTerm& term = query_model[ stem ];
      term.collection += logOdds;
      term.relevance += relevance;
    }

    delete docVec;
  }

  // shove into a vector and sort terms by Ponte metric
  std::vector<PonteTerm> sortedModel;
  std::transform( query_model.begin(),
                  query_model.end(),
                  std::back_inserter( sortedModel ),
                  PonteTerm::pair_projection() );
  std::sort( sortedModel.begin(), sortedModel.end(), PonteTerm::collection_greater() );

  // copy into a vector with only the relevance weights */
  std::vector< std::pair<std::string, double> > probabilities;
  std::transform( sortedModel.begin(),
                  sortedModel.end(),
                  std::back_inserter( probabilities ),
                  PonteTerm::relevance_projection() );
  

  return buildQuery( originalQuery, fbOrigWt, probabilities, fbTerms );
}

