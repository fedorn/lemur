/*==========================================================================
 * Copyright (c) 2000-2004 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software (and below), and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/

#include "TextHandler.hpp"
#include "PushIndex.hpp"
#include "InvFPTermList.hpp"
#include "Parser.hpp"
#include "WordSet.hpp"

#include <stdio.h>

#ifndef _CTFINDEXER_HPP
#define _CTFINDEXER_HPP

namespace lemur 
{
  namespace distrib 
  {
    
    class CtfIndexer : public lemur::api::TextHandler {

    public:
      CtfIndexer(const string &csName, int bufferSize, 
                 bool countStopWords = false);
      ~CtfIndexer();

      char * handleWord(char * word);

      void newDb(const string &name);

    private:
      int ctfCount;
      bool first;
      lemur::index::PushIndex * collsel;
      lemur::parse::DocumentProps * csdp;
      lemur::index::InvFPTerm * term;
      bool countStopWds;
    };
  }
}

#endif
