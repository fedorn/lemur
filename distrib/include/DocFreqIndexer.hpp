#ifndef _DOCFREQINDEXER_HPP
#define _DOCFREQINDEXER_HPP

#include "TextHandler.hpp"
#include "InvPushIndex.hpp"
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

      lemur::index::InvPushIndex * collsel;

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
