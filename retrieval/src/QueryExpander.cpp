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
// QueryExpander
// 
// 18 Aug 2004 -- dam
//

#include "indri/QueryExpander.hpp"

QueryExpander::QueryExpander( QueryEnvironment * env , Parameters& param ) {
  _env = env;
  _param = param;
}

std::vector<ScoredExtentResult> QueryExpander::runExpandedQuery( std::string originalQuery , int resultsRequested , bool verbose) {
  if( verbose )
    std::cout << "Unexpanded query = " << originalQuery << std::endl;

  // run original query
  std::vector<ScoredExtentResult> results = _env->runQuery( originalQuery , resultsRequested );

  // expand original query
  std::string expandedQuery = expand( originalQuery , results );

  if( verbose )
    std::cerr << "Expanded query = " << expandedQuery << std::endl;

  // run expanded query
  results = _env->runQuery( expandedQuery , resultsRequested );

  return results;
}

std::vector<DocumentVector*> QueryExpander::getDocumentVectors( std::vector<ScoredExtentResult>& results, int fbDocs ) {
  // copy results documents into a doc ID vector
  std::vector<int> documentIDs;
  for( int doc = 0; doc < fbDocs && doc < results.size(); doc++ )
    documentIDs.push_back( results[ doc ].document );

  // fetch the document vectors
  return _env->documentVectors( documentIDs );
}

std::vector<std::string> * QueryExpander::getVocabulary( std::vector<ScoredExtentResult>& results, int fbDocs ) {
  std::vector<DocumentVector*> docVectors = getDocumentVectors( results, fbDocs );
  std::vector<std::string> * strings;
  
  strings = getVocabulary( docVectors );
  delete_vector_contents( docVectors );
  return strings;
}

std::vector<std::string> * QueryExpander::getVocabulary( std::vector<DocumentVector*>& docVectors ) {
  std::map<std::string, bool> terms;

  for( int doc = 0; doc < docVectors.size(); doc++ ) {
    DocumentVector * docVec = docVectors[ doc ];
    std::vector<int> term_positions = docVec->positions();
    std::vector<std::string> term_list = docVec->stems();

    std::vector<int>::iterator pos_iter;
    for( pos_iter = term_positions.begin() ; pos_iter != term_positions.end() ; ++pos_iter )
      terms[ term_list [ *pos_iter ] ] = true;
  }

  std::vector<std::string> * vocab = new std::vector<std::string>;
  std::map<std::string, bool>::iterator term_iter;
  for( term_iter = terms.begin() ; term_iter != terms.end() ; ++term_iter )
    vocab->push_back( term_iter->first );

  return vocab;
}

int QueryExpander::getCF( std::string term ) {
  int cf = _cf_cache[ term ];

  if( cf == 0 ) {
    cf = _env->stemCount( term );
    _cf_cache[ term ] = cf;
    return cf;
  }

  return cf;
}

