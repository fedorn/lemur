/*==========================================================================
 * Copyright (c) 2002 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/

/// A Passage FP Indexer
/*! \page PassageIndexer Passage FP Indexer
<P>
 This application builds an FP passage index for a collection of documents.  
Documents are segmented into passages of size <tt>passageSize</tt> with an 
overlap of <tt>passageSize/2</tt> terms per passage. 
<P>
To use it, follow the general steps of running a lemur application.
<p>
The parameters are:
<p>
<ol>
<li> <tt>index</tt>: name of the index table-of-content file without the
.ifp extension.
<li> <tt>memory</tt>: memory (in bytes) of InvFPPushIndex (def = 96000000).
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
<li> "krovetz" Krovetz stemmer, requires additional parameters
<ol>
<li> <tt>KstemmerDir</tt>: Path to directory of data files used by Krovetz's stemmer.
</ol>
<li> "arabic" arabic stemmer, requires additional parameters
<ol>
<li> <tt>arabicStemDir</tt>: Path to directory of data files used by the Arabic stemmers.
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
<li> <tt>passageSize</tt>: Number of terms per passage.
</ol>
 */
/*
  author: dmf
 */

#include "TextHandlerManager.hpp"
#include "InvPassageTextHandler.hpp"
#include "Param.hpp"
#include "indri/Path.hpp"
#include "TrecParser.hpp"

using namespace lemur::api;

// Local parameters used by the indexer 
namespace LocalParameter {
  int memory;
  // name (minus extension) of the database
  string index;
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
  int psgSize;

  void get() {
    index = ParamGetString("index");
    memory = ParamGetInt("memory", 96000000);
    psgSize = ParamGetInt("passageSize", 50);
    stopwords = ParamGetString("stopwords");
    acronyms = ParamGetString("acronyms");
    docFormat = ParamGetString("docFormat");
    dataFiles = ParamGetString("dataFiles");
    stemmer = ParamGetString("stemmer");
  }
};

// put more usage info in here
void usage(int argc, char ** argv) {
  cerr << "Usage:" << endl
       << argv[0] << " paramfile [datfile1] [datfile2] ... " << endl
       << endl
       << "PassageIndexer builds a database using either a Parser " 
       << "and InvPassagePushIndex." << endl
       << "Summary of parameters:" << endl << endl
       << "\tindex - name of the index (without the .ifp extension)" << endl
       << "\tmemory - memory (in bytes) of InvPassagePushIndex (def = 96000000)" << endl
       << "\tpassageSize - passage size (number of words) (def = 50)." << endl
       << "\tstopwords - name of file containing stopword list" << endl
       << "\t            Words in this file should be one per line." << endl
       << "\t            If this parameter is not specified, all words " << endl
       << "\t            are indexed." << endl
       << "\tacronyms - name of file containing acronym list (one word" << endl
       << "\t           per line).  Uppercase words recognized as acronyms" << endl
       << "\t           (eg USA U.S.A. USAs USA's U.S.A.) are left " << endl
       << "\t           uppercase if in the acronym list." << endl
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
       << "\tKstemmerDir - pathname to data files used by kstemmer. " << endl
       << "\tarabicStemDir - pathname to data files used by the Arabic stemmers. " << endl
       << "\tarabicStemFunc - which Arabic stemmer algorithm. " << endl
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

  // Create the appropriate parser and acronyms list if needed
  Parser * parser = NULL;
  parser = TextHandlerManager::createParser(LocalParameter::docFormat, 
					    LocalParameter::acronyms);
  // if failed to create parser, create a default
  // Should not do this, if no parser is created, app should say
  // so and exit... dmf 01/2004. #include "TrecParser.hpp" can
  // be removed if this is.
  if (!parser)
    parser = new lemur::parse::TrecParser();
  
  // Create the stopper if needed.
  Stopper * stopper = NULL;
  stopper = TextHandlerManager::createStopper(LocalParameter::stopwords);

  // Create the stemmer if needed.
  Stemmer * stemmer = NULL;
  stemmer = TextHandlerManager::createStemmer(LocalParameter::stemmer);

  // Create the indexer. (Note: this has an InvFPPushIndex that 
  // it uses to do the indexing, but InvFPTextHandler implements the
  // TextHandler class, so that it is compatible with my parser
  // architecture.  See the TextHandler and InvFPTextHandler classes
  // for more info.)
  
  if (LocalParameter::index.empty()) {
    throw Exception("PassageIndexer", "index must be specified");
  }

  lemur::parse::InvPassageTextHandler* indexer;
  indexer = new lemur::parse::InvPassageTextHandler(LocalParameter::index,
				      LocalParameter::psgSize,
				      LocalParameter::memory);

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
    if (!indri::file::Path::exists(LocalParameter::dataFiles.c_str())) {
      throw Exception("PassageIndexer", "dataFiles specified does not exist");
    }

    ifstream source(LocalParameter::dataFiles.c_str());
    if (!source.is_open()) {
      throw Exception("PassageIndexer","could not open dataFiles specified");
    } else {
      string filename;
      while (getline(source, filename)) {
	cerr << "Parsing " << filename <<endl;
	parser->parse(filename);
      }
    }
  } else {
    for (int i = 2; i < argc; i++) {
      cerr << "Parsing " << argv[i] << endl;
      string filename(argv[i]);
    if (!indri::file::Path::exists(filename)) {
      throw Exception("PassageIndexer", "datfile specified does not exist");
    }
      parser->parse(filename);
    }
  }
  // free memory
  delete(stopper);
  delete(stemmer);
  delete(parser);
  delete(indexer);
  return 0;
}


