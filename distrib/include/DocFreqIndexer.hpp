
#include "TextHandler.hpp"
#include "InvPushIndex.hpp"
#include "Parser.hpp"
#include "WordSet.hpp"

#include <stdio.h>

#ifndef _DOCFREQINDEXER_HPP
#define _DOCFREQINDEXER_HPP


class DocFreqIndexer : public TextHandler {

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

  InvPushIndex * collsel;

  DocumentProps * csdp;
  InvFPTerm * term;

  WordSet docWords;

  FILE * collWords;
  FILE * serverSizes;
  int numDocs;
  
  bool countStopWds;

};

#endif
