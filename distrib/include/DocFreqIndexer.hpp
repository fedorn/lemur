
#include "TextHandler.hpp"
#include "InvFPPushIndex.hpp"
#include "Parser.hpp"
#include "WordSet.hpp"

#include <stdio.h>

#ifndef _DOCFREQINDEXER_HPP
#define _DOCFREQINDEXER_HPP


class DocFreqIndexer : public TextHandler {

public:
  DocFreqIndexer(char * csName, char * cwName, 
		 char * ssName, int bufferSize, bool countStopWords = false);
  ~DocFreqIndexer();

  char * handleDoc(char * docno);
  char * handleWord(char * word);

  void newDb(char * name);



private:
  
  int cw;
  int dfCount;
  bool first;

  InvFPPushIndex * collsel;

  DocumentProps * csdp;
  InvFPTerm * term;

  WordSet docWords;

  FILE * collWords;
  FILE * serverSizes;
  int numDocs;
  
  bool countStopWds;

};

#endif