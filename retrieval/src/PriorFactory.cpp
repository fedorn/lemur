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
// PriorFactory
//
// 4 May 2004 -- tds
//

#include "indri/PriorFactory.hpp"
#include "indri/QuerySpec.hpp"
#include <fstream>
#include "Exception.hpp"

void indri::query::PriorFactory::_readPriorFile( prior_data& data, const std::string& fileName ) {
  // open a prior file
  std::ifstream in;
  in.open( fileName.c_str(), std::ifstream::in );

  // read each line of the prior file, and put it in a map
  while( !in.eof() ) {
    std::string doc;
    std::string valString;

    in >> doc
       >> valString;

    int begin;
    int end;

    int pos = doc.find( '-' );

    if( pos > 0 ) {
      std::string startString = doc.substr( 0, pos );
      std::string endString = doc.substr( pos+1 );

      begin = (int) string_to_i64( startString );
      end = (int) string_to_i64( endString );
    } else {
      begin = (int) string_to_i64( doc );
      end = begin;
    }

    double value = atof( valString.c_str() );

    indri::lang::PriorNode::tuple_type t;
    t.begin = begin;
    t.end = end;
    t.score = value;

    data.tuples[t.begin] = t;
  }

  in.close();
}

void indri::query::PriorFactory::_readParamValues( prior_data& data, indri::api::Parameters& range ) {
  for( size_t j=0; j<range.size(); j++ ) {
    indri::lang::PriorNode::tuple_type t;

    t.begin = range[j].get("begin", 0);
    t.end = range[j].get("end", 0);
    t.score = range[j].get("score", 0.);

    data.tuples[t.begin] = t;
  }
}

indri::query::PriorFactory::PriorFactory() 
{
}

indri::query::PriorFactory::PriorFactory( indri::api::Parameters& parameters ) :
  _parameters(parameters)
{
  initialize();
}

void indri::query::PriorFactory::initialize( ) {
  if( _parameters.exists( "prior" ) ) {
    indri::api::Parameters priors = _parameters["prior"];

    for( size_t i=0; i<priors.size(); i++ ) {
      indri::api::Parameters prior = priors[i];
      std::string paramName = prior["name"];

      prior_data data;
      data.field = prior.get( "field", "" );

      if( prior.exists("file") ) {
        std::string filename = prior["file"];
        _readPriorFile( data, filename );
      } else if( prior.exists( "range" ) ) {
        indri::api::Parameters range = priors["range"];
        _readParamValues( data, range );
      }

      _data[paramName] = data;
    }
  }
}

indri::lang::PriorNode* indri::query::PriorFactory::create( const std::string& name ) {
  // look up the prior and return it if it exists
  if( _data.find( name ) != _data.end() ) {
    return new indri::lang::PriorNode( _data[name].field, _data[name].tuples );
  } else {
    LEMUR_THROW( LEMUR_RUNTIME_ERROR, "Prior '" + name + "' does not exist." );
  }
}
