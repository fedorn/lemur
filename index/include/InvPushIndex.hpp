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


#ifndef _INVPUSHINDEX_HPP
#define _INVPUSHINDEX_HPP

//! Example Class for push method of building an index

/*!

  This class builds an index of inverted lists assigning termids and docids.  It also expects a DocumentProp to have the total number of terms that were in a document.  It expects that all stopping and stemming (if any) occurs before the term is passed in.

*/

/*
 * NAME DATE - COMMENTS
 * tnt 01/02 - created
 ======================================================================*/
#include "common_headers.hpp"
#include "PushIndex.hpp"
#include "MemCache.hpp"
#include "InvFPTypes.hpp"
#include "InvDocList.hpp"
#include "InvFPTerm.hpp"
#include "InvIndexMerge.hpp"


typedef map<string, InvDocList*, less<string> > TABLE_T;

class InvPushIndex : public PushIndex {
public:
  InvPushIndex(){ };
  InvPushIndex(const string &prefix, int cachesize=128000000, long maxfilesize=2100000000, DOCID_T startdocid=1);
  ~InvPushIndex();

  /// sets the name for this index. the name will be the prefix for all files related to this index
  void setName(const string &prefix);

  /// the beginning of a new document, returns true if initiation was successful
  bool beginDoc(const DocumentProps* dp);

  /// adding a term to the current document, returns true if term was added successfully.  
  bool addTerm(const Term& t);

  /// signify the end of current document
  void endDoc(const DocumentProps* dp);

  /// signify the end of current document and associate with certain document manager. this doesn't change the mgr that was previously set.
  virtual void endDoc(const DocumentProps* dp, const string &mgr);

  /// signify the end of this collection.  properties passed at the beginning of a collection should be handled by the constructor.
  void endCollection(const CollectionProps* cp);

  /// set the document manager to use for succeeding documents
  void setDocManager(const string &mgrID);

protected:
  void writeTOC(int numinv);
  void writeDocIDs();
  void writeCache();
  void lastWriteCache();
  void writeDTIDs();
  void writeDocMgrIDs();
  /// returns the internal id of given docmgr
  /// if not already registered, mgr will be added
  int docMgrID(const string &mgr);
  virtual void doendDoc(const DocumentProps* dp, int mgrid);

  long maxfile; /// the biggest our file size can be
  MemCache* cache; /// the main memory handler for building
  vector<EXDOCID_T> docIDs; /// list of external docids in internal docid order
  vector<TERM_T> termIDs; /// list of terms in termid order
  vector<string> tempfiles; /// list of tempfiles we've written to flush cache
  vector<string> dtfiles; /// list of dt index files
  vector<string> docmgrs;  // the list of doc managers we have (index = id)

  FILE* writetlookup; /// filestream for writing the lookup table to the docterm db
  ofstream writetlist; /// filestream for writing the list of located terms for each document

  COUNT_T tcount;    /// count of total terms
  COUNT_T tidcount ; /// count of unique terms
  COUNT_T dtidcount; /// count of unique terms in a current doc
  string name;    /// the prefix name
  TABLE_T wordtable; /// table of all terms and their doclists
  map<TERMID_T, COUNT_T> termlist; /// maps of terms and freqs
  int* membuf; /// memory to use for cache and buffers
  int membufsize;  // how much memory we have
  int curdocmgr; // the current docmanager to use
};

#endif
