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

#include "QryBasedSampler.hpp"
#include "Exception.hpp"

#include "common_headers.hpp"
#include <cassert>
#include <cstdio>


lemur::distrib::QryBasedSampler::QryBasedSampler() {

  // Set defaults
  numDocs = 300;
  numWords = 10000;
  numQueries = 300;
  termMode = 0;

  docsPerQuery = 10; 
}

lemur::distrib::QryBasedSampler::~QryBasedSampler() {

}

bool
lemur::distrib::QryBasedSampler::probe(const char * initQuery) {
  // PRECONDITIONS:
  // language model manager has been set
  assert(freqCounter != NULL);
  // database manager has been set and a database is open
  assert(db != NULL);

  // if initQuery is NULL, probe is being fed from randomWord and will
  // get no new inputs.
  // emit no output and return true to allow termination of the probe
  // loop in QryBasedSample.cpp
  // dmf 05/2005
  if (initQuery == NULL) {
      cout << "No query to probe with." << endl;
      return true;
  }


  MemParser * parser = db->getParser();

  seenDocs.clear();
 
  // Document output file name
  string dfname = outputPrefix + "docs";
  db->setOutputFile(dfname);

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
      // if there are no more unseen words, randomWord will return NULL
      // dmf 05/2005
      if (query == NULL) {
        // done > 0, so at least one probe completed.
        // emit the model
        freqCounter->endDoc();
        // output language model
        string fname = outputPrefix + "model";
        freqCounter->output(fname);
        return true;
      }
    }

    // send the query to the database
    //    fprintf(stderr, "#: %3d query: %s\n", qcount, query); 
    
    results = db->query(query, docsPerQuery);
    int numResults = results->numDocs;

    qcount++;

    if (done == 0 && (numResults == 0)) {
      free(query);
      delete[] (results->ids);
      delete(results);
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
              ((termMode & T_NWORDS)&&(freqCounter->numWords() >= numWords)) ) );

         i++) {

      docid_t id = results->ids[i];
      
      // check to make sure we haven't already gotten this doc
      docidset::iterator it = seenDocs.find(id);
      if (it == seenDocs.end()) {
        //      seenDocs.insert(strdup(id));
        seenDocs.insert(id);
        
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
          //      free (doc->id);
          delete doc;
          done++;       

        } catch (lemur::api::Exception &ex) {
          cerr << "Skipping doc " << id << endl;
          ex.writeMessage(cerr);

          if (doc->doc) {
            delete [] doc->doc;
          }
          if (doc) {
            //      free (doc->id);
            delete doc;
          }
        }

      }

      //      free (id);
      
    }

    // Free memory
    for ( ; i < numResults; i++) {
      //      free (results->ids[i]);
    }
    delete [] results->ids;
    delete results;
    free (query);
  }

  freqCounter->endDoc();

  // output language model
  string fname = outputPrefix + "model";
  freqCounter->output(fname);

  return true;
}

//------- Get and set functions below. -------

void 
lemur::distrib::QryBasedSampler::setDBManager(const DBManager * database) {
  db = database;
}

const lemur::distrib::DBManager * 
lemur::distrib::QryBasedSampler::getDBManager() const {
  return db;
}   


void 
lemur::distrib::QryBasedSampler::setFreqCounter(FreqCounter * counter) {
  freqCounter = counter;
}

const lemur::distrib::FreqCounter * 
lemur::distrib::QryBasedSampler::getFreqCounter() const {
  return freqCounter;
}

void
lemur::distrib::QryBasedSampler::setOutputPrefix(const string &prefix) {
  outputPrefix = prefix;
}

const string &
lemur::distrib::QryBasedSampler::getOutputPrefix() const {
  return outputPrefix;
}

void 
lemur::distrib::QryBasedSampler::setNumDocs(int n) {
  numDocs = n;
}

int
lemur::distrib::QryBasedSampler::getNumDocs() const {
  return numDocs;
}

void 
lemur::distrib::QryBasedSampler::setDocsPerQuery(int n) {
  docsPerQuery = n;
}

int
lemur::distrib::QryBasedSampler::getDocsPerQuery() const {
  return docsPerQuery;
}

void 
lemur::distrib::QryBasedSampler::setNumWords(int n) {
  numWords = n;
}

int
lemur::distrib::QryBasedSampler::getNumWords() const {
  return numWords;
}

void 
lemur::distrib::QryBasedSampler::setNumQueries(int n) {
  numQueries = n;
}

int
lemur::distrib::QryBasedSampler::getNumQueries() const {
  return numQueries;
}


void
lemur::distrib::QryBasedSampler::setTermMode(int m) {
  termMode = m;
}

int
lemur::distrib::QryBasedSampler::getTermMode() const {
  return termMode;
}
