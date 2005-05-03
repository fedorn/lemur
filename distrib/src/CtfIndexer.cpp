
#include "CtfIndexer.hpp"


 
lemur::distrib::CtfIndexer::CtfIndexer(const string &csName, int bufferSize, 
		       bool countStopWords) {
  collsel = new lemur::index::InvPushIndex(csName, bufferSize);

  csdp = new lemur::parse::DocumentProps();
  term = new lemur::index::InvFPTerm();

  countStopWds = countStopWords;

  ctfCount = 0;
  first = true;
}

lemur::distrib::CtfIndexer::~CtfIndexer() {


  csdp->length(ctfCount);
  collsel->endDoc(csdp);
  collsel->endCollection(NULL);

  delete term;
  delete collsel;
  delete csdp;
}



char *
lemur::distrib::CtfIndexer::handleWord(char * word) {

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
lemur::distrib::CtfIndexer::newDb(const string &name) {
  
  if (!first) {
    cout << "coll term freq: " << ctfCount << endl;
    csdp->length(ctfCount);
    collsel->endDoc(csdp);
    
  } else {
    first = false;
  }

  csdp->stringID((char *)name.c_str()); // fix this
  csdp->length(0);
  collsel->beginDoc(csdp);


  ctfCount = 0;


}

