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
/*
  10/22/2002 -- dmf Add arrays dtfstreams and invfstreams to hold open
  ifstreams for the inverted list files so that each is opened and closed
  only once.
 */

#ifndef _INVINDEX_HPP
#define _INVINDEX_HPP

///  Class for retrieving indexed document collection based on the abstract class Index.hpp
#include "common_headers.hpp"
#include "Index.hpp"
#include "InvDocList.hpp"
#include "InvTermList.hpp"
#include "InvFPTypes.hpp"
#include "Param.hpp"
#include "DocMgrManager.hpp"

// for counts array
#define UNIQUE_TERMS 0
#define TOTAL_TERMS  1
#define DOCS         2
#define DT_FILES     3
#define INV_FILES    4

#define NAMES_SIZE   8
// for names array
#define DOC_INDEX    0
#define DOC_LOOKUP   1
#define TERM_INDEX   2
#define TERM_LOOKUP  3
#define TERM_IDS     4
#define DOC_IDS      5
#define DOCMGR_IDS   6
#define VERSION_NUM  7

class InvIndex : public Index {
public:
   InvIndex();
   InvIndex(const string &indexName);
  ~InvIndex(); 

  /// @name Open index 
  //@{

  /// Open previously created Index with given prefix, return true if opened successfully
  bool open(const string &indexName);
  //@}

  /// @name Spelling and index conversion
  //@{

  /// Convert a term spelling to a termID
  TERMID_T term(const TERM_T &word) const;

  /// Convert a termID to its spelling
  const TERM_T term(TERMID_T termID) const;

  /// Convert a spelling to docID
  DOCID_T document(const EXDOCID_T &docIDStr) const;

  /// Convert a docID to its spelling
  const EXDOCID_T document(DOCID_T docID) const; 

  const DocumentManager* docManager(DOCID_T docID) const;

  //@}

  /// @name Summary counts
  //@{

  /// Total count (i.e., number) of documents in collection
  COUNT_T docCount() const { return counts[DOCS]; };

  /// Total count of unique terms in collection
  COUNT_T termCountUnique() const { return counts[UNIQUE_TERMS]; };

  /// Total counts of a term in collection
  COUNT_T termCount(TERMID_T termID) const;

  /// Total counts of all terms in collection
  COUNT_T termCount() const { return counts[TOTAL_TERMS]; };

  /// Average document length 
  float docLengthAvg() const;

  /// Total counts of doc with a given term
  COUNT_T docCount(TERMID_T termID) const;

  /// Total counts of terms in a document, including stop words
  COUNT_T docLength(DOCID_T docID) const;

  /// Total count of terms in given document, not including stop words
  virtual COUNT_T docLengthCounted(DOCID_T docID) const;

  //@}

  /// @name Index entry access
  //@{
  /// doc entries in a term index, @see DocList @see InvFPDocList
  DocInfoList* docInfoList(TERMID_T termID) const;

  /// word entries in a document index (bag of words), @see TermList
  TermInfoList* termInfoList(DOCID_T docID) const;

  //@}

  /// set the mesg stream
 void setMesgStream(ostream * lemStream);

protected:
  /// readin all toc
  bool fullToc(const string &fileName);
  /// readin index lookup table
  bool indexLookup();
  /// readin inverted index filenames map
  bool invFileIDs();
  /// read in document manager internal and external ids map
  bool docMgrIDs();
  /// read in dt index lookup table of format ver1.9 (and up?)
  bool dtLookup();
  /// read in dt index lookup table of format older than ver1.9 
  bool dtLookup_ver1();
  /// read in dt index filenames map
  bool dtFileIDs();
  /// read in termIDs to term spelling map
  bool termIDs();
  /// read in docIDs to doc spelling map
  bool docIDs();


  LOC_T* counts;    // array to hold all the overall count stats of this db
  string *names;  // array to hold all the names for files we need for this db
  float aveDocLen; // the average document length in this index
  inv_entry* lookup;  // the array holding entries (index is termid)
  dt_entry* dtlookup; // the array holding entries to dt index (index of array is docid)
  int dtloaded; // indicate load status of the dt index (loaded or not)
  TERM_T* terms;   // array of the term spellings (index is termid)
  EXDOCID_T* docnames; // array of the external docids (index is docid)
  string * dtfiles; // array of dt index filenames
  ifstream *dtfstreams; // array of dt index input streams
  string * invfiles; // array of inv index filenames
  ifstream *invfstreams; // array of inv index input streams
  vector<DocumentManager*> docmgrs; // list of document managers
  // we make them mutable, but they don't actually get changed. this seems to be necessary for map::find method
  mutable map<TERM_T, TERMID_T, less<TERM_T> > termtable; // table of terms to termid
  mutable map<EXDOCID_T, DOCID_T, less<EXDOCID_T> > doctable; // table of exdocids to docid
  ostream* msgstream; // Lemur code messages stream		
};

#endif
