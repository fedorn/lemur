
#include "CtfIndexer.hpp"


 
CtfIndexer::CtfIndexer(char * csName, int bufferSize, bool countStopWords) {
  collsel = new InvFPPushIndex(csName, bufferSize);

  csdp = new DocumentProps();
  term = new InvFPTerm();

  countStopWds = countStopWords;

  ctfCount = 0;
  first = true;
}

CtfIndexer::~CtfIndexer() {


  csdp->length(ctfCount);
  collsel->endDoc(csdp);
  collsel->endCollection(NULL);

  delete term;
  delete collsel;
  delete csdp;
}



char *
CtfIndexer::handleWord(char * word, char * orig) {

  if (word != NULL) {

    int len = strlen(word);
    if (len <= 20) {
      term->strLength(len);
      term->spelling(word);
      term->position(ctfCount);
      collsel->addTerm(*term);
      ctfCount++;
    }
  } else {
    if (countStopWds) {
      ctfCount++;
    }
  }
  return word;
}


void 
CtfIndexer::newDb(char * name) {
  
  if (!first) {
    cout << "coll term freq: " << ctfCount << endl;
    csdp->length(ctfCount);
    collsel->endDoc(csdp);
    
  } else {
    first = false;
  }

  csdp->stringID(name);
  csdp->length(0);
  collsel->beginDoc(csdp);


  ctfCount = 0;


}

