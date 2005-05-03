/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

#include "FreqCounter.hpp"

#include "common_headers.hpp"


// need time to seed the random number generator
#ifdef _WIN32
#include <time.h>
#else
#include <sys/time.h>
#endif
#include <cctype>

using namespace lemur::api;


lemur::distrib::FreqCounter::FreqCounter(const Stopper * stopWords) {
  stopper = stopWords;

  // Set defaults
  randomMode = R_CTF;
  srand(time(NULL));
  ctfTot = 0;
  dfTot = 0;
  nWords = 0;
  atfValid = false;
  name = "";
}

lemur::distrib::FreqCounter::FreqCounter(const string &filename, const Stopper * stopWords) {
  stopper = stopWords;

  // Set defaults
  randomMode = R_CTF;
  srand(time(NULL));
  ctfTot = 0;
  dfTot = 0;
  nWords = 0;
  atfValid = false;
  name = "";

  // Load statistics from file
  input(filename);
}

lemur::distrib::FreqCounter::~FreqCounter() {
  // Free memory
  freqmap::iterator curr = freqInfo.begin();
  while (curr != freqInfo.end()) {
    free (curr->first);
    curr++;
  }
}

void
lemur::distrib::FreqCounter::clear() {

  // Empty containers
  randdone.clear();
  doc.clear();

  // Free memory
  freqmap::iterator curr = freqInfo.begin();
  while (curr != freqInfo.end()) {
    free (curr->second.word);
    curr++;
  }

  freqInfo.clear();

  // Reset counts
  ctfTot = 0;
  dfTot = 0;
  nWords = 0;
  atfValid = false;
}

char * 
lemur::distrib::FreqCounter::handleWord(char * word) {
  char * oword = word;
  if (word != NULL) {
    if (stopper == NULL || !stopper->stopWord(word)) {

      freqmap::iterator curr = freqInfo.find(word);
      if (curr == freqInfo.end()) {    
	// Create a new freqinfo_t structure
	word = strdup(word);
	freqinfo_t fi;
	fi.word = word;
	fi.ctf = 0;
	fi.df = 0;
	freqInfo[word] = fi;
	nWords++;
      } else { 
	word = curr->second.word;
      }
      // Increment counts
      freqInfo[word].ctf++;
      ctfTot++;

      // Store this word in the doc set
      doc.insert(word);

      // Average term frequency sum is no longer valid.
      atfValid = false;
    }
  }
  return oword;    
}

char * 
lemur::distrib::FreqCounter::handleDoc(char * docno) {
  endDoc();
  return docno;
}

void
lemur::distrib::FreqCounter::endDoc() {
  // Update the document frequencies.

  // Iterate over terms in the document.
  stringset::iterator curr = doc.begin();
  while (curr != doc.end()) {
    //      if (freqInfo.find(*curr) == freqInfo.end()) {
    //        cerr << "Word not found: " << *curr << endl;
    //      }
    // Up the document frequency for the word.
    freqInfo[*curr].df++;
    dfTot += 1;
    curr++;
  }

  // Average term frequency sum is no longer valid.
  atfValid = false;
  
  // Clear the doc set.
  doc.clear();
}


void 
lemur::distrib::FreqCounter::input(const string &filename) {
  char word[5000];
  int ctfc;
  int dfc;

  // Load statistics from the file.
  ifstream istr(filename.c_str());
  while (! istr.eof()) {
    // Read in a line from the file.
    istr >> word;
    if (istr.eof()) break;
    istr >> ctfc;
    if (istr.eof()) break;
    istr >> dfc;

    if (stopper == NULL || !stopper->stopWord(word)) {

      // Create a new freqinfo_t structure.
      char * w = strdup(word);
      freqinfo_t fi;
      fi.word = w;
      fi.ctf = ctfc;
      fi.df = dfc;

      // Place structure in the map
      freqInfo[w] = fi;

      // Update totals
      ctfTot += ctfc;
      dfTot += dfc;
      nWords++;
      
    }
  }

  // Average term frequency sum is no longer valid.
  atfValid = false;
  istr.close();
}


// Output counts to file.
void
lemur::distrib::FreqCounter::output(const string &filename) const {
  ofstream outfile(filename.c_str());
  // Iterate over terms in the freqInfo map
  freqmap::const_iterator curr = freqInfo.begin();
  while (curr != freqInfo.end()) {
    //    char * word = curr->first;
    const freqinfo_t fi = curr->second;
    // dumb error detection
    if (!(fi.df > fi.ctf || fi.ctf > ctfTot || fi.df > dfTot)) 
      // Output word, collection term frequency, and document frequency.
      outfile << fi.word << "\t" << fi.ctf << "\t" << fi.df << endl;
    else 
      cerr << "Bogus word." << endl;
    curr++;
  }
}


// Select a random word.
char *
lemur::distrib::FreqCounter::randomWord() {
  char * w;

  // Find a random word given the random word selection mode.
  if (randomMode == R_AVE_TF)
    w = randomAveTf();
  else if (randomMode == R_DF) 
    w = randomDf();
  else if (randomMode == R_UNIFORM) 
    w = randomUniform();
  else 
    w = randomCtf();
 
  // Check to see if this word has been selected before.
  stringset::iterator it = randdone.find(w);
  if (it != randdone.end()) {
    // Word was not unique, select another.
    free (w);
    w = randomWord(); // recurses indefinitely if all terms have been used -LMY
  }
  randdone.insert(w);

  // Word was unique, return it.
  return w;
}

char *
lemur::distrib::FreqCounter::randomCtf() const {
  // Random number from 1 to collection term frequencies total
  int n = (rand() % ctfTot) + 1;
  int i = 0;
  // Iterate over terms until i (= sum of ctf for words seen so far)
  // is at least n.
  freqmap::const_iterator curr = freqInfo.begin();
  i += curr->second.ctf;
  while (curr != freqInfo.end() && i < n) {
    curr++;
    i += curr->second.ctf;
  }
  return strdup(curr->first);
}

char *
lemur::distrib::FreqCounter::randomDf() const {
  // Random number from 1 to document frequencies total
  int n = (rand() % dfTot) + 1;
  int i = 0;
  // Iterate over terms until i (= sum of df for words seen so far)
  // is at least n.
  freqmap::const_iterator curr = freqInfo.begin();
  i += curr->second.df;
  while (curr != freqInfo.end() && i < n) {
    curr++;
    i += curr->second.df;
  }
  return strdup(curr->first);
}

char *
lemur::distrib::FreqCounter::randomAveTf() const {
  if (atfValid == false) {
    // Recompute avetfTot
    avetfTot = 0;
    freqmap::const_iterator curr = freqInfo.begin();
    while (curr != freqInfo.end()) {
      avetfTot += curr->second.ctf/ (long double) curr->second.df;
      curr++;
    }    
    atfValid = true;
  }


  // Random number between 0 and 1
  long double n = rand() / (long double) LONG_MAX;
  long double i = 0;
  // Iterate over terms until i (= sum of avetf/avetfTot for words seen so far)
  // is at least n.
  freqmap::const_iterator curr = freqInfo.begin();
  i += (curr->second.ctf / (long double) curr->second.df) / avetfTot;
  while (curr != freqInfo.end() && i < n) {
    curr++;
    i += (curr->second.ctf / (long double) curr->second.df) / avetfTot;
  }
  return strdup(curr->first);
}

char *
lemur::distrib::FreqCounter::randomUniform() const {
  // Random number between 1 and number of unique words
  int n = (rand() % nWords) + 1;
  int i = 0;
  // Iterate over terms until i (= number words seen so far)
  // is at least n.
  freqmap::const_iterator curr = freqInfo.begin();
  i++; 
  while (curr != freqInfo.end() && i < n) {
    curr++;
    i++;
  }
  return strdup(curr->first);
}

// Compute the collection term frequecy ratio of two LMs.
// This model is the reference model.
double
lemur::distrib::FreqCounter::ctfRatio(FreqCounter & lm) const {
  double ctfSum = 0.0;
  double ctfTot2 = 0.0;
  freqmap::const_iterator curr = freqInfo.begin();
  while (curr != freqInfo.end()) {
    if (lm.getCtf(curr->first) > 0) {
      ctfSum += curr->second.ctf;
    }
    ctfTot2 += curr->second.ctf;
    curr++;
  }
  return (ctfSum / (double) ctfTot2);
}
namespace lemur
{
  namespace distrib
  {
    
    // Used by pruneBottomWords - stores a set ordered
    // by ctf then word.
    struct ltctf
    {
      bool operator()(freqinfo_t f1, freqinfo_t f2) const{
        if (f1.ctf == f2.ctf) 
          return strcmp(f2.word, f1.word) < 0;
        return (f1.ctf - f2.ctf) < 0;
      }
    };
    typedef set<freqinfo_t, ltctf> ctfset;
  }
}


// Remove all but the most frequent numTopWords words.
void
lemur::distrib::FreqCounter::pruneBottomWords(int numTopWords) {
  freqmap::iterator curr = freqInfo.begin();
  int nw = 1;
  ctfset topWords;
  freqinfo_t min = curr->second;
  curr++;
  while (curr != freqInfo.end()) {
    freqinfo_t fi = curr->second;
    if (nw < numTopWords) {
      // not enough words in topWords yet, add the next
      topWords.insert(fi);
      if (fi.ctf < min.ctf) {
	min = fi;
      }
      nw++;
    } else {
      if (fi.ctf > min.ctf) {
	topWords.erase(min);
	free(min.word);
	topWords.insert(fi);
	ctfset::iterator tmp = topWords.begin();
	min = *tmp;

      }
    }
    curr++;
  }
  freqInfo.clear();
  ctfset::iterator it = topWords.begin();
  while (it != topWords.end()) {
    freqInfo[(*it).word] = *it;
    it++;
  }

}

//------ Get and set functions below. ------
 
void
lemur::distrib::FreqCounter::setName(const string &freqCounterName) {
  name = freqCounterName;
}

const string &
lemur::distrib::FreqCounter::getName() const {
  return name;
}


const lemur::distrib::freqmap *
lemur::distrib::FreqCounter::getFreqInfo() const {
  return &freqInfo;
}

void 
lemur::distrib::FreqCounter::setRandomMode(int mode) {
  // only change the mode if it is in the right range 
  if (mode <= R_UNIFORM && mode >= R_CTF) {
    randomMode = mode;
  }
}

int
lemur::distrib::FreqCounter::getRandomMode() const {
  return randomMode;
}

int 
lemur::distrib::FreqCounter::getCtf(const char * word) const {
  //we're not going to change w, but need it to work with map API
  char* w = const_cast<char*>(word);
  if (freqInfo.find(w) != freqInfo.end()) {
    const freqinfo_t fi = freqInfo[w];
    return fi.ctf;
  }
  return 0;
}

int
lemur::distrib::FreqCounter::getDf(const char * word) const {
  //we're not going to change w, but need it to work with map API
  char* w = const_cast<char*>(word);
  if (freqInfo.find(w) != freqInfo.end()) {
    const freqinfo_t fi = freqInfo[w];
    return fi.df;
  }
  return 0;
}

// Return the average term frequency for a word
double
lemur::distrib::FreqCounter::getAveTf(const char * word) const {
  //we're not going to change w, but need it to work with map API
  char* w = const_cast<char*>(word);
  if (freqInfo.find(w) != freqInfo.end()) {
    const freqinfo_t fi = freqInfo[w];
    return (fi.ctf / (double) fi.df);
  }
  return 0;
}


// Return number of unique words.
int 
lemur::distrib::FreqCounter::numWords() const {
  return nWords;
}

// Return total count of words.
int 
lemur::distrib::FreqCounter::totWords() const {
  return ctfTot;
}
