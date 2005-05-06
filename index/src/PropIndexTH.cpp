/*==========================================================================
 *
 *  Original source copyright (c) 2003, Carnegie Mellon University.
 *  See copyright.cmu for details.
 *
 *==========================================================================
 */

#include "PropIndexTH.hpp"
#include "KeyfileIncIndex.hpp"

lemur::parse::PropIndexTH::PropIndexTH(const string &filename, int bufferSize,
                                       bool countStopWords, int ind) {
  // create index and helper objects  
  if (ind == 1)
    index = new lemur::index::InvFPPushIndex(filename, bufferSize);
  // add support for future positional indexes here
  else 
    throw lemur::api::Exception("PropIndexTH", "Unknown index type");

  dp = NULL;
  term = new lemur::index::InvFPTerm();
  countStopWds = countStopWords;
  docLength = 0;
  // set state that is on first doc
  first = true;
}

lemur::parse::PropIndexTH::PropIndexTH(const string &filename, int bufferSize,
                                       bool countStopWords, string ind) {
  // create index and helper objects  
  if (ind == "inv")
    index = new lemur::index::InvFPPushIndex(filename, bufferSize);
  else if (ind == "key")
    index = new lemur::index::KeyfileIncIndex(filename, bufferSize);
  // add support for future positional indexes here
  else 
    throw lemur::api::Exception("PropIndexTH", "Unknown index type");

  dp = NULL;
  term = new lemur::index::InvFPTerm();
  countStopWds = countStopWords;
  docLength = 0;
  // set state that is on first doc
  first = true;
}

lemur::parse::PropIndexTH::~PropIndexTH() {
  // end the doc and close the collection
  if (!first) endDoc(); // if we haven't done any yet, don't bother.
  endCollection();
  delete dp;
  delete term;
  delete index;
}

void lemur::parse::PropIndexTH::endDoc() {
  dp->length(docLength);
  index->endDoc(dp);
  dp->stringID(NULL);
}

void
lemur::parse::PropIndexTH::endCollection() {
  // write out properties from chain, excluding this 
  BasicCollectionProps* props = new BasicCollectionProps();
  if (!props) {
    cout << "could not initialize" << endl;
    return;
  }
  
  lemur::api::TextHandler* temphandler = this->getPrevHandler();
  while (temphandler) {
    temphandler->writePropertyList(props);
    temphandler = temphandler->getPrevHandler();
  }

  index->endCollection(props);
  delete (props);
}

char* lemur::parse::PropIndexTH::handleDoc(char* docno) {
  // finish the old doc  
  if (!first) {
    endDoc();
  } else {
    dp = new DocumentProps();
    first = false;
  }

  // begin the new document
  dp->length(0);  
  docLength = 0;
  dp->stringID(docno);
  index->beginDoc(dp);
  return docno;
}

char* lemur::parse::PropIndexTH::handleWord(char* word, const char* original, PropertyList *lst) {
  const Property* prop = NULL;
  int position = -1;
  char* tag = NULL;
  if (word) {
    // ignore words longer than max characters
    int len = strlen(word);
    if (len <= MAX_WORD_LENGTH) {
      // get position first
      prop = lst->getProperty("position");
      if (!prop)
        throw lemur::api::Exception("PropIndexTH","missing \"position\" property in property list");
      position = *((int*) prop->getValue());
      term->position(position);
      if (len > 0) {
        // index the word 
        term->strLength(len);
        term->spelling(word);
        index->addTerm(*term);
        docLength++;
      }

      // index all STRING properties
      lst->startIteration();
      while (lst->hasMore()) {
        prop = lst->nextEntry();
        if (prop->getType() == Property::STRING) {
          tag = (char*) prop->getValue();
          if (tag) {
            term->strLength(strlen(tag));
            if (term->strLength() > 0) {
              term->spelling(tag);
              index->addTerm(*term);
            }
          } // if tag 
        }// if prop is string
      } // while more props
    }
  } else {
    if (countStopWds) docLength++;
    // if the original was null too, there's nothing we can do
    if (!original)
      return word;

    // when the word is null, it usually means it was a stopword
    // sometimes stop words have named entity tags, in this case we want to index it
    // check to see if it has a named entity tag
    prop = lst->getProperty("NE");
    if ((prop) && (prop->getLength() > 1)) {
      // index the term
      // still ignore words longer than max characters
      int len = strlen(original);
      if (len <= MAX_WORD_LENGTH) {
        //      tag = (char*) prop->getValue();
        // get position first
        prop = lst->getProperty("position");
        if (!prop)
          throw lemur::api::Exception("PropIndexTH","missing \"position\" property in property list");
        position = *((int*) prop->getValue());
        term->position(position);
        if (len > 0) {
          // index the word 
          term->strLength(len);
          term->spelling(original);
          index->addTerm(*term);
        }

        // index the NE tag
        prop = lst->getProperty("NE");
        tag = (char*)prop->getValue();
        term->strLength(strlen(tag));
        term->spelling(tag);
        index->addTerm(*term);

        // index the B_NE tag if any
        prop = lst->getProperty("B_NE");
        if (prop) {
          tag = (char*)prop->getValue();
          term->strLength(strlen(tag));
          if (term->strLength() > 0) {
            term->spelling(tag);
            index->addTerm(*term);
          }
        } // B_NE

        // index the E_NE tag if any
        prop = lst->getProperty("E_NE");
        if (prop) {
          tag = (char*)prop->getValue();
          term->strLength(strlen(tag));
          if (term->strLength() > 0) {
            term->spelling(tag);
            index->addTerm(*term);
          }
        } // E_NE
      }     
    } // if NE
  } // else word is NULL
  return word;
}


void lemur::parse::PropIndexTH::setDocManager(const string &mgrID) {
  index->setDocManager(mgrID);
}
