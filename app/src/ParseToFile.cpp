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

#include "Stopper.hpp"
#include "PorterStemmer.hpp"
#include "KStemmer.hpp"
#include "ArabicStemmer.hpp"
#include "WebParser.hpp"
#include "TrecParser.hpp"
#include "ReutersParser.hpp"
#include "ChineseParser.hpp"
#include "ChineseCharParser.hpp"
#include "ArabicParser.hpp"
#include "WriterTextHandler.hpp"
#include "Param.hpp"
#include "FUtil.hpp"
#include "Exception.hpp"

// Local parameters used by the indexer 
namespace LocalParameter {

  // name of file containing stopwords
  char * stopwords;
  // name of file containing acronyms
  char * acronyms;
  // format of documents (trec or web)
  char * docFormat;
  // which stemmer to use
  char * stemmer;
  // output file
  char * outFile;
  // path name to data files used by kstemmer
  char *kstemmer_dir;

  void get() {
    // my code uses char *'s while the param utils use
    // strings.  maybe I should convert to strings...

    outFile = strdup(ParamGetString("outputFile"));
    stopwords = strdup(ParamGetString("stopwords"));
    acronyms = strdup(ParamGetString("acronyms"));
    docFormat = strdup(ParamGetString("docFormat"));
    // convert docFormat to lowercase
    for (char * d = docFormat; *d != '\0'; d++) *d = tolower(*d);

    stemmer = strdup(ParamGetString("stemmer"));
    // convert docFormat to lowercase
    for (char * e = stemmer; *e != '\0'; e++) *e = tolower(*e);
    if(!strcmp(stemmer, "krovetz")) {
      // Using kstemmer needs a path to data files
      if(strlen(ParamGetString("KstemmerDir"))>0) {
	// if KstemmerDir is declared then resets STEM_DIR otherwise uses the default
	kstemmer_dir = new char[MAX_FILENAME_LENGTH];
	kstemmer_dir[0]='\0';
	strcat(kstemmer_dir, "STEM_DIR=");
	strcat(kstemmer_dir,ParamGetString("KstemmerDir"));
	if(putenv(kstemmer_dir))
	  cerr << "putenv can not set STEM_DIR" << endl;
      }
    } else if (!strcmp(stemmer, "arabic")){
      ArabicStemmerParameter::get();
    }
  }

  // free the memory allocated in get()
  void freeMem() {
    free(stopwords);
    free(acronyms);
    free(docFormat);
    free(stemmer);
    delete[](kstemmer_dir);
    free(outFile);
  }
};

// put more usage info in here
void usage(int argc, char ** argv) {
  cerr << "Usage:" << endl
       << argv[0] << " paramfile datfile1 datfile2 ... " << endl
       << endl
       << "ParseToFile parses documents and writes output compatible with " << endl
       << "BuildBasicIndex. The program uses either the TrecParser class, " 
       << endl
       << "WebParser or Chinese(Char)Parser class to parse." << endl
       << "Summary of parameters:" << endl << endl
       << "\toutputFile - name of file to output parsed documents to" << endl
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
    throw Exception("ParseToFile", "Unknown docFormat specified");
  } else {
    cerr << "Using default trec parser" << endl;
    parser = new TrecParser();
  }

  // Create the stopper if needed.
  Stopper * stopper = NULL;
  if (strcmp(LocalParameter::stopwords, "")) {
    if (!fileExist(LocalParameter::stopwords)) {
      throw Exception("ParseToFile", "stopwords file specified does not exist");
    }
    stopper = new Stopper(LocalParameter::stopwords);
  }

  // Create the acronym list and tell parser if needed.
  WordSet * acros = NULL;
  if (strcmp(LocalParameter::acronyms, "")) {
    if (!fileExist(LocalParameter::acronyms)) {
      throw Exception("ParseToFile", "acronyms file specified does not exist");
    }
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
    throw Exception("ParseToFile", "Unknown stemmer specified");
  }

  // Create the document writer.
  if (!strcmp(LocalParameter::outFile, "")) {
    throw Exception("ParseToFile", "outputFile must be specified");
  }
  WriterTextHandler writer(LocalParameter::outFile);


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

  th->setTextHandler(&writer);

  // parse the data files
  for (int i = 2; i < argc; i++) {
    cerr << "Parsing " << argv[i] << endl;
    if (!fileExist(argv[i])) {
      throw Exception("ParseToFile", "datfile specified does not exist");
    }
    parser->parse(argv[i]);
  }

  // free memory
  if (acros != NULL) delete acros;
  if (stopper != NULL) delete stopper;
  delete parser;
  LocalParameter::freeMem();
  return 0;
}

