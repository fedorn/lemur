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
// PriorFactory
//
// 4 May 2004 -- tds
//

#include "indri/PriorFactory.hpp"
#include "indri/QuerySpec.hpp"
#include <fstream>
#include "Exception.hpp"

void PriorFactory::_readPriorFile( prior_data& data, const std::string& fileName ) {
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

void PriorFactory::_readParamValues( prior_data& data, Parameters& range ) {
  for( size_t j=0; j<range.size(); j++ ) {
    indri::lang::PriorNode::tuple_type t;

    t.begin = range[j].get("begin", 0);
    t.end = range[j].get("end", 0);
    t.score = range[j].get("score", 0.);

    data.tuples[t.begin] = t;
  }
}

PriorFactory::PriorFactory() 
{
}

PriorFactory::PriorFactory( Parameters& parameters ) :
  _parameters(parameters)
{
  initialize();
}

void PriorFactory::initialize( ) {
  if( _parameters.exists( "prior" ) ) {
    Parameters priors = _parameters["prior"];

    for( size_t i=0; i<priors.size(); i++ ) {
      Parameters prior = priors[i];
      std::string paramName = prior["name"];

      prior_data data;
      data.field = prior.get( "field", "" );

      if( prior.exists("file") ) {
        std::string filename = prior["file"];
        _readPriorFile( data, filename );
      } else if( prior.exists( "range" ) ) {
        Parameters range = priors["range"];
        _readParamValues( data, range );
      }

      _data[paramName] = data;
    }
  }
}

indri::lang::PriorNode* PriorFactory::create( const std::string& name ) {
  // look up the prior and return it if it exists
  if( _data.find( name ) != _data.end() ) {
    return new indri::lang::PriorNode( _data[name].field, _data[name].tuples );
  } else {
    LEMUR_THROW( LEMUR_RUNTIME_ERROR, "Prior '" + name + "' does not exist." );
  }
}
