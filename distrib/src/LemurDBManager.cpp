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

#include "LemurDBManager.hpp"

#include "StringQuery.hpp"

#include "Param.hpp"

using namespace lemur::api;

// Open the database.
void
lemur::distrib::LemurDBManager::open(const string &dbname) {
  // Load the parameter file 
  ParamPushFile(dbname);
  RetrievalParameter::get();

  // Create the index
  index = IndexManager::openIndex(RetrievalParameter::databaseIndex);
  // Create the parser -- DON'T CHANGE THIS 
  parser = new LemurMemParser(index);
  // Create the results list
  accumulator = new lemur::retrieval::ArrayAccumulator(index->docCount());
  results = new IndexedRealVector(index->docCount());
  model = RetMethodManager::createModel(index, accumulator,
                                        RetrievalParameter::retModel);
  ParamPopFile();
  outfile = NULL;
}

// Return the parser. -- DON'T CHANGE THIS
lemur::distrib::MemParser *
lemur::distrib::LemurDBManager::getParser() const {
  return parser;
}


// Query the database.
lemur::distrib::results_t *
lemur::distrib::LemurDBManager::query(const char * query, int numdocs) const {
  // Create the return value container.
  results_t * res = new results_t;
  res->ids = new docid_t[numdocs];

  // Create the query object.
  lemur::parse::StringQuery qry(query);

  // Create the query representation.
  QueryRep * qryRep = model->computeQueryRep(qry); 
  // Retrieve results.

  model->scoreCollection(*qryRep, *results);

  results->Sort();


  IndexedRealVector::iterator j;
  j = results->begin();
  int i;
  for (i = 0; (i < numdocs) && (j != results->end()); i++) {
    // Place document id in the return value.
    //    res->ids[i] = strdup(index->document(j->ind));    
    res->ids[i] = index->document(j->ind);    
    j++;
  }
  res->numDocs = i;

  // Free the query representation.
  delete qryRep;

  // Return the results.
  return res;
}

// Return a document given its id.  The doc part of the return value
// structure is really an array of integer term ids.
lemur::distrib::doc_t *
lemur::distrib::LemurDBManager::getDoc(docid_t docid) const {
  int lemurID = index->document(docid);

  // Create the return value.
  doc_t * doc = new doc_t;
  //  doc->id = strdup(docid);
  doc->id = docid;
 
  // Create thte doc contents part.
  int intlen = index->docLength(lemurID);
  int sizeratio = sizeof(int) / sizeof(char);
  int bytelen = intlen * sizeratio + 1;
  doc->doc = new char[bytelen];
  int * intdoc = (int *) doc->doc;
  int * loc = intdoc;
  bytelen = 0;

  // Write the term ids to the doc.
  TermInfoList * til = index->termInfoList(lemurID);
  while (til->hasMore()) {
    TermInfo * ti = til->nextEntry();
    for (int i = 0; i < ti->count(); i++) {
      *loc = ti->termID();
      loc++;
      bytelen += sizeratio; 
    }
  }
  delete til;

  doc->len = bytelen;

  return doc;
}

void
lemur::distrib::LemurDBManager::setOutputFile(const string &filename) const {
  if (outfile != NULL) {
    outfile->close();
    delete outfile;
    outfile = NULL;
  }
  outfile = new ofstream(filename.c_str());
}

// Write a doc to the end of the file
void
lemur::distrib::LemurDBManager::output(const docid_t docid) const {
  int c = 0;

  int lemurID = index->document(docid);
  
  *outfile << "<DOC>\n<DOCNO> " << docid << " </DOCNO>\n<TEXT>\n";
  // Write the term ids to the file.
  TermInfoList * til = index->termInfoList(lemurID);
  while (til->hasMore()) {
    TermInfo * ti = til->nextEntry();
    for (int i = 0; i < ti->count(); i++) {
      if (c % 10 == 0) *outfile << "\n";
      *outfile << index->term(ti->termID()) << " ";
      c++;
    }
  }
  delete til;

  *outfile << "\n</TEXT>\n</DOC>\n";
}



// Close the database and free memory.
void
lemur::distrib::LemurDBManager::close() {
  delete index;
  delete model;
  delete accumulator;
  delete results;
  // this should be deleted.
  delete parser;
  outfile->close();
  delete outfile;
}
