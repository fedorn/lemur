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
// TermScoreFunctionFactory
//
// 19 August 2004 -- tds
//

#include "indri/TermScoreFunctionFactory.hpp"
#include "indri/BeliefNode.hpp"
#include "indri/TFIDFTermScoreFunction.hpp"
#include "indri/JelinekMercerTermScoreFunction.hpp"
#include "indri/DirichletTermScoreFunction.hpp"
#include "indri/TwoStageTermScoreFunction.hpp"
#include "indri/Parameters.hpp"

static void termscorefunctionfactory_parse( indri::api::Parameters& converted, const std::string& spec );

//
// Here's the place to add your own TermScoreFunction.
//
// Your job is to change the if( method == "..." ) statement in the get() method
// so that it can recognize your TermScoreFunction.  A spec line for Dirichlet might look like this:
//    dirichlet:1500
// That line represents Dirichlet smoothing with mu=1500.  The lines "d:1500" or "dir:1500" would also work.
//
// You'll want to pick a smoothing spec format with the name of your TermScoreFunction at the beginning,
// followed by a colon, followed by the parameters your smoothing method takes.  Return a pointer
// to a newly allocated TermScoreFunction.  It is the caller's job to delete the TermScoreFunction when
// it is finished with it.
//

indri::query::TermScoreFunction* indri::query::TermScoreFunctionFactory::get( const std::string& stringSpec, double collectionFrequency ) {
  indri::api::Parameters spec;
  termscorefunctionfactory_parse( spec, stringSpec );
  std::string method = spec.get( "method", "dirichlet" );

  if( method == "dirichlet" || method == "d" || method == "dir" ) {
    // dirichlet -- takes parameter "mu"
    double mu = spec.get( "mu", 2500 );
    double docmu=spec.get("documentMu",1e30);
    return new indri::query::DirichletTermScoreFunction( mu, collectionFrequency, docmu );
  } else if( method == "linear" || method == "jm" || method == "jelinek-mercer" ) {
    // jelinek-mercer -- can take parameters collectionLambda (or just lambda) and documentLambda
    double documentLambda = spec.get( "documentLambda", 0.0 );
    double collectionLambda;
    
    if( spec.exists( "collectionLambda" ) )
      collectionLambda = spec.get( "collectionLambda", 0.4 );
    else
      collectionLambda = spec.get( "lambda", 0.4 );

    return new indri::query::JelinekMercerTermScoreFunction( collectionFrequency, collectionLambda, documentLambda );
  } else if( method == "two" || method == "two-stage" || method == "twostage" ) {
    // twostage -- takes parameters mu and lambda
    double mu = spec.get( "mu", 2500 );
    double lambda = spec.get( "lambda", 0.4 );
    
    return new indri::query::TwoStageTermScoreFunction( mu, lambda, collectionFrequency );
  }

  // if nothing else worked, we'll use dirichlet with mu=2500
  return new indri::query::DirichletTermScoreFunction( 2500, collectionFrequency );
}

static void termscorefunctionfactory_parse( indri::api::Parameters& converted, const std::string& spec ) {
  int nextComma = 0;
  int nextColon = 0;
  int  location = 0;

  for( location = 0; location < spec.length(); ) {
    nextComma = spec.find( ',', location );
    nextColon = spec.find( ':', location );

    std::string key = spec.substr( location, nextColon-location );
    std::string value = spec.substr( nextColon+1, nextComma-nextColon-1 );

    converted.set( key, value );

    if( nextComma > 0 )
      location = nextComma+1;
    else
      location = spec.size();
  }
}

