

/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
 */

#include "QryBasedSampler.hpp"
#include "Exception.hpp"

#include "common_headers.hpp"
#include <cassert>
#include <cstdio>


QryBasedSampler::QryBasedSampler() {

  // Set defaults
  numDocs = 300;
  numWords = 10000;
  numQueries = 300;
  termMode = 0;

  docsPerQuery = 10; 
}

QryBasedSampler::~QryBasedSampler() {

}

bool
QryBasedSampler::probe(char * initQuery) {
  // PRECONDITIONS:
  // language model manager has been set
  assert(freqCounter != NULL);
  // database manager has been set and a database is open
  assert(db != NULL);

  MemParser * parser = db->getParser();

  seenDocs.clear();
 
  // Document output file name
  char * dfname = (char *) malloc((strlen(outputPrefix) + 5) * sizeof(char));
  sprintf(dfname, "%sdocs", outputPrefix);
  db->setOutputFile(dfname);
  free (dfname);

  // the number of docs done so far
  int done = 0;
  
  // the number of queries done so far
  int qcount = 0;

  // the query we'll use for probing
  char * query;
  
  // Results returned by database manager.
  results_t * results; 

   // Check termination conditions
  while (!(((termMode & T_NDOCS) && (done >= numDocs)) ||
	   ((termMode & T_NWORDS) && (freqCounter->numWords() >= numWords)) ||
	   ((termMode & T_NQRYS) && (qcount >= numQueries)))) {

    // generate the query
    if (done == 0) {
      query = strdup(initQuery);
    } else {
      query = freqCounter->randomWord();
    }


    // send the query to the database
    //    fprintf(stderr, "#: %3d query: %s\n", qcount, query); 
    
    results = db->query(query, docsPerQuery);
    int numResults = results->numDocs;

    qcount++;

    if (done == 0 && (numResults == 0)) {
      cout << "No documents returned for initial query!" << endl;
      return false;
    }

    int od = done;
    int i;
    // Check termination conditions
    for (i = 0; 
	 (i < docsPerQuery) && 
	   (i < numResults) &&
	   (!(((termMode & T_NDOCS) && (done >= numDocs)) ||
	      ((termMode & T_NWORDS)&&(freqCounter->numWords() >= numWords)) ||
	      ((termMode & T_NQRYS) && (qcount >= numQueries)))); 
	 i++) {

      docid_t id = results->ids[i];
      
      // check to make sure we haven't already gotten this doc
      docidset::iterator it = seenDocs.find(id);
      if (it == seenDocs.end()) {
	seenDocs.insert(strdup(id));
	
	doc_t * doc;

	try {
	  // get doc
	  doc = db->getDoc(id);

	  // parse doc, which updates freqCounter
	  parser->parse(doc);
	
	  // Output doc to file.
	  db->output(id);
	  
	  // Free memory
	  if (doc->doc) {
	    delete [] doc->doc;
	  }
	  free (doc->id);
	  delete doc;

	  done++;	

	} catch (Exception &ex) {
	  cerr << "Skipping doc " << id << endl;
	  ex.writeMessage(cerr);

	  if (doc->doc) {
	    delete [] doc->doc;
	  }
	  if (doc) {
	    free (doc->id);
	    delete doc;
	  }
	}

      }

      free (id);
      
    }

    // Free memory
    for ( ; i < numResults; i++) {
      free (results->ids[i]);
    }
    delete [] results->ids;
    delete results;
    free (query);
  }

  freqCounter->endDoc();

  // output language model
  char * fname = (char *) malloc((strlen(outputPrefix) + 6) * sizeof(char));
  sprintf(fname, "%smodel", outputPrefix);
  freqCounter->output(fname);
  free (fname);

  return true;
}

//------- Get and set functions below. -------

void 
QryBasedSampler::setDBManager(DBManager * database) {
  db = database;
}

DBManager * 
QryBasedSampler::getDBManager() {
  return db;
}   


void 
QryBasedSampler::setFreqCounter(FreqCounter * counter) {
  freqCounter = counter;
}

FreqCounter * 
QryBasedSampler::getFreqCounter() {
  return freqCounter;
}

void
QryBasedSampler::setOutputPrefix(char * prefix) {
  outputPrefix = prefix;
}

char *
QryBasedSampler::getOutputPrefix() {
  return outputPrefix;
}

void 
QryBasedSampler::setNumDocs(int n) {
  numDocs = n;
}

int
QryBasedSampler::getNumDocs() {
  return numDocs;
}

void 
QryBasedSampler::setDocsPerQuery(int n) {
  docsPerQuery = n;
}

int
QryBasedSampler::getDocsPerQuery() {
  return docsPerQuery;
}

void 
QryBasedSampler::setNumWords(int n) {
  numWords = n;
}

int
QryBasedSampler::getNumWords() {
  return numWords;
}

void 
QryBasedSampler::setNumQueries(int n) {
  numQueries = n;
}

int
QryBasedSampler::getNumQueries() {
  return numQueries;
}


void
QryBasedSampler::setTermMode(int m) {
  termMode = m;
}

int
QryBasedSampler::getTermMode() {
  return termMode;
}
