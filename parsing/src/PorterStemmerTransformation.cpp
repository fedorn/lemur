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
// PorterStemmerTransformation
//
// 13 May 2004 -- tds
//
// Adapted from the Lemur PorterStemmer class
//

#include "indri/PorterStemmerTransformation.hpp"

// in porter_stemmer.cpp: warning: this function is not thread safe
extern int porter_stem(char * p, int i, int j);

ParsedDocument* PorterStemmerTransformation::transform( ParsedDocument* document ) {
  greedy_vector<char*>& terms = document->terms;

  for( size_t i=0; i<terms.size(); i++ ) {
    char* term = terms[i];
    
    if( !term )
      continue;

    int length = strlen( term );
    int newLength = porter_stem( term, 0, int(length)-1 );
    assert( newLength <= length );
    term[newLength+1] = 0;
  }

  return document;
}

void PorterStemmerTransformation::setHandler( ObjectHandler<ParsedDocument>& handler ) {
  _handler = &handler;
}

void PorterStemmerTransformation::handle( ParsedDocument* document ) {
  _handler->handle( transform( document ) );
}
