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

