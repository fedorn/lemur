#include "TextHandlerManager.hpp"
#include "DocFreqIndexer.hpp"
#include "CtfIndexer.hpp"
#include "Param.hpp"
#include "TrecParser.hpp"

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

  bool countStopWds;

  void get() {
    df = strdup(ParamGetString("dfIndex", "collSel_df"));
    ctf = strdup(ParamGetString("ctfIndex", "collSel_ctf"));
    memory = ParamGetInt("memory", 64000000);
    stopwords = strdup(ParamGetString("stopwords"));
    acronyms = strdup(ParamGetString("acronyms"));
    docFormat = strdup(ParamGetString("docFormat"));
    dfCounts = strdup(ParamGetString("dfCounts", "collSel_df.cw"));
    dfDocs = strdup(ParamGetString("dfDocs", "collSel_df.cd"));

    stemmer = strdup(ParamGetString("stemmer"));
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

  //  parser->setTextHandler(stopper);

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

  if (stopper != NULL) delete stopper;
  delete parser;
  LocalParameter::freeMem();
  return 0;
}

