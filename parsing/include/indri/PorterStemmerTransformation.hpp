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

#ifndef INDRI_PORTERSTEMMERTRANSFORMATION_HPP
#define INDRI_PORTERSTEMMERTRANSFORMATION_HPP

#include "indri/Transformation.hpp"

class PorterStemmerTransformation : public Transformation {
private:
  ObjectHandler<ParsedDocument>* _handler;

public:
  ParsedDocument* transform( ParsedDocument* document );

  void setHandler( ObjectHandler<ParsedDocument>& handler );
  void handle( ParsedDocument* document );
};

#endif // INDRI_PORTERSTEMMERTRANSFORMATION_HPP
