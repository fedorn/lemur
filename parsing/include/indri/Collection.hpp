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
// Collection
//
// 11 May 2004 -- tds
//

#ifndef INDRI_COLLECTION_HPP
#define INDRI_COLLECTION_HPP

#include "indri/ObjectHandler.hpp"
#include "indri/ParsedDocument.hpp"

class Collection {
public:
  virtual ~Collection() {};
  virtual void addDocument( int documentID, ParsedDocument* document ) = 0;
  virtual ParsedDocument* retrieve( int documentID ) = 0;
};

#endif // INDRI_COLLECTION_HPP
