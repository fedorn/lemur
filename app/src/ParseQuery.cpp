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

#include "TextHandlerManager.hpp"
#include "IndexManager.hpp"
#include "QueryTextHandler.hpp"
#include "WriterTextHandler.hpp"
#include "indri/Path.hpp"
#include "Param.hpp"

using namespace lemur::api;

// Local parameters used by the indexer 
namespace LocalParameter {

  // name of the database
  string index;
  // name of file containing stopwords
  string stopwords;
  // name of file containing acronyms
  string acronyms;
  // format of documents (trec or web)
  string docFormat;
  // stemmer to use
  string stemmer;
  // query output file
  string qryOutFile;

  void get() {
    index = ParamGetString("index");
    stopwords = ParamGetString("stopwords");
    acronyms = ParamGetString("acronyms");
    docFormat = ParamGetString("docFormat");
    stemmer = ParamGetString("stemmer");
    qryOutFile = ParamGetString("qryOutFile");
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
  if (!parser)
      throw Exception("ParseQuery", "Unable to create parser for docFormat");
  
  // Create the stopper if needed.
  Stopper * stopper = NULL;
  stopper = TextHandlerManager::createStopper(LocalParameter::stopwords);

  // Create the stemmer if needed.
  Stemmer * stemmer = NULL;
  stemmer = TextHandlerManager::createStemmer(LocalParameter::stemmer);

  if (LocalParameter::index.empty()) {
    throw Exception("ParseQuery", "index must be specified");
  }
  Index * ind = IndexManager::openIndex(LocalParameter::index.c_str());

  lemur::parse::QueryTextHandler queryHandler;
  queryHandler.setIndex(ind);
  
  lemur::parse::WriterTextHandler writer(LocalParameter::qryOutFile);

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
    string filename(argv[i]);
    if (!indri::file::Path::exists(filename)) {
      throw Exception("ParseQuery", "datfile specified does not exist");
    }
    parser->parse(filename);
  }

  // free memory
  delete(stopper);
  delete(stemmer);
  delete(parser);
  delete(ind);
  return 0;
}

