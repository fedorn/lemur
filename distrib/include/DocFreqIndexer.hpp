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

#ifndef _DOCFREQINDEXER_HPP
#define _DOCFREQINDEXER_HPP

#include "TextHandler.hpp"
#include "PushIndex.hpp"
#include "InvFPTermList.hpp"
#include "Parser.hpp"
#include "WordSet.hpp"

#include <stdio.h>


namespace lemur 
{
  namespace distrib
  {
    
    class DocFreqIndexer : public lemur::api::TextHandler {

    public:
      DocFreqIndexer(const string &csName, const string &cwName, 
                     const string &ssName, int bufferSize, 
                     bool countStopWords = false);
      ~DocFreqIndexer();

      char * handleDoc(char * docno);
      char * handleWord(char * word);
      void handleEndDoc();

      void newDb(const string &name);



    private:
  
      int cw;
      int dfCount;
      bool first;

      lemur::index::PushIndex * collsel;

      lemur::parse::DocumentProps * csdp;
      lemur::index::InvFPTerm * term;

      lemur::utility::WordSet docWords;

      FILE * collWords;
      FILE * serverSizes;
      int numDocs;
  
      bool countStopWds;

    };
  }
}

#endif
