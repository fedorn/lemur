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
/*
  author: dmf
*/
#ifndef _INVPASSAGETEXTHANDLER_HPP
#define _INVPASSAGETEXTHANDLER_HPP
#include "InvFPTextHandler.hpp"
#include "InvPassagePushIndex.hpp"

namespace lemur 
{
  namespace parse 
  {
    
    ///
    ///  InvPassageTextHandler builds an InvFPIndex using InvPassagePushIndex.
    ///  This class is a destination TextHandler.  
    ///

    class InvPassageTextHandler : public InvFPTextHandler {

    public:
      /// Create a InvPassageTextHandler with index name filename 
      /// (minus the .ifp extension) and specified buffer size.
      InvPassageTextHandler(const std::string &filename, int psgSize, int bufferSize, 
                            bool countStopWords = false) {
        // create index and helper objects  
        index = new lemur::index::InvPassagePushIndex(filename, psgSize, bufferSize);
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
