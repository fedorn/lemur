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



#ifndef _MINDDBMANAGER_HPP
#define _MINDDBMANAGER_HPP

#include "DBManager.hpp"
#include "Param.hpp"

#include "soapH.h"


/// Parameters needed for querying Mind databases
namespace MindParameter {
  static String registry;

  /// Get the parameters
  static void get() {
    registry = ParamGetString("MindRegistry");
  }
};


/*! Provides a simple wrapper to mind databases for 
 * use with the query-based sampling code.
 */
class MindDBManager : public DBManager, public MemParser {

public:
  MindDBManager();

  ~MindDBManager();

  
  /// Open a Mind database - The dbname is
  /// a semicolon seperated string with the
  /// proxy, proxy url, proxy-interface, proxy-construction-text, 
  /// and the number of docs in the db
  /// components.  Example:
  /// urn:proxy.Google;http://....;urn:proxy-interface.Google;urn:proxy-construction-text.Google;2000000000
  void open(char * dbname);

  /// Query the database.
  results_t * query (char * query, int numdocs);

  /// Parse a document. (Gets the proxy-construction-text to do the work.)
  void parse(doc_t * doc);

  /// Get a parser for the database. (Really this same object.)
  MemParser * getParser();

  /// Get a document given its document id.
  doc_t * getDoc(docid_t docid);

  /// Send a document to the proxy.
  void output(docid_t docid);
  /// Set the proxy documents will be sent to.
  /// Example: urn:proxy.Google
  void setOutputFile(char * filename);

  /// Close the database.
  void close();


private:
  
  void changeNamespace(char * id, char * ns, char * re);

  struct soap * soap;

  char * registry;
  char * interface;
  char * interfaceName;
  char * construction;
  char * constructionName;
  char * proxy;
  char * proxyName;

  char * sampleAttribute;
  char * schemaName;

  // schema of the database
  mindtype__Schema * schema;
  // result of the last query
  mindtype__PropResult * mindResult;

};

#endif
