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

#ifndef _DBMANAGER_HPP
#define _DBMANAGER_HPP

#include "MemParser.hpp"
namespace lemur 
{
  namespace distrib
  {
    
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

      virtual void open (const string &info) = 0;
      virtual void close() = 0;

      /// Query a database.
      virtual results_t * query (const char * query, int numdocs) const = 0;
      /// Get a parser that can parse this database's documents.
      virtual MemParser * getParser() const = 0;
      /// Get a document from the database.
      virtual doc_t * getDoc(const docid_t docid) const = 0;
      /// Append a document to file.
      virtual void output(const docid_t docid) const = 0;
      /// Set output file name
      virtual void setOutputFile(const string &filename) const = 0;

    };
  }
}

#endif
