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

#ifndef INDRI_STOPPERTRANSFORMATION_HPP
#define INDRI_STOPPERTRANSFORMATION_HPP

#include "indri/Transformation.hpp"
#include <string>
#include <vector>
#include "string-set.h"
#include "indri/Parameters.hpp"

class StopperTransformation : public Transformation {
private:
  ObjectHandler<ParsedDocument>* _handler;
  String_set* _table;

public:
  StopperTransformation();
  StopperTransformation( const std::vector<std::string>& stopwords );
  StopperTransformation( const std::vector<const char*>& stopwords );
  StopperTransformation( const std::vector<char*>& stopwords );
  StopperTransformation( Parameters& stopwords );
  ~StopperTransformation();

  void read( const std::vector<std::string>& stopwords );
  void read( const std::vector<const char*>& stopwords );
  void read( const std::vector<char*>& stopwords );
  void read( const std::string& filename );
  void read( Parameters& stopwords );

  ParsedDocument* transform( ParsedDocument* document );

  void handle( ParsedDocument* document );
  void setHandler( ObjectHandler<ParsedDocument>& handler );
};

#endif // INDRI_STOPPERTRANSFORMATION_HPP

