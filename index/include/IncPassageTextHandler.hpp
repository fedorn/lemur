/*==========================================================================
 * Copyright (c) 2002 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/


#ifndef _INCPASSAGETEXTHANDLER_HPP
#define _INCPASSAGETEXTHANDLER_HPP
#include "IncFPTextHandler.hpp"
#include "IncPassagePushIndex.hpp"

namespace lemur
{
  namespace parse
  {
    
///
///  IncPassageTextHandler builds an InvFPIndex using IncPassagePushIndex.
///  This class is a destination TextHandler.  
///

class IncPassageTextHandler : public IncFPTextHandler {

public:
  /// Create a IncPassageTextHandler with index name filename 
  /// (minus the .ifp extension) and specified buffer size.
  IncPassageTextHandler(const string &filename, int psgSize, int bufferSize, 
			bool countStopWords = false) {
    // create index and helper objects  
    index = new lemur::index::IncPassagePushIndex(filename, psgSize, bufferSize);
    dp = new DocumentProps();
    term = new lemur::index::InvFPTerm();
    countStopWds = countStopWords;
    docLength = 0;
    pos = 1;
    // set state that is on first doc
    first = true;
  }
};
  }
}

#endif

