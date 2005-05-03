
#include "TextHandler.hpp"
#include "InvPushIndex.hpp"
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
      lemur::index::InvPushIndex * collsel;
      lemur::parse::DocumentProps * csdp;
      lemur::index::InvFPTerm * term;
      bool countStopWds;
    };
  }
}

#endif
