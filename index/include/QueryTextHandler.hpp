/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

#include "TextHandler.hpp"
#include "Index.hpp"
#include "InvFPTypes.hpp"

#ifndef _QUERYTEXTHANDLER_HPP
#define _QUERYTEXTHANDLER_HPP

namespace lemur 
{
  namespace parse 
  {
    
    ///
    ///  The QueryTextHandler is designed to help parse queries.
    ///
    ///  The QueryTextHandler
    ///  checks query terms against an Index and adds the
    ///  uppercase form of the term to the query if it occurs
    ///  more frequently than the parsed form passed to the 
    ///  QueryTextHandler.  This is to help catch query
    ///  terms that are acronyms but are not capitalized in
    ///  the query. 

    class QueryTextHandler : public lemur::api::TextHandler {

    public:

      QueryTextHandler();


      char * handleWord(char * word);


      /// Set the index used to check for acronyms.
      void setIndex(const lemur::api::Index * ind);

    private:
  
      /// The Index that will be used for acronym checking.
      const lemur::api::Index * index;
    };
  }
}

#endif
