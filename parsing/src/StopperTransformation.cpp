/*==========================================================================
 * Copyright (c) 2003-2004 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/


//
// StopperTransformation
//
// 13 May 2004 -- tds
//


#include "indri/StopperTransformation.hpp"
#include "string-set.h"
#include "Exception.hpp"
#include <fstream>
#include <iostream>

indri::parse::StopperTransformation::StopperTransformation() :
  _table( string_set_create() ),
  _handler(0)
{
}

indri::parse::StopperTransformation::StopperTransformation( const std::vector<char*>& stopwords ) :
  _table( string_set_create() ),
  _handler(0)
{
  read( stopwords );
}

indri::parse::StopperTransformation::StopperTransformation( const std::vector<const char*>& stopwords ) :
  _table( string_set_create() ),
  _handler(0)
{
  read( stopwords );
}

indri::parse::StopperTransformation::StopperTransformation( const std::vector<std::string>& stopwords ) :
  _table( string_set_create() ),
  _handler(0)
{
  read( stopwords );
}

indri::parse::StopperTransformation::StopperTransformation( indri::api::Parameters& stopwords ) :
  _table( string_set_create() ),
  _handler(0)
{
  read( stopwords );
}

indri::parse::StopperTransformation::~StopperTransformation() {
  string_set_delete( _table );
}

void indri::parse::StopperTransformation::read( const std::string& filename ) {
  std::ifstream in;

  in.open( filename.c_str(), std::ios::in );
  if( !in.good() )
    LEMUR_THROW( LEMUR_IO_ERROR, "Stopper was unable to open the stopwords file named: " + filename );

  while( !in.eof() ) {
    // a little buffer is okay; it doesn't really make sense to
    // have a long stopword anyway
    char buffer[512];
    in.getline( buffer, sizeof buffer );
    
    // skip blank lines
    if( strlen(buffer) )
      string_set_add( buffer, _table );
  }

  in.close();
}

void indri::parse::StopperTransformation::read( const std::vector<std::string>& stopwords ) {
  for( size_t i=0; i<stopwords.size(); i++ ) {
    string_set_add( stopwords[i].c_str(), _table );
  }
}

void indri::parse::StopperTransformation::read( const std::vector<const char*>& stopwords ) {
  for( size_t i=0; i<stopwords.size(); i++ ) {
    string_set_add( stopwords[i], _table );
  }
}

void indri::parse::StopperTransformation::read( const std::vector<char*>& stopwords ) {
  for( size_t i=0; i<stopwords.size(); i++ ) {
    string_set_add( stopwords[i], _table );
  }
}

void indri::parse::StopperTransformation::read( indri::api::Parameters& stopwords ) {
  for( int i=0; unsigned(i) < stopwords.size(); i++ ) {
    string_set_add( ( (std::string) stopwords[i] ).c_str(), _table );
  }
}

indri::api::ParsedDocument* indri::parse::StopperTransformation::transform( indri::api::ParsedDocument* document ) {
  indri::utility::greedy_vector<char*>& terms = document->terms;

  for( size_t i=0; i<terms.size(); i++ ) {
    if( terms[i] && string_set_lookup( terms[i], _table ) != 0 ) {
      terms[i] = 0;
    }
  }

  return document;
}

void indri::parse::StopperTransformation::handle( indri::api::ParsedDocument* document ) {
  _handler->handle( transform(document) );
}

void indri::parse::StopperTransformation::setHandler( ObjectHandler<indri::api::ParsedDocument>& handler ) {
  _handler = &handler;
}
