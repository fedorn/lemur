/*==========================================================================
 *
 *  Original source copyright (c) 2003, Carnegie Mellon University.
 *  See copyright.cmu for details.
 *
 *==========================================================================
*/
#include "PropIndexTH.hpp"

PropIndexTH::PropIndexTH(char * filename, int bufferSize,
			 bool countStopWords, int ind) {
  // create index and helper objects  
  if (ind == 1)
    index = new InvFPPushIndex(filename, bufferSize);
  // add support for future positional indexes here
  else 
    throw Exception("PropIndexTH", "Unknown index type");

  dp = NULL;
  term = new InvFPTerm();
  countStopWds = countStopWords;
  docLength = 0;
  // set state that is on first doc
  first = true;
}

PropIndexTH::PropIndexTH(char * filename, int bufferSize,
			 bool countStopWords, string ind) {
  // create index and helper objects  
  if (ind == "inv")
    index = new InvFPPushIndex(filename, bufferSize);
  else if (ind == "key")
    index = new KeyfileIncIndex(filename, bufferSize);
  // add support for future positional indexes here
  else 
    throw Exception("PropIndexTH", "Unknown index type");

  dp = NULL;
  term = new InvFPTerm();
  countStopWds = countStopWords;
  docLength = 0;
  // set state that is on first doc
  first = true;
}

PropIndexTH::~PropIndexTH() {
  // end the doc and close the collection
  if (!first) endDoc(); // if we haven't done any yet, don't bother.
  index->endCollection(NULL);
  delete dp;
  delete term;
  delete index;
}

void PropIndexTH::endDoc() {
  dp->length(docLength);
  index->endDoc(dp);
  dp->stringID(NULL);
}

char* PropIndexTH::handleDoc(char* docno) {
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

char* PropIndexTH::handleWord(char* word, char* original, PropertyList *list) {
  Property* prop = NULL;
  int position = -1;
  char* tag = NULL;
  if (word) {
    // ignore words longer than max characters
    int len = strlen(word);
    if (len <= MAX_WORD_LENGTH) {
      // get position first
      prop = list->getProperty("position");
      if (!prop)
	throw Exception("PropIndexTH","missing \"position\" property in property list");
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
      list->startIteration();
      while (list->hasMore()) {
	prop = list->nextEntry();
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
    prop = list->getProperty("NE");
    if ((prop) && (prop->getLength() > 1)) {
      // index the term
      // still ignore words longer than max characters
      int len = strlen(original);
      if (len <= MAX_WORD_LENGTH) {
	//	tag = (char*) prop->getValue();
	// get position first
	prop = list->getProperty("position");
	if (!prop)
	  throw Exception("PropIndexTH","missing \"position\" property in property list");
	position = *((int*) prop->getValue());
	term->position(position);
	if (len > 0) {
	  // index the word 
	  term->strLength(len);
	  term->spelling(original);
	  index->addTerm(*term);
	}

	// index the NE tag
	prop = list->getProperty("NE");
	tag = (char*)prop->getValue();
	term->strLength(strlen(tag));
	term->spelling(tag);
	index->addTerm(*term);

	// index the B_NE tag if any
	prop = list->getProperty("B_NE");
	if (prop) {
	  tag = (char*)prop->getValue();
	  term->strLength(strlen(tag));
	  if (term->strLength() > 0) {
	    term->spelling(tag);
	    index->addTerm(*term);
	  }
	} // B_NE

	// index the E_NE tag if any
	prop = list->getProperty("E_NE");
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


void PropIndexTH::setDocManager(const char* mgrID) {
  index->setDocManager(mgrID);
}
