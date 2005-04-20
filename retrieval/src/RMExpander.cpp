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
// RMExpander
// 
// 18 Aug 2004 -- dam
//

#include "indri/RMExpander.hpp"
#include <math.h>
#include <stdio.h>
#include <sstream>

indri::query::RMExpander::RMExpander( indri::api::QueryEnvironment * env , indri::api::Parameters& param ) : indri::query::QueryExpander( env, param ) { }

std::string indri::query::RMExpander::expand( std::string originalQuery , std::vector<indri::api::ScoredExtentResult>& results ) {
  int fbDocs = _param.get( "fbDocs" , 10 );
  int fbTerms = _param.get( "fbTerms" , 10 );
  double fbOrigWt = _param.get( "fbOrigWeight", 0.5 );
  double mu = _param.get( "fbMu", 0 );

  std::vector<indri::api::DocumentVector*> docVectors = getDocumentVectors( results, fbDocs );
  std::vector<std::string> * rm_vocab = getVocabulary( docVectors );
  size_t vocabSize = rm_vocab->size();
  UINT64 colLen = _env->termCount();

  std::map<std::string, double> query_model;
  std::vector< std::pair<std::string, double> > probs;

  // gather document vectors / statistics for top fbDocs ranked documents
  for( int doc = 0; doc < fbDocs; doc++ ) {
    if( doc >= results.size() ) break;
    //std::cerr << "Expanding document " << doc << ", score = " << results[ doc ].score << std::endl;

    indri::api::DocumentVector * docVec = docVectors[ doc ];
    std::vector<int> term_positions = docVec->positions();
    std::vector<std::string> term_list = docVec->stems();

    size_t docLen = term_positions.size();

    // add in "seen" proportion of term probability
    std::vector<int>::iterator pos_iter;
    for( pos_iter = term_positions.begin() ; pos_iter != term_positions.end() ; ++pos_iter )
      query_model[ term_list[ *pos_iter ] ] += ( 1.0 / fbDocs ) * exp( results[ doc ].score ) / ( docLen + mu );

    // add in "unseen" proportion of term probability
    if( mu != 0.0 ) {
      std::vector<std::string>::iterator vocab_iter;
      for( vocab_iter = rm_vocab->begin() ; vocab_iter != rm_vocab->end() ; ++vocab_iter )
        query_model[ *vocab_iter ] += ( 1.0 / fbDocs ) * exp( results[ doc ].score ) * ( mu * double( getCF( *vocab_iter ) )/double(colLen) ) / ( docLen + mu );
    }
    
    delete docVec;
  }

  // sort terms based on P( w | \theta_Q )
  std::vector<std::string>::iterator vocab_iter;
  for( vocab_iter = rm_vocab->begin() ; vocab_iter != rm_vocab->end() ; ++vocab_iter )
    probs.push_back( std::pair<std::string, double>( *vocab_iter , query_model[ *vocab_iter ] ) );    

  std::sort( probs.begin() , probs.end() , QueryExpanderSort() );
  delete rm_vocab;

  return buildQuery( originalQuery, fbOrigWt, probs, fbTerms );
}

