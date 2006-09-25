/*==========================================================================
 * Copyright (c) 2004 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/

/*! \page BuildIndex
<P>
 This application builds an index for a collection of documents.
<P>
To use it, follow the general steps of running a lemur application.
<p>
The parameters are:
<p>
<ol>
<li> <tt>index</tt>: name of the index table-of-content file without the
extension.
<li> <tt>indexType</tt>: the type of index, key (KeyfileIncIndex), indri (LemurIndriIndex)
<li> <tt>memory</tt>: memory (in bytes) of KeyfileIncIndex cache (def = 128000000).
<li> <tt>stopwords</tt>: name of file containing the stopword list.
<li> <tt>acronyms</tt>: name of file containing the acronym list.
<li> <tt>countStopWords</tt>: If true, count stopwords in document length.
<li> <tt>docFormat</tt>: 
<ul>
<li> "trec" for standard TREC formatted documents 
<li> "web" for web TREC formatted documents
<li> "chinese" for segmented Chinese text (TREC format, GB encoding)
<li> "chinesechar" for unsegmented Chinese text (TREC format, GB encoding)
<li> "arabic" for Arabic text (TREC format, Windows CP1256 encoding)
</ul>
<li> <tt>stemmer</tt>: 
<ul>
<li> "porter" Porter stemmer.
<li> "krovetz" Krovetz stemmer,
<li> "arabic" arabic stemmer, requires additional parameters
<ol>
<li> <tt>arabicStemFunc</tt>: Which stemming algorithm to apply, one of:
<ul>
<li> arabic_stop          : arabic_stop
<li>  arabic_norm2         : table normalization
<li>  arabic_norm2_stop    : table normalization with stopping
<li>  arabic_light10       : light9 plus ll prefix
<li>  arabic_light10_stop  : light10 and remove stop words
</ul> 
</ol>
</ul>
<li> <tt>dataFiles</tt>: name of file containing list of datafiles to index.
</ol>
*/

#include "TextHandlerManager.hpp"
#include "IndriTextHandler.hpp"
#include "KeyfileTextHandler.hpp"
#include "KeyfileIncIndex.hpp"
#include "Param.hpp"
#include "indri/Path.hpp"

using namespace lemur::api;

// Local parameters used by the indexer 
namespace LocalParameter {
  int memory;

  // name (minus extension) of the database
  string index;
  // type of index to build
  string indexType;
  // name of file containing stopwords
  string stopwords;
  // name of file containing acronyms
  string acronyms;
  // format of documents (trec or web)
  string docFormat;
  // whether or not to stem
  string stemmer;
  // file with source files
  string dataFiles;

  bool countStopWords;

  void get() {
    index = ParamGetString("index");
    indexType = ParamGetString("indexType");
    memory = ParamGetInt("memory", 128000000);
    stopwords = ParamGetString("stopwords");
    acronyms = ParamGetString("acronyms");
    docFormat = ParamGetString("docFormat");
    dataFiles = ParamGetString("dataFiles");
    stemmer = ParamGetString("stemmer");
    countStopWords = (ParamGetString("countStopWords", "false") == "true");
  }
};

// put more usage info in here
void usage(int argc, char ** argv) {
  cerr << "Usage:" << endl
       << argv[0] << " paramfile datfile1 datfile2 ... " << endl
       << endl
       << "BuildIndex builds an index using TextHandler parsers." 
       << endl
       << "Summary of parameters:" << endl << endl
       << "\tindex - name of the index (without the index's extension)" << endl
       << "\tindexType - type of index to build (key,indri)" << endl
       << "\tmemory - memory (in bytes) of KeyfileIncIndex cache (def = 128000000)" << endl

       << "\tstopwords - name of file containing stopword list" << endl
       << "\t            Words in this file should be one per line." << endl
       << "\t            If this parameter is not specified, all words " << endl
       << "\t            are indexed." << endl
       << "\tacronyms - name of file containing acronym list (one word" << endl
       << "\t           per line).  Uppercase words recognized as acronyms" << endl
       << "\t           (eg USA U.S.A. USAs USA's U.S.A.) are left " << endl
       << "\t           uppercase if in the acronym list." << endl
       << "\t           (except for Indri index, where acronyms are not yet supported.)" << endl
       << "\tdocFormat - \"trec\" for standard TREC formatted documents "<< endl
       << "\t            \"web\" for web TREC formatted documents " << endl
       << "\t            \"reuters\" for Reuters XML documents " << endl
       << "\t            \"arabic\" for Arabic (Windows CP1256). " << endl  
       << "\t            \"chinese\" for for segmented Chinese (GB)." << endl  
       << "\t            \"chinesechar\" for unsegmented Chinese (GB)." << endl
       << "\tstemmer - \"porter\" to use Porter's stemmer. " << endl
       << "\t          \"krovetz\" to use Krovetz's stemmer (kstemmer). " << endl  
       << "\t          \"arabic\" to use an Arabic stemmer. " << endl  
       << "\t          (def = no stemmer)" << endl
       << "\tarabicStemFunc - which Arabic stemmer algorithm. " << endl
       << "\tcountStopWords - \"true\" to count stopwords in doc length. " << endl
       << "\t                 (def = false)" << endl
       << "\tdataFiles - name of file containing list of datafiles. " << endl
       << "\t              (one line per datafile name) " << endl
       << "\t              if not specified, enter datafiles on command line. "<< endl;
}

// get application parameters
void GetAppParam() {
  LocalParameter::get();
}


int AppMain(int argc, char * argv[]) {
  if ((argc < 3) && LocalParameter::dataFiles.empty()) {
    usage(argc, argv);
    return -1;
  }
  
  // Cannot create anything without Index name
  if (LocalParameter::index.empty()) {
    LEMUR_THROW(LEMUR_MISSING_PARAMETER_ERROR, "Please provide a name for the index you want to build. \nCheck the \"index\" parameter.");
  }

  if (LocalParameter::indexType.empty()) {
    LEMUR_THROW(LEMUR_MISSING_PARAMETER_ERROR, "Please provide a type for the index you want to build. \nCheck the \"indexType\" parameter. \nValid values are \"key\", or \"indri\" ");
  }

  // Create the appropriate parser and acronyms list if needed
  Parser * parser = NULL;
  parser = TextHandlerManager::createParser(LocalParameter::docFormat, 
                                            LocalParameter::acronyms);
  // if failed to create parser, abort
  if (!parser) {
    LEMUR_THROW(LEMUR_MISSING_PARAMETER_ERROR, "Please use a valid value for the required parameter \"docFormat\". Valid values are \"trec\", \"web\", \"reuters\",\"chinese\", \"chinesechar\", and \"arabic\". See program usage or Lemur documentation for more information.");
  }
  
  // Create the stopper if needed.
  Stopper * stopper = NULL;
  try {
    stopper = TextHandlerManager::createStopper(LocalParameter::stopwords);
  } catch (Exception &ex) {
    ex.writeMessage();
    cerr << "WARNING: BuildIndex continuing without stop words file loaded." << endl << "To omit stop words, check the \"stopwords\" parameter." << endl;
  }
  
  // Create the stemmer if needed.
  Stemmer * stemmer = NULL;
  try {
    stemmer = TextHandlerManager::createStemmer(LocalParameter::stemmer);
  } catch (Exception &ex) {
    ex.writeMessage();
    cerr << "WARNING: BuildIndex continuing without stemmer." << endl << "To use a stemmer, check the \"stemmer\" and other supporting parameters." << endl << "See program usage or Lemur documentation for more information.";
  }

  TextHandler* indexer;
  lemur::index::KeyfileIncIndex* index = NULL;

  if (LocalParameter::indexType == "indri") {
    indexer = new lemur::parse::IndriTextHandler(LocalParameter::index,
                                   LocalParameter::memory,
                                   parser);
  } else if (LocalParameter::indexType == "key") {
    index = new lemur::index::KeyfileIncIndex(LocalParameter::index,
                                LocalParameter::memory);
    indexer = new lemur::parse::KeyfileTextHandler(index,
                                     LocalParameter::countStopWords);
  } else {
    LEMUR_THROW(LEMUR_BAD_PARAMETER_ERROR,"Please use a valid value for the required parameter \"IndexType\". \nValid values are \"key\" or \"indri\"See program usage or Lemur documentation for more information.");
  }
  
  // chain the parser/stopper/stemmer/indexer
  TextHandler * th = parser;

  if (stopper != NULL) {
    th->setTextHandler(stopper);
    th = stopper;
  }

  if (stemmer != NULL) {
    th->setTextHandler(stemmer);
    th = stemmer;
  } 

  th->setTextHandler(indexer);

  // parse the data files
  if (!LocalParameter::dataFiles.empty()) {
    if (!indri::file::Path::exists(LocalParameter::dataFiles)) {
      LEMUR_THROW(LEMUR_IO_ERROR, "\"dataFiles\" specified does not exist");
    }
    ifstream source(LocalParameter::dataFiles.c_str());
    if (!source.is_open()) {
      LEMUR_THROW(LEMUR_IO_ERROR,"could not open \"dataFiles\" specified");
    } else {
      string filename;
      while (getline(source, filename)) {
        cerr << "Parsing file: " << filename <<endl;
        try {
          parser->parse(filename);
        } catch (Exception &ex) {
          LEMUR_RETHROW(ex,"Could not parse file");
        }
      }
    }
  } else {
    for (int i = 2; i < argc; i++) {
      cerr << "Parsing file: " << argv[i] << endl;
      string filename(argv[i]);
      try {
        parser->parse(filename);
      } catch (Exception &ex) {
        LEMUR_RETHROW(ex, "Could not parse file");
      }
    }
  }
  // free memory
  delete(indexer);
  delete(stemmer);
  delete(stopper);
  delete(parser);
  if (index)
    delete(index);
  return 0;
}

