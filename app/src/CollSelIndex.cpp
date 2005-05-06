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
#include "TextHandlerManager.hpp"
#include "DocFreqIndexer.hpp"
#include "CtfIndexer.hpp"
#include "Param.hpp"
#include "TrecParser.hpp"

using namespace lemur::api;

namespace LocalParameter {
  string df;
  string ctf;
  int memory;
  string stopwords;
  string acronyms;
  string docFormat;
  string dfDocs;
  string dfCounts;
  string stemmer;

  bool countStopWds;

  void get() {
    df = ParamGetString("dfIndex", "collSel_df");
    ctf = ParamGetString("ctfIndex", "collSel_ctf");
    memory = ParamGetInt("memory", 64000000);
    stopwords = ParamGetString("stopwords");
    acronyms = ParamGetString("acronyms");
    docFormat = ParamGetString("docFormat");
    dfCounts = ParamGetString("dfCounts", "collSel_df.cw");
    dfDocs = ParamGetString("dfDocs", "collSel_df.cd");

    stemmer = ParamGetString("stemmer");
    countStopWds = (ParamGetString("countStopWords", "false") == "true" 
                    ? true : false);

  }
};


void usage(int argc, char ** argv) {
  cerr << "Usage:" << endl
       << argv[0] << " paramfile datfile1 datfile2 ... " << endl;    

}

void GetAppParam() {
  LocalParameter::get();
}


int AppMain(int argc, char * argv[]) {
  if (argc < 3) {
    usage(argc, argv);
    return -1;
  }

  // Create the appropriate parser and acronyms list if needed
  Parser * parser = NULL;
  parser = TextHandlerManager::createParser(LocalParameter::docFormat, 
                                            LocalParameter::acronyms);
  // if failed to create parser, create a default
  if (!parser)
    parser = new lemur::parse::TrecParser();
  
  // Create the stopper if needed.
  Stopper * stopper = NULL;
  stopper = TextHandlerManager::createStopper(LocalParameter::stopwords);

  // Create the stemmer if needed.
  Stemmer * stemmer = NULL;
  stemmer = TextHandlerManager::createStemmer(LocalParameter::stemmer);

  lemur::distrib::DocFreqIndexer dfIndexer(LocalParameter::df,
                           LocalParameter::dfCounts,
                           LocalParameter::dfDocs,
                           LocalParameter::memory,
                           LocalParameter::countStopWds);

  lemur::distrib::CtfIndexer ctfIndexer(LocalParameter::ctf,
                        LocalParameter::memory,
                        LocalParameter::countStopWds);

  // chain the parser/stopper/stemmer/indexer

  TextHandler * th = parser;

  //  parser->setTextHandler(stopper);

  if (stopper != NULL) {
    th->setTextHandler(stopper);
    th = stopper;
  }

  if (stemmer != NULL) {
    th->setTextHandler(stemmer);
    th = stemmer;
  } 
  
  th->setTextHandler(&dfIndexer);
  dfIndexer.setTextHandler(&ctfIndexer);

  cout << (LocalParameter::countStopWds == true ? "true" : "false") << endl;

  for (int i = 2; i < argc; i++) {
    char * n = strdup(argv[i]);
    char * name = n + strlen(n);
    char * t = name;
    while (name > n && *name != '/') name--;
    if (*name == '/') {
      name++;
    }
    while (t > name && *t != '.') t--;
    if (t > name) *t = '\0';

    cerr << "indexing: " << argv[i] << endl;
    cerr << "collection: " << name << endl;
    
    dfIndexer.newDb(name);
    ctfIndexer.newDb(name);
    string filename(argv[i]);
    parser->parse(filename);

    free (n);

  }

  if (stopper != NULL) delete stopper;
  delete parser;
  return 0;
}

