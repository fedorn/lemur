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
// NormalizationTransformation
//
// 13 August 2004 -- tds
//

#include "indri/NormalizationTransformation.hpp"
#include "indri/Parameters.hpp"
#include "string-set.h"

NormalizationTransformation::NormalizationTransformation( Parameters* acronymList )
  :
  _handler(0),
  _acronyms(0)
{
  if( acronymList && acronymList->exists("word") ) {
    Parameters words = (*acronymList)["word"];
    _acronyms = string_set_create();
    
    for( int i=0; i<words.size(); i++ ) {
      std::string acronym;
      acronym = (std::string) words[i];
      string_set_add( acronym.c_str(), _acronyms );
    }
  }
}

NormalizationTransformation::~NormalizationTransformation() {
  if(_acronyms)
    string_set_delete(_acronyms);
}

ParsedDocument* NormalizationTransformation::transform( ParsedDocument* document ) {
  greedy_vector<char*>& terms = document->terms;
  unsigned int i, j, k;

  for( i=0; i<terms.size(); i++ ) {
    char* term = terms[i];

    // another step may have removed this term
    if( !term )
      continue;

    bool process = false;

    // look for uppercase letters, periods and apostrophes
    for( j=0; term[j]; j++ ) {
      char c = term[j];

      if( (c >= 'A' && c <= 'Z') || c == '.' || c == '\'' ) {
        process = true;
        break;
      }
    }

    // word is fine, no further processing is needed
    if( !process )
      continue;

    // remove periods and apostrophes
    for( j=0, k=0; term[j]; j++ ) {
      if( term[j] != '.' && term[j] != '\'' ) {
        term[k] = term[j];      
        k++;
      }
    }
    term[k] = 0;

    // if this is an acronym, skip it, otherwise, case normalize
    if( !_acronyms || !string_set_lookup( term, _acronyms ) ) {
      char* letter = term;
      
      for( ; *letter; letter++ )
        if( *letter >= 'A' && *letter <= 'Z' )
          *letter += 'a' - 'A';
    }
  }

  return document;
}

void NormalizationTransformation::handle( ParsedDocument* document ) {
  _handler->handle( transform( document ) );
}

void NormalizationTransformation::setHandler( ObjectHandler<ParsedDocument>& handler ) {
  _handler = &handler;
}


