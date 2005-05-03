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

#ifndef _QRYBASEDSAMPLER_HPP
#define _QRYBASEDSAMPLER_HPP



#include "FreqCounter.hpp"
#include "DBManager.hpp"

namespace lemur 
{
  namespace distrib 
  {
    
    /// Stores IDs of previously seen documents.
    typedef set<docid_t, less<string> > docidset;

    /*! Terminate on number of unique documents seen. */
#define T_NDOCS 1
    /*! Terminate on number of unique words seen. */
#define T_NWORDS 2
    /*! Terminate on number of queries run. */
#define T_NQRYS 4

    /*!
     * Manages the sampling of documents from a database.
     */
    class QryBasedSampler {
    public:
      QryBasedSampler();
      ~QryBasedSampler();

      /*! Probe the database, using initQuery as the initial query. */
      bool probe(const char * initQuery);
  
      /// Set the dstabase manager.
      void setDBManager(const DBManager * database);

      /*! Get a reference to the database manager. */
      const DBManager * getDBManager() const;


      /*! Set the frequency counter - this handles building the
       * database description from documents. */
      void setFreqCounter(FreqCounter * counter);

      /*! Get a reference to the frequency counter. */
      const FreqCounter * getFreqCounter() const;


      /*! Set the ouput prefix - this prepended to the file names
       * for documents and models.  Documents are stored in 
       * prefix.docs and the model for a db is stored in prefix.model. */
      void setOutputPrefix(const string &prefix);
  
      /*! Get the output prefix. */
      const string &getOutputPrefix() const;

      /*! Set the number of unique documents to retrieve. */
      void setNumDocs(int n);

      /*! Get the number of unique documents to retrieve. */
      int getNumDocs() const;


      /*! Set the number of unique words to retrieve. */
      void setNumWords(int n);

      /*! Get the number of unique words to retrieve. */
      int getNumWords() const;


      /*! Set the number of queries to run. */
      void setNumQueries(int n);

      /*! Get the number of queries to run. */
      int getNumQueries() const;


      /*! Set the termination mode.  To specify a disjunction of conditions,
       * OR the modes together.
       *
       * T_NDOCS - terminate on number of unique docs retrieved
       * T_NWORDS - terminate on number of unique words retrieved 
       * T_NQRYS = terminate on number of quries run */
      void setTermMode(int m);

      /*! Get the termination mode. */
      int getTermMode() const;
  

      /*! Set the number of documents per query to use. */
      void setDocsPerQuery(int n);
  
      /*! Get the number of documents per query used. */
      int getDocsPerQuery() const;


    private:

      /* for querying a db */
      const DBManager * db;


      /* for building a description of a db */
      FreqCounter * freqCounter;


      /* output prefix for filenames */
      string outputPrefix;


      /* termination mode of the probe -
       * either T_NDOCS or T_NWORDS */
      int termMode;
  
      /* number unique docs to retrieve - only used if
       * termMode == T_NDOCS */
      int numDocs;

      /* number unique words to retrieve - only used if
       * termMode == T_NWORDS */
      int numWords;

      /* number of queries to run - only used if
       * termMode == T_NQRYS */
      int numQueries;

      /* documents per query to use */
      int docsPerQuery;

      /* stores the ids of the document already retrieved
       * from the system.  used to prevent parsing
       * a document multiple times */
      docidset seenDocs;
    };
  }
}

#endif
