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

#include "TextHandlerManager.hpp"
#include "IndexManager.hpp"
#include "Index.hpp"
#include "QueryTextHandler.hpp"
#include "WriterTextHandler.hpp"
#include "FUtil.hpp"

#include "Param.hpp"

// Local parameters used by the indexer 
namespace LocalParameter {

  // name of the database
  char * index;
  // name of file containing stopwords
  char * stopwords;
  // name of file containing acronyms
  char * acronyms;
  // format of documents (trec or web)
  char * docFormat;
  // stemmer to use
  char * stemmer;
  // query output file
  char * qryOutFile;

  void get() {
    // my code uses char *'s while the param utils use
    // strings.  maybe I should convert to strings...

    index = strdup(ParamGetString("index"));
    stopwords = strdup(ParamGetString("stopwords"));
    acronyms = strdup(ParamGetString("acronyms"));
    docFormat = strdup(ParamGetString("docFormat"));
    stemmer = strdup(ParamGetString("stemmer"));
    qryOutFile = strdup(ParamGetString("qryOutFile"));
  }

  // free the memory allocated in get()
  void freeMem() {
    free(index);
    free(stopwords);
    free(acronyms);
    free(docFormat);
    free(stemmer);
    free(qryOutFile);
  }
};

// put more usage info in here
void usage(int argc, char ** argv) {
  cerr << "Usage:" << endl
       << argv[0] << " paramfile datfile1 datfile2 ... " << endl
       << endl
       << "ParseQuery parses queries using either the TrecParser, " << endl
       << "WebParser or Chinese(Char)Parser class and Index."
       << endl << "Summary of parameters:" << endl << endl
       << "\tindex - name of the index" << endl
       << "\tqryOutFile - name of output file" << endl
       << "\tstopwords - name of file containing stopword list" << endl
       << "\t            Words in this file should be one per line." << endl
       << "\t            If this parameter is not specified, all words " << endl
       << "\t            are indexed." << endl
       << "\tacronyms - name of file containing acronym list (one word" << endl
       << "\t           per line).  Uppercase words recognized as acronyms" << endl
       << "\t           (eg USA U.S.A. USAs USA's U.S.A.) are left " << endl
       << "\t           uppercase as USA if USA is in the acronym list." << endl
       << "\tdocFormat - \"trec\" for standard TREC formatted documents "<< endl
       << "\t            web for web TREC formatted documents (def = trec)" << endl
       << "\t            \"reuters\" for Reuters XML documents " << endl
       << "\t            \"arabic\" for Arabic (Windows CP1256). " << endl  
       << "\t            \"chinese\" for for segmented Chinese (GB)." << endl  
       << "\t            \"chinesechar\" for unsegmented Chinese (GB)." << endl
       << "\tstemmer - \"porter\" to use Porter's stemmer. " << endl
       << "\t          \"krovetz\" to use Krovetz's stemmer (kstemmer). " << endl  
       << "\t          \"arabic\" to use an Arabic stemmer. " << endl  
       << "\t          (def = no stemmer)" << endl
       << "\tKstemmerDir - pathname to data files used by kstemmer. " << endl
       << "\tarabicStemDir - pathname to data files used by the Arabic stemmers. " << endl
       << "\tarabicStemFunc - which Arabic stemmer algorithm. " << endl;
}

// get application parameters
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
    parser = new TrecParser();
  
  // Create the stopper if needed.
  Stopper * stopper = NULL;
  stopper = TextHandlerManager::createStopper(LocalParameter::stopwords);

  // Create the stemmer if needed.
  Stemmer * stemmer = NULL;
  stemmer = TextHandlerManager::createStemmer(LocalParameter::stemmer);

  if (!strcmp(LocalParameter::index, "")) {
    throw Exception("ParseQuery", "index must be specified");
  }
  Index * ind = IndexManager::openIndex(LocalParameter::index);

  QueryTextHandler queryHandler;
  queryHandler.setIndex(ind);
  
  WriterTextHandler writer(LocalParameter::qryOutFile);

  // chain the parser/queryHandler/stopper/stemmer/writer

  TextHandler * th = parser;

  th->setTextHandler(&queryHandler);
  th = &queryHandler;

  if (stopper != NULL) {
    th->setTextHandler(stopper);
    th = stopper;
  }

  if (stemmer != NULL) {
    th->setTextHandler(stemmer);
    th = stemmer;
  } 

  th->setTextHandler(&writer);

  // parse the data files
  for (int i = 2; i < argc; i++) {
    cerr << "Parsing " << argv[i] << endl;
    if (!fileExist(argv[i])) {
      throw Exception("ParseQuery", "datfile specified does not exist");
    }
    parser->parse(argv[i]);
  }

  // free memory
  if (stopper != NULL) delete stopper;
  delete parser;
  delete ind;
  LocalParameter::freeMem();
  return 0;
}

