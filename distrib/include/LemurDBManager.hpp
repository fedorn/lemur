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



#ifndef _LEMURDBMANAGER_HPP
#define _LEMURDBMANAGER_HPP

#include "DBManager.hpp"

#include "InvFPIndex.hpp"

#include "RetrievalMethod.hpp"
#include "LemurMemParser.hpp"
#include "RetMethodManager.hpp"

/// Parameters needed for querying Lemur databases
namespace LemurParameter {
  static RetMethodManager::RetModel mod;
  /// Database name
  static String dbname;

  /// Get the parameters
  static void get() {
    // default is KL divergence model
    mod = (RetMethodManager::RetModel) ParamGetInt("retModel", RetMethodManager::KL); 
    dbname = ParamGetString("index");
    RetrievalParameter::get();  
  }
};

/*! Provides a simple wrapper to lemur databases for 
 * use with the query-based sampling code.
 */
class LemurDBManager : public DBManager {

public:
  /// Open a Lemur database - The dbname is a parameter file
  /// which specifies retrieval parameters as with RetEval.
  void open(char * dbname);

  /// Query the database.
  results_t * query (char * query, int numdocs);

  /// Get a parser for the database.
  MemParser * getParser();

  /// Get a document given its document id.
  doc_t * getDoc(docid_t docid);

  /// Write a document to file.
  void output(docid_t docid);
  /// Set output file name
  void setOutputFile(char * filename);

  /// Close the database.
  void close();

private:
  
  Index * index;
  RetrievalMethod * model;
  ScoreAccumulator * accumulator;
  IndexedRealVector * results;
  LemurMemParser * parser;
  ofstream * outfile;

};

#endif
