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
// CaseNormalizationTransformation
//
// 13 August 2004 -- tds
//

#ifndef INDRI_CASENORMALIZATIONTRANSFORMATION_HPP
#define INDRI_CASENORMALIZATIONTRANSFORMATION_HPP

#include "indri/Transformation.hpp"
#include "indri/Parameters.hpp"
#include "string-set.h"

class NormalizationTransformation : public Transformation {
private:
  ObjectHandler<ParsedDocument>* _handler;
  String_set* _acronyms;

public:
  NormalizationTransformation( Parameters* acronymList = 0 );
  ~NormalizationTransformation();

  ParsedDocument* transform( ParsedDocument* document );
  
  void handle( ParsedDocument* document );
  void setHandler( ObjectHandler<ParsedDocument>& handler );
};

#endif // INDRI_CASENORMALIZATIONTRANSFORMATION_HPP



