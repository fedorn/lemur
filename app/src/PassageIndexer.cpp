/*==========================================================================
 * Copyright (c) 2002 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
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

#include "Stopper.hpp"
#include "PorterStemmer.hpp"
#include "KStemmer.hpp"
#include "ArabicStemmer.hpp"
#include "WebParser.hpp"
#include "TrecParser.hpp"
#include "ChineseParser.hpp"
#include "ChineseCharParser.hpp"
#include "ArabicParser.hpp"
#include "ReutersParser.hpp"
#include "InvPassageTextHandler.hpp"
#include "Param.hpp"
#include "FUtil.hpp"

// Local parameters used by the indexer 
namespace LocalParameter {
  int memory;
  // name (minus extension) of the database
  char * index;
  // name of file containing stopwords
  char * stopwords;
  // name of file containing acronyms
  char * acronyms;
  // format of documents (trec or web)
  char * docFormat;
  // whether or not to stem
  char * stemmer;
  // file with source files
  char * dataFiles;
  // path name to data files used by kstemmer
  char *kstemmer_dir;
  int psgSize;

  void get() {
    // my code uses char *'s while the param utils use
    // strings.  maybe I should convert to strings...

    index = strdup(ParamGetString("index"));
    memory = ParamGetInt("memory", 96000000);
    psgSize = ParamGetInt("passageSize", 50);
    stopwords = strdup(ParamGetString("stopwords"));
    acronyms = strdup(ParamGetString("acronyms"));
    docFormat = strdup(ParamGetString("docFormat"));
    dataFiles = strdup(ParamGetString("dataFiles"));

    // convert docFormat to lowercase
    for (char * d = docFormat; *d != '\0'; d++) *d = tolower(*d);

    stemmer = strdup(ParamGetString("stemmer"));
    // convert docFormat to lowercase
    for (char * e = stemmer; *e != '\0'; e++) *e = tolower(*e);
    if(!strcmp(stemmer, "krovetz")) {
      // Using kstemmer needs a path to data files
      if(strlen(ParamGetString("KstemmerDir"))>0) {
	// if KstemmerDir is declared then resets STEM_DIR otherwise 
	// uses the default
	kstemmer_dir = new char[MAX_FILENAME_LENGTH];
	kstemmer_dir[0]='\0';
	strcat(kstemmer_dir, "STEM_DIR=");
	strcat(kstemmer_dir,ParamGetString("KstemmerDir"));
	if(putenv(kstemmer_dir))
	  cout << "putenv can not set STEM_DIR" << endl;
      }    
    } else if (!strcmp(stemmer, "arabic")){
      ArabicStemmerParameter::get();
    }    
  }

  // free the memory allocated in get()
  void freeMem() {
    free(index);
    free(stopwords);
    free(acronyms);
    free(docFormat);
    free(stemmer);
    delete[](kstemmer_dir);
    free(dataFiles);
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
  if ((argc < 3) && (!strcmp(LocalParameter::dataFiles, ""))) {
    usage(argc, argv);
    return -1;
  }

  // Create the appropriate parser.
  Parser * parser;
  if (!strcmp(LocalParameter::docFormat, "web")) {
    parser = new WebParser();
  } else if (!strcmp (LocalParameter::docFormat, "reuters")) {
    parser = new ReutersParser();
  } else if (!strcmp (LocalParameter::docFormat, "trec")) {
    parser = new TrecParser();
  } else if (!strcmp(LocalParameter::docFormat, "chinese")) {
    parser = new ChineseParser();
  } else if (!strcmp(LocalParameter::docFormat, "chinesechar")) {
    parser = new ChineseCharParser();
  } else if (!strcmp(LocalParameter::docFormat, "arabic")) {
    parser = new ArabicParser();
  } else if (strcmp (LocalParameter::docFormat, "")) {
    throw Exception("PassageIndexer", "Unknown docFormat specified");
  } else {
    cerr << "Using default trec parser" << endl;
    parser = new TrecParser();
  }

  // Create the stopper if needed.
  Stopper * stopper = NULL;
  if (strcmp(LocalParameter::stopwords, "")) {
    if (!fileExist(LocalParameter::stopwords)) {
      throw Exception("PassageIndexer", 
		      "stopwords file specified does not exist");
    }
    stopper = new Stopper(LocalParameter::stopwords);
  }

  // Create the acronym list and tell parser if needed.
  WordSet * acros = NULL;
  if (strcmp(LocalParameter::acronyms, "")) {
    acros = new WordSet(LocalParameter::acronyms);
    parser->setAcroList(acros);
  }
  
  // Create the stemmer if needed.
  Stemmer * stemmer = NULL;
  if (!strcmp(LocalParameter::stemmer, "porter")) {
    stemmer = new PorterStemmer();
  } else if (!strcmp(LocalParameter::stemmer, "krovetz")) {
    stemmer = new KStemmer();
  } else if (!strcmp(LocalParameter::stemmer, "arabic")) {
    stemmer = new ArabicStemmer(ArabicStemmerParameter::stemDir, 
				ArabicStemmerParameter::stemFunc);
  } else if (strcmp(LocalParameter::stemmer, "")) {
    throw Exception("PassageIndexer", "Unknown stemmer specified");
  }

  // Create the indexer. (Note: this has an InvFPPushIndex that 
  // it uses to do the indexing, but InvFPTextHandler implements the
  // TextHandler class, so that it is compatible with my parser
  // architecture.  See the TextHandler and InvFPTextHandler classes
  // for more info.)
  
  if (!strcmp(LocalParameter::index, "")) {
    throw Exception("PassageIndexer", "index must be specified");
  }

  InvPassageTextHandler* indexer;
  indexer = new InvPassageTextHandler(LocalParameter::index,
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
  if (strcmp(LocalParameter::dataFiles, "")) {
    if (!fileExist(LocalParameter::dataFiles)) {
      throw Exception("PassageIndexer", "dataFiles specified does not exist");
    }

    ifstream source(LocalParameter::dataFiles);
    if (!source.is_open()) {
      throw Exception("PassageIndexer","could not open dataFiles specified");
    } else {
      string filename;
      while (getline(source, filename)) {
	cerr << "Parsing " << filename <<endl;
	parser->parse((char*)filename.c_str());
      }
    }
  } else {
    for (int i = 2; i < argc; i++) {
      cerr << "Parsing " << argv[i] << endl;
    if (!fileExist(argv[i])) {
      throw Exception("PassageIndexer", "datfile specified does not exist");
    }
      parser->parse(argv[i]);
    }
  }

  // free memory
  if (acros != NULL) delete acros;
  if (stopper != NULL) delete stopper;
  delete parser;
  delete indexer;
  LocalParameter::freeMem();
  return 0;
}


