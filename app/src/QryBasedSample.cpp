/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
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
using namespace lemur::api;
using namespace lemur::distrib;


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
    std::string rmode = ParamGetString("queryMode", "unif");
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
  std::string dbManager;
  /// Filename for a list of databases to sample
  string listFile;
  /// Initial query selection model
  string initModel;

  /// Get the application specific parameters
  void get() {
    dbManager = ParamGetString("dbManager", "lemur");
    listFile = ParamGetString("listFile", "");    
    initModel = ParamGetString("initModel", "");
    // Enforce that both the listFile and initModel be specified
    if (listFile.empty() ||initModel.empty()) {
      usage();
      exit(-1);
    }
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
  lemur::distrib::FreqCounter freqCounter;
  freqCounter.setRandomMode(QBSParameter::qryMode);

  // Create the database manager
  lemur::distrib::DBManager * dbm;
  if (LocalParameter::dbManager == "lemur") {
    dbm = new lemur::distrib::LemurDBManager();
  } else {
    throw Exception("QryBasedSample", "Unknown dbManager specified.");
  }

  // Create and initialize the query-based sampler object
  lemur::distrib::QryBasedSampler dbp;

  dbp.setNumDocs(QBSParameter::numDocs);
  dbp.setNumWords(QBSParameter::numWords);
  dbp.setNumQueries(QBSParameter::numQueries);
  dbp.setTermMode(QBSParameter::terminateMode);
  dbp.setDocsPerQuery(QBSParameter::docsPerQuery);  
  
  dbp.setDBManager(dbm);
  dbp.setFreqCounter(&freqCounter);

  // Initial query terms are selected from this model.
  lemur::distrib::FreqCounter initQueryModel(LocalParameter::initModel);
  initQueryModel.setRandomMode(QBSParameter::qryMode);
  
  // Main processing loop - perform sampling on all databases.
  ifstream inl(LocalParameter::listFile.c_str());
  while (! inl.eof()) {
    string dbn;
    string opre;
    string dbid;
    // Read in database.
    inl >> dbn;
    if (! inl.eof()) {
      
      // Read in output prefix.
      inl >> opre;
      // Read in database label.
      inl >> dbid;

      // Output which database we're about to sample.
      cout << "Database: " << dbn << endl;
        
      // Tell the prober the new output prefix.
      dbp.setOutputPrefix(opre);      

      // Open the new database.
      dbm->open(dbn);
      // Get a parser for the database.
      lemur::distrib::MemParser * dp = dbm->getParser();
      // Tell the parser to pass info on to the model builder.
      dp->setTextHandler(&freqCounter);
      
      // Select a query term and sample the datbase.
      char * iq = initQueryModel.randomWord();
      while (dbp.probe(iq) == false) {
        // randomWord may return NULL
        // dmf 05/2005
        if (iq != NULL) free (iq);
        iq = initQueryModel.randomWord();
      }
      // randomWord may return NULL   
      // dmf 05/2005
      if (iq != NULL) free (iq);
      
      // Clear old counts.
      freqCounter.clear();
      // Close the database.
      dbm->close();
      
    }
  }
  delete(dbm);
  return 0;
}


