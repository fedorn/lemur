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



#ifndef _LEMURDBMANAGER_HPP
#define _LEMURDBMANAGER_HPP
#include "IndexManager.hpp"

#include "DBManager.hpp"
#include "RetrievalMethod.hpp"
#include "LemurMemParser.hpp"
#include "RetMethodManager.hpp"
namespace lemur 
{
  namespace distrib 
  {
    
    /*! Provides a simple wrapper to lemur databases for 
     * use with the query-based sampling code.
     */
    class LemurDBManager : public DBManager {

    public:
      /// Open a Lemur database - The dbname is a parameter file
      /// which specifies retrieval parameters as with RetEval.
      void open(const string &dbname);

      /// Query the database.
      results_t * query (const char * query, int numdocs) const;

      /// Get the parser for the database. (This should always
      /// return the same parser.)
      MemParser * getParser() const;

      /// Get a document given its document id.
      doc_t * getDoc(const docid_t docid) const;

      /// Write a document to file.
      void output(const docid_t docid) const;
      /// Set output file name
      void setOutputFile(const string &filename) const;

      /// Close the database.
      void close();

    private:
  
      lemur::api::Index * index;
      lemur::api::RetrievalMethod * model;
      lemur::api::ScoreAccumulator * accumulator;
      lemur::api::IndexedRealVector * results;
      LemurMemParser * parser;
      mutable ofstream * outfile;

    };
  }
}

#endif
