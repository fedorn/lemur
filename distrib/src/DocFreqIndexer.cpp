#include "DocFreqIndexer.hpp"



DocFreqIndexer::DocFreqIndexer(char * csName, char * cwName, 
			 char * ssName, int bufferSize, bool countStopWords) {
  collsel = new InvFPPushIndex(csName, bufferSize);
  collWords = fopen(cwName, "wb");
  serverSizes = fopen(ssName, "wb");

  csdp = new DocumentProps();
  term = new InvFPTerm();

  countStopWds = countStopWords;

  cw = 0;
  dfCount = 0;
  first = true;
}

DocFreqIndexer::~DocFreqIndexer() {


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
DocFreqIndexer::handleDoc(char * docno, char * orig) {
  numDocs++;
  return docno;
}

void
DocFreqIndexer::handleEndDoc() {
  docWords.clear();
}

char *
DocFreqIndexer::handleWord(char * word, char * orig) {
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
DocFreqIndexer::newDb(char * name) {
  
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

  csdp->stringID(name);
  csdp->length(0);
  collsel->beginDoc(csdp);


  cw = 0;
  dfCount = 0;
  docWords.clear();
  numDocs = 0;


}

