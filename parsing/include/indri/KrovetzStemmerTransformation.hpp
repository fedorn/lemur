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
// KrovetzStemmerTransformation.hpp
//
// 13 May 2004 -- tds
//

#ifndef INDRI_KROVETZSTEMMERTRANSFORMATION_HPP
#define INDRI_KROVETZSTEMMERTRANSFORMATION_HPP

#include "indri/Transformation.hpp"
#include "indri/Parameters.hpp"

class KrovetzStemmerTransformation : public Transformation {
private:
  char* _stemBuffer;
  int _stemBufferSize;
  ObjectHandler<ParsedDocument>* _handler;

  char* _getBuffer( size_t length );
  const char* _getBufferEnd() const;
  ParsedDocument* _restart( ParsedDocument* document, size_t lastIndex, char* endOfStemming );
  ParsedDocument* _processTerms( ParsedDocument* document, size_t start, char* stem, const char* end );
  char* KrovetzStemmerTransformation::_growBuffer( size_t length, char* oldEnd );

public:
  KrovetzStemmerTransformation( Parameters& parameters );
  ~KrovetzStemmerTransformation();

  ParsedDocument* transform( ParsedDocument* document );

  void setHandler( ObjectHandler<ParsedDocument>& handler );
  void handle( ParsedDocument* document ); 
};

#endif // INDRI_KROVETZSTEMMERTRANSFORMATION_HPP

