/*==========================================================================
 * Copyright (c) 2003 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/

#include "DocMgrManager.hpp"
#include "TextHandlerManager.hpp"
#include "InvFPTextHandler.hpp"
#include "KeyfileTextHandler.hpp"
#include "KeyfileIncIndex.hpp"
#include "Param.hpp"
#include "Exception.hpp"

using namespace lemur::api;

// Local parameters used by the indexer 
namespace LocalParameter {
  int memory;
  string index;  // optional name (minus extension) of the database
  string indexType;  // index type (one of key or inv)
  string manager; // name of document manager
  string mgrType; // which doc mgr to use
  string stopwords;  // name of file containing stopwords
  string acronyms;   // name of file containing acronyms
  string docFormat;  // format of documents (trec or web)
  string stemmer;  // what stemmer to use
  int position;  //whether to keep positions and dtindex
  string dataFiles;  // file with source files
  bool countStopWords;

  void get() {
    index = ParamGetString("index");
    indexType = ParamGetString("indexType", "inv");
    manager = ParamGetString("manager");
    // default is FlattextDocMgr.
    mgrType = ParamGetString("managerType", "flat");
    memory = ParamGetInt("memory", 96000000);
    stopwords = ParamGetString("stopwords");
    acronyms = ParamGetString("acronyms");
    docFormat = ParamGetString("docFormat");
    stemmer = ParamGetString("stemmer");
    dataFiles = ParamGetString("dataFiles");
    position = ParamGetInt("position", 1);
    countStopWords = (ParamGetString("countStopWords", "false") == "true");
  }
};

// put more usage info in here
void usage(int argc, char ** argv) {
  cerr << "Usage:" << endl
       << argv[0] << " paramfile " << endl
       << endl
       << "BuildDocMgr builds a document manager. Builds an index at the end of chaing if an index name is provided."
       << endl
       << "Summary of parameters:" << endl << endl
       << "\tmanager -  required name of the document manager (without extension)" << endl    
       << "\tmanagerType -  required name of the document manager type, one of flat (FlatfileDocMgr) bdm (KeyfileDocMgr) or elem (ElemDocMgr)." << endl    
       << "\tdocFormat - \"trec\" for standard TREC formatted documents "<< endl
       << "\t            \"web\" for web TREC formatted documents " << endl
       << "\t            \"reuters\" for Reuters XML documents " << endl
       << "\t            \"arabic\" for Arabic (Windows CP1256). " << endl  
       << "\t            \"chinese\" for for segmented Chinese (GB)." << endl  
       << "\t            \"chinesechar\" for unsegmented Chinese (GB)." << endl
       << "\tdataFiles - name of file containing list of datafiles. " << endl
       << "\t              (one line per datafile name) " << endl
       << "\tFollowing parameters optional for building index" << endl
       << "\tindex - name of the index (without the extension)" << endl
       << "\tindexType - the type of index, one of key (for KeyfileIncIndex) "
       << "\tor inv (for Inv(FP)PushIndex) respectively." << endl
       << "\tmemory - memory (in bytes) of index cache (def = 96000000)" << endl
       << "\tposition - store position information (def = 1)" << endl
       << "\tstopwords - name of file containing stopword list" << endl
       << "\t            Words in this file should be one per line." << endl
       << "\t            If this parameter is not specified, all words " << endl
       << "\t            are indexed." << endl
       << "\tacronyms - name of file containing acronym list (one word" << endl
       << "\t           per line).  Uppercase words recognized as acronyms" << endl
       << "\t           (eg USA U.S.A. USAs USA's U.S.A.) are left " << endl
       << "\t           uppercase if in the acronym list." << endl
       << "\tstemmer - \"porter\" to use Porter's stemmer. " << endl
       << "\t          \"krovetz\" to use Krovetz's stemmer (kstemmer). " << endl  
       << "\t          \"arabic\" to use an Arabic stemmer. " << endl  
       << "\t          (def = no stemmer)" << endl
       << "\tKstemmerDir - pathname to data files used by kstemmer. " << endl
       << "\tarabicStemDir - pathname to data files used by the Arabic stemmers. " << endl
       << "\tarabicStemFunc - which Arabic stemmer algorithm. " << endl
       << "\tcountStopWords - \"true\" to count stopwords in doc length. " << endl
       << "\t                 (def = false)" << endl;
}

// get application parameters
void GetAppParam() {
  LocalParameter::get();
}


int AppMain(int argc, char * argv[]) {
  if (argc < 2) {
    usage(argc, argv);
    return -1;
  }

  // Create DocumentManager with appropriate parse mode
  DocumentManager* docmgr;
  // there's currently only two docmanagers
  docmgr = DocMgrManager::createDocMgr(LocalParameter::mgrType, 
				       LocalParameter::manager, 
				       LocalParameter::docFormat, 
				       LocalParameter::dataFiles);
  if (!docmgr)
    throw Exception ("BuildDocMgr", "\nInsufficient parameters for creating document manager. Check manager, dataFiles, and docFormat");

  // are we building an index?
  Stopper * stopper = NULL;
  Stemmer * stemmer = NULL;
  TextHandler* indexer = NULL;
  Index *ind = NULL;
  if (!LocalParameter::index.empty()) {
    // Create the stopper if needed.
    stopper = TextHandlerManager::createStopper(LocalParameter::stopwords);
    
    // Create the stemmer if needed.
    stemmer = TextHandlerManager::createStemmer(LocalParameter::stemmer);
    // needs to handle alternative index types...

    // Create the right type of indexer (DocMgrManager should handle this).
    // Need an abstract class to hang the setDocManager call on here...
    if (LocalParameter::indexType == "inv") {
      indexer = new lemur::parse::InvFPTextHandler(LocalParameter::index, 
				     LocalParameter::memory, 
				     LocalParameter::countStopWords, 
				     LocalParameter::position);
      // register document manager with PushIndex
      ((lemur::parse::InvFPTextHandler *)indexer)->setDocManager(docmgr->getMyID());

    } else if (LocalParameter::indexType == "key"){
      ind = new lemur::index::KeyfileIncIndex(LocalParameter::index,
				LocalParameter::memory);
      indexer = new lemur::parse::KeyfileTextHandler((lemur::index::KeyfileIncIndex *)ind,
				       LocalParameter::countStopWords);
      // register document manager with PushIndex
      ((lemur::parse::KeyfileTextHandler *)indexer)->setDocManager(docmgr->getMyID());
    } else {
    throw Exception ("BuildDocMgr", "Unknown index type");
    }
    
    
    // chain the document manager(parser)/stopper/stemmer/indexer    
    TextHandler *th = dynamic_cast<TextHandler*> (docmgr);
    if (!th)
      throw Exception ("BuildDocMgr", "This application currently only supports special DocumentManagers that also behave like TextHandlers, ie FlattextDocMgr, KeyfileDocMgr, ElemDocMgr");

    if (stopper != NULL) {
      th->setTextHandler(stopper);
      th = stopper;
    }
    
    if (stemmer != NULL) {
      th->setTextHandler(stemmer);
      th = stemmer;
    } 

    th->setTextHandler(indexer);
  }
  // parse the data files
  docmgr->buildMgr();
  
  // free memory
  delete(indexer);
  delete(stopper);
  delete(stemmer);
  delete(ind);
  delete(docmgr);
  return 0;
}
