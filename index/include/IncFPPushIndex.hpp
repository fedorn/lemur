/*==========================================================================
 * Copyright (c) 2002 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

#ifndef _INCFPPUSHINDEX_HPP
#define _INCFPPUSHINDEX_HPP

/*
 * NAME DATE - COMMENTS
 * dmf 07/02 - subclass from InvFPPushIndex
 *
 ======================================================================*/
#include "InvFPPushIndex.hpp"

namespace lemur 
{
  namespace index 
  {
    
    //! Incremental push method of building an index

    /*!

    This class builds an index assigning termids, docids, tracking locations of term within documents, and tracking terms within documents.  It also expects a DocumentProp to have the total number of terms that were in a document.  It expects that all stopping and stemming (if any) occurs before the term is passed in. If used with an existing index, new documents are added incrementally.

    */


    class IncFPPushIndex : public InvFPPushIndex {
    public:
      /// opens the index named by prefix, if it exists, and initializes
      /// the state of the indexer to begin adding new documents.
      IncFPPushIndex(const string &prefix, int cachesize=128000000, 
                     long maxfilesize=2100000000, 
                     lemur::api::DOCID_T startdocid=1);
    private:
      /// existing index filenames
      string invfpF, dtF, dmgrF, didF, tidF;
      /// try to open the index
      bool tryOpen(const string &name);
      /// read the table of contents (.ifp) file.
      bool readToc(const string &name);
      /// read the inverted file ids.
      void readInvFileIDs();
      /// read the dt file ids.
      void readDtFileIDs();
      /// read the document manager ids.
      void readDocMgrIDs();
      /// read the term ids.
      void readTermIDs();
      /// read the docids.
      void readDocIDs();
    };
  }
}

#endif
