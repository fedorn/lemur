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

#ifdef WIN32
#define snprintf _snprintf
#endif

RMExpander::RMExpander( QueryEnvironment * env , Parameters& param ) : QueryExpander( env, param ) { }

std::string RMExpander::expand( std::string originalQuery , std::vector<ScoredExtentResult>& results ) {
  int fbDocs = _param.get( "fbDocs" , 10 );
  int fbTerms = _param.get( "fbTerms" , 10 );
  double fbOrigWt = _param.get( "fbOrigWt", 0.5 );
  double mu = _param.get( "fbMu", 0 );

  std::vector<DocumentVector*> docVectors = getDocumentVectors( results, fbDocs );
  std::vector<std::string> * rm_vocab = getVocabulary( docVectors );
  int vocabSize = rm_vocab->size();
  int colLen = _env->termCount();

  std::map<std::string, double> query_model;
  std::vector< std::pair<std::string, double> > probs;

  // gather document vectors / statistics for top fbDocs ranked documents
  for( int doc = 0; doc < fbDocs; doc++ ) {
    if( doc >= results.size() ) break;
    //std::cerr << "Expanding document " << doc << ", score = " << results[ doc ].score << std::endl;

    DocumentVector * docVec = docVectors[ doc ];
    std::vector<int> term_positions = docVec->positions();
    std::vector<std::string> term_list = docVec->stems();

    int docLen = term_positions.size();

    // add in "seen" proportion of term probability
    std::vector<int>::iterator pos_iter;
    for( pos_iter = term_positions.begin() ; pos_iter != term_positions.end() ; ++pos_iter )
      query_model[ term_list[ *pos_iter ] ] += ( 1.0 / fbDocs ) * exp( results[ doc ].score ) / ( docLen + mu );

    // add in "unseen" proportion of term probability
    if( mu != 0.0 ) {
      std::vector<std::string>::iterator vocab_iter;
      for( vocab_iter = rm_vocab->begin() ; vocab_iter != rm_vocab->end() ; ++vocab_iter )
        query_model[ *vocab_iter ] += ( 1.0 / fbDocs ) * exp( results[ doc ].score ) * ( mu * getCF( *vocab_iter ) ) / ( docLen + mu );
    }
    
    delete docVec;
  }

  // sort terms based on P( w | \theta_Q )
  std::vector<std::string>::iterator vocab_iter;
  for( vocab_iter = rm_vocab->begin() ; vocab_iter != rm_vocab->end() ; ++vocab_iter )
    probs.push_back( std::pair<std::string, double>( *vocab_iter , query_model[ *vocab_iter ] ) );    

  std::sort( probs.begin() , probs.end() , QueryExpanderSort() );

  std::stringstream ret;
  
  ret.setf( std::ios_base::fixed );
  ret.precision( 32 );

  ret << "#weight( " 
      << fbOrigWt
      << " "
      << originalQuery
      << " "
      << (1.0 - fbOrigWt)
      << " #weight( ";

  // extract top fbTerms and construct a new query
  std::vector< std::pair<std::string, double> >::iterator query_model_iter;
  int num_added = 0;
  for( query_model_iter = probs.begin() ; query_model_iter != probs.end() && num_added < fbTerms; ++query_model_iter ) {
    std::string term = query_model_iter->first;
    // skip out of vocabulary term and those terms assigned 0 probability in the query model
    if( term != "[OOV]" && query_model_iter->second != 0.0 ) {
      ret << " " 
          << query_model_iter->second
          << " \""
          << term
          << "\" ";
      num_added++;
    }
  }

  ret << " ) ) ";

  delete rm_vocab;
  return ret.str();
}

