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

