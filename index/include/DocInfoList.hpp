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


#ifndef _DOCINFOLIST_HPP
#define _DOCINFOLIST_HPP




//!  Abstract Representation of Doc Information Entry 

/*! 

  This is an abstract class that represents the information associated
with each  doc entry in the term to doc index. The basic information 
includes  the doc ID and the frequency of a term in the document.

*/

class DocInfo {
public:
  DocInfo() {}
  DocInfo(int docID, int termCount) :
    did(docID), tcount(termCount) {}

  virtual ~DocInfo() {}
  
  /// doc id
  virtual int docID() const {return did;}
  
  /// term count in the doc
  virtual int termCount() const {return tcount;}

  /// set doc id
  virtual void docID(int id) {did = id;}

  /// set term count
  virtual void termCount(int c) {tcount = c;}

  // return list of positions this term occurs in this document
  // default implementation to return NULL if no position information available for this DocInfo
  virtual const int* positions() const { return NULL; }  

  /// set the list of positions
  virtual void positions(const int* pos) {}

protected:
  int did;
  int tcount;
};



//!  Abstract Interface of Doc Information List 

/*! 

  This is an abstract class that represents the doc entries in term index,
  i.e., the doc ID and the frequency of a term in the document.

*/

class DocInfoList {
public:
  virtual ~DocInfoList() {}
  
  /// prepare iteration
  virtual void startIteration() const=0;

  /// test if there's any entry
  virtual bool hasMore() const=0;

  /// fetch the next entry, return a pointer to a local static memory, so do not delete it
  virtual DocInfo *nextEntry() const=0;

};




#endif
