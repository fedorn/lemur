#include "Stopper.hpp"
#include "PorterStemmer.hpp"
#include "WebParser.hpp"
#include "TrecParser.hpp"
#include "ReutersParser.hpp"
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
    name++;
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

