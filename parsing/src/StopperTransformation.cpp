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
// StopperTransformation
//
// 13 May 2004 -- tds
//


#include "indri/StopperTransformation.hpp"
#include "string-set.h"
#include "Exception.hpp"
#include <fstream>
#include <iostream>

StopperTransformation::StopperTransformation() :
  _table( string_set_create() ),
  _handler(0)
{
}

StopperTransformation::StopperTransformation( const std::vector<char*>& stopwords ) :
  _table( string_set_create() ),
  _handler(0)
{
  read( stopwords );
}

StopperTransformation::StopperTransformation( const std::vector<const char*>& stopwords ) :
  _table( string_set_create() ),
  _handler(0)
{
  read( stopwords );
}

StopperTransformation::StopperTransformation( const std::vector<std::string>& stopwords ) :
  _table( string_set_create() ),
  _handler(0)
{
  read( stopwords );
}

StopperTransformation::StopperTransformation( Parameters& stopwords ) :
  _table( string_set_create() ),
  _handler(0)
{
  read( stopwords );
}

StopperTransformation::~StopperTransformation() {
  string_set_delete( _table );
}

void StopperTransformation::read( const std::string& filename ) {
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

void StopperTransformation::read( const std::vector<std::string>& stopwords ) {
  for( size_t i=0; i<stopwords.size(); i++ ) {
    string_set_add( stopwords[i].c_str(), _table );
  }
}

void StopperTransformation::read( const std::vector<const char*>& stopwords ) {
  for( size_t i=0; i<stopwords.size(); i++ ) {
    string_set_add( stopwords[i], _table );
  }
}

void StopperTransformation::read( const std::vector<char*>& stopwords ) {
  for( size_t i=0; i<stopwords.size(); i++ ) {
    string_set_add( stopwords[i], _table );
  }
}

void StopperTransformation::read( Parameters& stopwords ) {
  for( int i=0; unsigned(i) < stopwords.size(); i++ ) {
    string_set_add( ( (std::string) stopwords[i] ).c_str(), _table );
  }
}

ParsedDocument* StopperTransformation::transform( ParsedDocument* document ) {
  greedy_vector<char*>& terms = document->terms;

  for( size_t i=0; i<terms.size(); i++ ) {
    if( terms[i] && string_set_lookup( terms[i], _table ) != 0 ) {
      terms[i] = 0;
    }
  }

  return document;
}

void StopperTransformation::handle( ParsedDocument* document ) {
  _handler->handle( transform(document) );
}

void StopperTransformation::setHandler( ObjectHandler<ParsedDocument>& handler ) {
  _handler = &handler;
}
