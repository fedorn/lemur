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


#ifndef _INDEX_HPP
#define _INDEX_HPP
// C. Zhai 02/08/2001

#include "TermInfoList.hpp"
#include "DocInfoList.hpp"
#include "DocumentManager.hpp"
#include "CollectionProps.hpp"
#include "lemur-platform.h"
#include "lemur-compat.hpp"

namespace lemur 
{
  namespace api 
  {
    
    ///  Abstract Class for indexed document collection.

    /*! 

    This is an abstract class that provides a uniform interface
    for access to an indexed document collection. The following
    is an example of using it.


    <PRE>


    Index &amp;myIndex;

    myIndex.open("index-file");


    int t1;
    ... 

    // now fetch doc info list for term t1
    // this returns a dynamic instance, so you'll need to delete it
    DocInfoList *docList = myIndex.docInfoList(t1);

    docList->startIteration();

    DocInfo *entry;
    while (docList->hasMore()) {
    entry = docList->nextEntry(); 
    // this returns a pointer to a *static* memory, do don't delete entry!
  
    cout << "entry doc id: "<< entry->docID() <<endl;
    cout << "entry term count: "<< entry->termCount() << endl;
    }

    delete docList;
    </PRE>
    */

    class Index {
    public:

      virtual ~Index() {}; 

      /// @name Open index 
      //@{

      /// Open previously created Index, return true if opened successfully, <tt>indexName</tt> should be the full name of the table-of-content file for the index. E.g., "index.bsc" for an index built with the basic indexer. 
      virtual bool open(const string &indexName)=0;
      //@}

      /// @name Spelling and index conversion
      //@{

      /// Convert a term spelling to a termID, returns 0 if out of vocabulary. Valid index starts at 1.
      virtual TERMID_T term (const TERM_T &word) const=0;

      /// Convert a valid termID to its spelling
      virtual const TERM_T term (TERMID_T termID) const=0;

      /// Convert a spelling to docID, returns 0 if out of vocabulary. Valid index starts at 1.
      virtual DOCID_T document (const EXDOCID_T &docIDStr) const=0;

      /// Convert a valid docID to its spelling
      virtual const EXDOCID_T document (DOCID_T docID) const=0;
      /// A String identifier for the document manager to get at the source
      /// of the document with this document id
      //  virtual const char* docManager(int docID) { return NULL;}
      virtual const DocumentManager* docManager(DOCID_T docID) const {return NULL;}


      /// Return a string ID for the term lexicon (usually the file name of the lexicon)
      /*! This function should be pure virtual; the default implementation
        is just for convenience. Appropriate implementation to be done in the future. 
      */
      virtual const string termLexiconID() const { return "";} 

      //@}

      /// @name Summary counts
      //@{

      /// Total count (i.e., number) of documents in collection
      virtual COUNT_T docCount () const=0;

      /// Total count of unique terms in collection, i.e., the term vocabulary size
      virtual COUNT_T termCountUnique () const=0;

      /// Total counts of a term in collection
      virtual COUNT_T termCount (TERMID_T termID) const=0;

      /// Total counts of all terms in collection
      virtual COUNT_T termCount () const=0;

      /// Average document length 
      virtual float docLengthAvg() const=0;

      /// Total counts of doc with a given term
      virtual COUNT_T docCount(TERMID_T termID) const=0;

      /// Total counts of terms in a document  
      virtual COUNT_T docLength (DOCID_T docID) const=0;

      //@}

      /// @name Index entry access
      //@{
      /// returns a new instance of DocInfoList which represents the doc entries in a term index, you must delete the instance later. @see DocInfoList
      virtual DocInfoList *docInfoList(TERMID_T termID) const=0;

      /// returns a new instance of TermInfoList which represents the word entries in a document index, you must delete the instance later. @see TermInfoList
      virtual TermInfoList *termInfoList(DOCID_T docID) const=0;

      //@}

      // returns TermInfoList is sequential representation (not bag of words)
      // return NULL list when sequence is not available.
      virtual TermInfoList *termInfoListSeq(DOCID_T docID) const { return NULL; }

      /// return whatever collection properties might have been passed in 
      /// indexing with call to PushIndex::endCollection(CollectionProps)
      virtual const lemur::parse::CollectionProps* collectionProps() const { return NULL; }
    };
  }
}


#endif



