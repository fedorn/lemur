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

#include <fstream>
#include <cstring>
#include <cstdlib>
#include <cstdio>


#include "QryBasedSampler.hpp"
#include "LemurDBManager.hpp"
#include "FreqCounter.hpp"
#include "Exception.hpp"

//#include "DocFreqIndexer.hpp"
//#include "CtfIndexer.hpp"


#include "Param.hpp"


/// Prints out useful usage information
void usage() {
  cout << "QryBasedSample samples databases and builds vocabulary models." 
       << endl << endl
       << "Command line options:" << endl
       << "param_file - first argument, must be here for Lemur " << endl 
       << endl
       << "Parameter file format:" << endl << endl << endl
       << "dbManager = manager;\tlemur or mind" << endl << endl
       << "numDocs = num;" 
       << "\tterminate probe when num unique docs from" << endl 
       << "\t\tthe database have been seen" << endl << endl
       << "numWords = num;" 
       << "\tterminate probe when num unique words from " << endl
       << "\t\tthe database have been seen" << endl << endl
       << "numQueries = num;" 
       << "\tterminate probe when num queries have" << endl 
       << "\t\t\tbeen run " << endl << endl
       << "docsPerQuery = num;\tuse num documents per query" << endl << endl
       << "queryMode = mode;\tspecify the mode for query selection:" << endl
       << endl
       << "\tunif  - random uniform from documents seen so far" << endl
       << "\tavetf - word chosen with probability proportional" << endl
       << "\t        to its average term frequency" << endl
       << "\tctf   - word chosen w/ prob proportional to its " << endl
       << "\t        collection term frequency" << endl
       << "\tdf    - word chosen w/ prob proportional to its " << endl
       << "\t        document frequency" << endl << endl
       << "listFile = filename;\tfile containing list of databases to" << endl
       << "\t\t\tprobe and their output prefixes.  " << endl
       << "\t\t\tFormat is db<tab>prefix<tab>dbname, " << endl
       << "\t\t\twith one tuple per line." << endl 
       << "\t\t\tFor lemur dbFormat, the db field is a parameter file" << endl
       << "\t\t\tthat contains the database and retrieval method" << endl 
       << "\t\t\tparameters as with RetEval.  For mind dbFormat" << endl
       << "\t\t\tthe field contains a semicolon seperated list of" << endl
       << "\t\t\tvalues needed for building a resource description" << endl
       << "\t\t\tin the MIND system: " << endl << endl
       << "  urn_proxy;url_proxy;urn_proxy_interface;urn_proxy_cons_text;num_docs_in_db" << endl 
       << endl
       << "initModel = modelfile;\ta language model to use for initial"<< endl
       << "\t\t\tquery selection. Words are selected" << endl
       << "\t\t\tusing random uniform from the model." << endl;
//         << "docFreqIndex = dbname;\tbuilds a (lemur) collection " << endl
//         << "\t\t\tselection database storing document " << endl
//         << "\t\t\tfrequencies. (Use with CORI)" << endl
//         << "ctfIndex = dbname;\tbuilds a (lemur) collection " << endl
//         << "\t\t\tselection database storing collection " << endl 
//         << "\t\t\tterm frequencies. (Use with KL)" << endl;

}

/// Parameters for query-based samping
namespace QBSParameter {
  /// Query selection mode
  int qryMode;
  /// Number of unique documents to get
  int numDocs;
  /// Number of unique words to get
  int numWords;
  /// Number of unique queries to run
  int numQueries;
  /// Termination mode
  int terminateMode;
  /// Number of documents per query to use
  int docsPerQuery;
  
  /// Gets and initializes query-based sampling parameters
  void get() {


    // Look up random query selection mode
    String rmode = ParamGetString("queryMode", "unif");
    if (rmode == "unif") qryMode = R_UNIFORM;
    else if (rmode == "avetf") qryMode = R_AVE_TF;
    else if (rmode == "df") qryMode = R_DF;
    else if (rmode == "ctf") qryMode = R_CTF;

    // Look up termination mode parameters
    numDocs = ParamGetInt("numDocs", 0);
    if (numDocs > 0) 
      terminateMode |= T_NDOCS;

    numWords = ParamGetInt("numWords", 0);
    if (numWords > 0)
      terminateMode |= T_NWORDS;

    numQueries = ParamGetInt("numQueries", 0);
    if (numQueries > 0) 
      terminateMode |= T_NQRYS;

    // Look up number of documents per query
    docsPerQuery = ParamGetInt("docsPerQuery", 4);

    // Enforce the existence of at least one termination mode parameter
    if (terminateMode == 0) {
      usage();
      exit(-1);
    }

  }
  
};

/// Application specific parameters
namespace LocalParameter {
  /// database manager
  String dbManager;
  /// Filename for a list of databases to sample
  char * listFile;
  /// Initial query selection model
  char * initModel;

  // Name of Collection Term Frequency index to build
  // char * ctfIndex;
  // Name of Document Frequency index to build
  // char * dfIndex;


  /// Get the application specific parameters
  void get() {
    dbManager = ParamGetString("dbManager", "lemur");
    listFile = strdup(ParamGetString("listFile", ""));    
    initModel = strdup(ParamGetString("initModel", ""));
//      dfIndex = strdup(ParamGetString("docFreqIndex", ""));    
//      ctfIndex = strdup(ParamGetString("ctfIndex", ""));

    // Enforce that both the listFile and initModel be specified
    if (strcmp(listFile, "") == 0 || strcmp(initModel, "") == 0) {
      usage();
      exit(-1);
    }
  }

  /// Free memory created by the get function
  void freeMem() {
    free(listFile);
    free(initModel);
//      free(ctfIndex);
//      free(dfIndex);
  }
};

/// Get the application's parameters
void GetAppParam() {
  LocalParameter::get();
  QBSParameter::get();
}


/// Main function for query-based sampling application
int AppMain (int argc, char * argv []) {

  // Keeps track of db stats during sampling
  FreqCounter freqCounter;
  freqCounter.setRandomMode(QBSParameter::qryMode);

  // Create the database manager
  DBManager * dbm;
  if (LocalParameter::dbManager == "lemur") {
    dbm = new LemurDBManager();
  } else {
    throw Exception("QryBasedSample", "Unknown dbManager specified.");
  }

  // Create and initialize the query-based sampler object
  QryBasedSampler dbp;

  dbp.setNumDocs(QBSParameter::numDocs);
  dbp.setNumWords(QBSParameter::numWords);
  dbp.setNumQueries(QBSParameter::numQueries);
  dbp.setTermMode(QBSParameter::terminateMode);
  dbp.setDocsPerQuery(QBSParameter::docsPerQuery);  
  
  dbp.setDBManager(dbm);
  dbp.setFreqCounter(&freqCounter);

  /*
  // Create and chain the document frequency indexer.
  DocFreqIndexer * dfIndexer = NULL;
  if (strcmp(LocalParameter::dfIndex, "") != 0) {
    char * csdbCw = new char[strlen(LocalParameter::dfIndex) + 4];
    strcpy(csdbCw, LocalParameter::dfIndex);
    strcat(csdbCw, ".cw");
    char * csdbSs = new char[strlen(LocalParameter::dfIndex) + 4];
    strcpy(csdbSs, LocalParameter::dfIndex);
    strcat(csdbSs, ".ss");
    dfIndexer = new DocFreqIndexer(LocalParameter::dfIndex, 
				   csdbCw, csdbSs, 24*1024*1024);      
    freqCounter.setTextHandler(dfIndexer);
    delete [] csdbCw;
    delete [] csdbSs;
  }
 
  // Create and chain the collection term frequency indexer.
  CtfIndexer * ctfIndexer = NULL;
  if (strcmp(LocalParameter::ctfIndex, "")  != 0) {
    char * csdbCw = new char[strlen(LocalParameter::ctfIndex) + 4];
    strcpy(csdbCw, LocalParameter::ctfIndex);
    strcat(csdbCw, ".cw");
    char * csdbSs = new char[strlen(LocalParameter::ctfIndex) + 4];
    strcpy(csdbSs, LocalParameter::ctfIndex);
    strcat(csdbSs, ".ss");
    ctfIndexer = new CtfIndexer(LocalParameter::ctfIndex, 
				csdbCw, csdbSs, 24*1024*1024);
    if (dfIndexer != NULL) {
      dfIndexer->setTextHandler(ctfIndexer);
    } else {
      freqCounter.setTextHandler(ctfIndexer);
    }
    delete [] csdbCw;
    delete [] csdbSs;
  }
  */

  // Initial query terms are selected from this model.
  FreqCounter initQueryModel(LocalParameter::initModel);
  initQueryModel.setRandomMode(QBSParameter::qryMode);
  
  // Main processing loop - perform sampling on all databases.
  ifstream inl(LocalParameter::listFile);
  while (! inl.eof()) {
    char dbn[5000];
    char opre[5000];
    char dbid[100];
    // Read in database.
    inl >> dbn;
    if (! inl.eof()) {
      
      // Read in output prefix.
      inl >> opre;
      // Read in database label.
      inl >> dbid;

      /*
      // If building a collection selection database,
      // tell them about the new database.
      if (dfIndexer != NULL) {
	dfIndexer->newDb(dbid);
      }
      if (ctfIndexer != NULL) {
	ctfIndexer->newDb(dbid);
      }
      */
      
      // Output which database we're about to sample.
      cout << "Database: " << dbn << endl;
	
      // Tell the prober the new output prefix.
      dbp.setOutputPrefix(opre);      

      // Open the new database.
      dbm->open((char *) dbn);
      // Get a parser for the database.
      MemParser * dp = dbm->getParser();
      // Tell the parser to pass info on to the model builder.
      dp->setTextHandler(&freqCounter);
      
      // Select a query term and sample the datbase.
      char * iq = initQueryModel.randomWord();
      while (dbp.probe(iq) == false) {
	free (iq);
	iq = initQueryModel.randomWord();
      }
   
      free (iq);
      
      // Clear old counts.
      freqCounter.clear();
      // Close the database.
      dbm->close();
      
    }
    
  }
  /*
  // Delete indexers.
  if (dfIndexer != NULL) {
    delete dfIndexer;
  }
  if (ctfIndexer != NULL) {
    delete ctfIndexer;
  }
  */

  // Free parameter memory.
  LocalParameter::freeMem();

  return 0;
}


