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

#include "DocFreqIndexer.hpp"
#include "KeyfileIncIndex.hpp"


lemur::distrib::DocFreqIndexer::DocFreqIndexer(const string &csName, const string &cwName, 
                                               const string &ssName, int bufferSize, 
                                               bool countStopWords) {
  collsel = new lemur::index::KeyfileIncIndex(csName, bufferSize);
  collWords = fopen(cwName.c_str(), "ab");
  serverSizes = fopen(ssName.c_str(), "ab");

  csdp = new lemur::parse::DocumentProps();
  term = new lemur::index::InvFPTerm();

  countStopWds = countStopWords;

  cw = 0;
  dfCount = 0;
  first = true;
}

lemur::distrib::DocFreqIndexer::~DocFreqIndexer() {


  csdp->length(dfCount);
  collsel->endDoc(csdp);
  collsel->endCollection(NULL);

  fwrite(&cw, sizeof(int), 1, collWords);
  fwrite(&numDocs, sizeof(int), 1, serverSizes);
  fclose(collWords);
  fclose(serverSizes);

  delete term;
  delete collsel;
  delete csdp;
}

char *
lemur::distrib::DocFreqIndexer::handleDoc(char * docno) {
  handleEndDoc();
  numDocs++;
  return docno;
}

void
lemur::distrib::DocFreqIndexer::handleEndDoc() {
  docWords.clear();
}

char *
lemur::distrib::DocFreqIndexer::handleWord(char * word) {
  if (word != NULL) {

    int len = strlen(word);
    if (len <= 20) {
      if (!docWords.contains(word)) {
        term->strLength(len);
        term->spelling(word);
        term->position(dfCount);
        collsel->addTerm(*term);
        docWords.add(word);
        dfCount++;
      }
      cw++;
    }
  } else {
    if (countStopWds) {
      cw++;
    }
  }
  return word;
}


void 
lemur::distrib::DocFreqIndexer::newDb(const string &name) {
  
  if (!first) {
    cout << "words in collection: " << cw << endl;
    cout << "doc freq: " << dfCount << endl;
    csdp->length(dfCount);
    collsel->endDoc(csdp);
    fwrite(&cw, sizeof(int), 1, collWords);
    fwrite(&numDocs, sizeof(int), 1, serverSizes);
    
  } else {
    first = false;
  }

  csdp->stringID((char *)name.c_str());
  csdp->length(0);
  collsel->beginDoc(csdp);


  cw = 0;
  dfCount = 0;
  docWords.clear();
  numDocs = 0;


}

