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

#ifndef _DBMANAGER_HPP
#define _DBMANAGER_HPP

#include "MemParser.hpp"

/*!
 * Results is a structure containing a count of document IDs and a 
 * number of items in the list.
 */
typedef struct results {
  int numDocs;
  docid_t * ids;
} results_t;



/*!
 * An abstract wrapper for database access.  For use with the 
 * QryBasedSampler.  Provides an interface for basic access -
 * querying, accessing documents, and providing a document parser.
 */
class DBManager {

public:

  virtual void open (char * info) = 0;
  virtual void close() = 0;

  /// Query a database.
  virtual results_t * query (char * query, int numdocs) = 0;
  /// Get a parser that can parse this database's documents.
  virtual MemParser * getParser() = 0;
  /// Get a document from the database.
  virtual doc_t * getDoc(docid_t docid) = 0;
  /// Append a document to file.
  virtual void output(docid_t docid) = 0;
  /// Set output file name
  virtual void setOutputFile(char * filename) = 0;

};

#endif
