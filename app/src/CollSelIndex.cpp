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
#include "DocFreqIndexer.hpp"
#include "CtfIndexer.hpp"
#include "Param.hpp"

namespace LocalParameter {
  char * df;
  char * ctf;
  int memory;
  char * stopwords;
  char * acronyms;
  char * docFormat;
  char * dfDocs;
  char * dfCounts;
  char * stemmer;
  char* d;	
  // path name to data files used by kstemmer
  char *kstemmer_dir;

  bool countStopWds;

  void get() {
    df = strdup(ParamGetString("dfIndex", "collSel_df"));
    ctf = strdup(ParamGetString("ctfIndex", "collSel_ctf"));
    memory = ParamGetInt("memory", 64000000);
    stopwords = strdup(ParamGetString("stopwords"));
    acronyms = strdup(ParamGetString("acronyms"));
    docFormat = strdup(ParamGetString("docFormat"));
    for (d = docFormat; *d != '\0'; d++) *d = tolower(*d);
    dfCounts = strdup(ParamGetString("dfCounts", "collSel_df.cw"));
    dfDocs = strdup(ParamGetString("dfDocs", "collSel_df.cd"));

    stemmer = strdup(ParamGetString("stemmer"));
    // convert stemmer to lowercase
    for (d = stemmer; *d != '\0'; d++) *d = tolower(*d);
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
    
    
    countStopWds = (ParamGetString("countStopWords", "false") == "true" 
		    ? true : false);

  }
  void freeMem() {
    free(df);
    free(ctf);
    free(stopwords);
    free(acronyms);
    free(docFormat);
    free(dfCounts);
    free(dfDocs);
    free(stemmer);
    delete[](kstemmer_dir);
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
    throw Exception("PushIndexer", "Unknown docFormat specified");
  } else {
    cerr << "Using default trec parser" << endl;
    parser = new TrecParser();
  }

  Stopper * stopper = NULL;
   if (strcmp(LocalParameter::stopwords, "")) {
    stopper = new Stopper(LocalParameter::stopwords);
  }

  WordSet * acros = NULL;
  if (strcmp(LocalParameter::acronyms, "")) {
    acros = new WordSet(LocalParameter::acronyms);
    parser->setAcroList(acros);
  }

  Stemmer * stemmer = NULL;
  if (!strcmp(LocalParameter::stemmer, "porter")) {
    stemmer = new PorterStemmer();
  } else if (!strcmp(LocalParameter::stemmer, "krovetz")) {
    stemmer = new KStemmer();
  } else if (!strcmp(LocalParameter::stemmer, "arabic")) {
    stemmer = new ArabicStemmer(ArabicStemmerParameter::stemDir, 
				ArabicStemmerParameter::stemFunc);
  }

  DocFreqIndexer dfIndexer(LocalParameter::df,
			   LocalParameter::dfCounts,
			   LocalParameter::dfDocs,
			   LocalParameter::memory,
			   LocalParameter::countStopWds);

  CtfIndexer ctfIndexer(LocalParameter::ctf,
			LocalParameter::memory,
			LocalParameter::countStopWds);

  // chain the parser/stopper/stemmer/indexer

  TextHandler * th = parser;

  parser->setTextHandler(stopper);

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
    parser->parse(argv[i]);

    free (n);

  }

  if (acros != NULL) delete acros;
  if (stopper != NULL) delete stopper;
  delete parser;
  LocalParameter::freeMem();
  return 0;
}

