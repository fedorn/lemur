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

/// An InvFP Indexer
/*! \page BuildPropIndex InvFP Indexer
<P>
 This application builds an InvFP index for a collection of documents with
properties associated with terms.
<P>
To use it, follow the general steps of running a lemur application.
<p>
The parameters are:
<p>
<ol>
<li> <tt>index</tt>: name of the index to create (don't include extension)
<li> <tt>indexType</tt>:the type of index to create, "key" (KeyfileIncIndex) or "inv" (InvFPIndex). default is inv
<li> <tt>memory</tt>: memory (in bytes) of InvFPPushIndex cache (def = 96000000).
<li> <tt>stopwords</tt>: name of file containing the stopword list.
<li> <tt>acronyms</tt>: name of file containing the acronym list.
<li> <tt>countStopWords</tt>: If true, count stopwords in document length.
<li> <tt>docFormat</tt>: 
<ul>
<li> "brill" for documents with Brill's part of speech tags, still needs DOC separators between documents similar to Lemur's WebParser. This is the default.
<li> "identifinder" for documents with Identifinder's named entity tags, still needs DOC separators between documents similar to Lemur's WebParser. 
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
</ol>
 */
#include "TextHandlerManager.hpp"
#include "BrillPOSTokenizer.hpp"
#include "BrillPOSParser.hpp"
#include "PropIndexTH.hpp"
#include "Param.hpp"
#include "FUtil.hpp"

// Local parameters used by the indexer 
namespace LocalParameter {
  int memory;

  // name (minus extension) of the database
  string index;
  string indexType;
  // name of file containing stopwords
  string stopwords;
  // name of file containing acronyms
  string acronyms;
  // whether or not to stem
  string stemmer;
  // file with source files
  string dataFiles;
  string docFormat;
  bool countStopWords;

  void get() {
    docFormat = ParamGetString("docFormat");
    index = ParamGetString("index");
    indexType = ParamGetString("indexType", "inv");
    memory = ParamGetInt("memory", 96000000);
    stopwords = ParamGetString("stopwords");
    acronyms = ParamGetString("acronyms");
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
       << "BuildPropIndex builds a database with properties PropIndexTH." 
       << endl
       << "Summary of parameters:" << endl << endl
       << "\tindex - name of the index to create (don't include extension)"<< endl
       << "\tindexType - the type of index, one of key (for KeyfileIncIndex) "
       << "\tor inv (for InvFPPushIndex) respectively. (def = inv)" << endl
       << "\tmemory - memory (in bytes) of InvFPPushIndex cache (def = 96000000)." << endl
       << "\tstopwords - name of file containing stopword list" << endl
       << "\t            Words in this file should be one per line." << endl
       << "\t            If this parameter is not specified, all words " << endl
       << "\t            are indexed." << endl
       << "\tacronyms - name of file containing acronym list (one word" << endl
       << "\t           per line).  Uppercase words recognized as acronyms" << endl
       << "\t           (eg USA U.S.A. USAs USA's U.S.A.) are left " << endl
       << "\t           uppercase if in the acronym list." << endl
       << "\tdocFormat - \"brill\" for documents with Brill's part of speech tags, still needs DOC separators between documents similar to Lemur's WebParser. This is the default." << endl 
       << "\t            \"identifinder\" for documents with Identifinder's named entity tags, still needs DOC separators between documents similar to Lemur's WebParser." << endl 
       << "\tstemmer - \"porter\" to use Porter's stemmer. " << endl
       << "\t          \"krovetz\" to use Krovetz's stemmer (kstemmer). " << endl  
       << "\t          \"arabic\" to use an Arabic stemmer. " << endl  
       << "\t          (def = no stemmer)" << endl
       << "\tKstemmerDir - pathname to data files used by kstemmer. " << endl
       << "\tarabicStemDir - pathname to data files used by the Arabic stemmers. " << endl
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
  
  // Create the Parser
  Parser* parser = NULL;
  TextHandler* th;
  BrillPOSTokenizer* tok = NULL;
  parser = TextHandlerManager::createParser(LocalParameter::docFormat, 
					    LocalParameter::acronyms);

  if (!parser) {
    parser = new BrillPOSParser();
    tok = new BrillPOSTokenizer();
    parser->setTextHandler(tok);
    th = tok;
  } else {
    if (LocalParameter::docFormat == "brill") {
      tok = new BrillPOSTokenizer();
      parser->setTextHandler(tok);
      th = tok;
    } else {
      th = parser;
    }
  }
  
  // Create the stopper if needed.
  Stopper * stopper = NULL;
  stopper = TextHandlerManager::createStopper(LocalParameter::stopwords);

  // Create the stemmer if needed.
  Stemmer * stemmer = NULL;
  stemmer = TextHandlerManager::createStemmer(LocalParameter::stemmer);

  // Create the indexer. (Note: this has an InvFPPushIndex that 
  // it uses to do the indexing, but PropIndexTH implements the
  // TextHandler class, so that it is compatible with my parser
  // architecture.  See the TextHandler and InvFPTextHandler classes
  // for more info.)
  PropIndexTH indexer(LocalParameter::index, 
		      LocalParameter::memory, 
		      LocalParameter::countStopWords, 
		      LocalParameter::indexType);

  // chain the parser/stopper/stemmer/indexer
  if (stopper) {
    th->setTextHandler(stopper);
    th = stopper;
  }

  if (stemmer) {
    th->setTextHandler(stemmer);
    th = stemmer;
  } 

  th->setTextHandler(&indexer);

  // parse the data files
  if (!LocalParameter::dataFiles.empty()) {
    if (!fileExist(LocalParameter::dataFiles)) {
      throw Exception("PushIndexer", "dataFiles specified does not exist");
    }
    ifstream source(LocalParameter::dataFiles.c_str());
    if (!source.is_open()) {
      throw Exception("PushIndexer","could not open dataFiles specified");
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
      parser->parse(filename);
    }
  }
  // free memory
  delete(stopper);
  delete(stemmer);
  delete(tok);
  //  delete tagger;
  delete(parser);
  return 0;
}

