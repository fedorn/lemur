
#include "TextHandler.hpp"
#include "InvPushIndex.hpp"
#include "Parser.hpp"
#include "WordSet.hpp"

#include <stdio.h>

#ifndef _CTFINDEXER_HPP
#define _CTFINDEXER_HPP


class CtfIndexer : public TextHandler {

public:
  CtfIndexer(char * csName, int bufferSize, bool countStopWords = false);
  ~CtfIndexer();

  char * handleWord(char * word);

  void newDb(char * name);

private:
  
  int ctfCount;
  bool first;

  InvPushIndex * collsel;

  DocumentProps * csdp;
  InvFPTerm * term;

 
  bool countStopWds;
};

#endif
