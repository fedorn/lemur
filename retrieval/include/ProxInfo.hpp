/*==========================================================================
 * Copyright (c) 2002-2003 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/
/*
  author: fff, dmf
  * split out into separate header. Rewrite to use DocInfoList w/o copy.
*/
#ifndef _PROXINFO_HPP_
#define _PROXINFO_HPP_

#include "InvFPIndex.hpp"

/// Proximity information for a term within a document. Provides support
/// for ordered and unordered window operators. Used to prune document
/// lists before scoring the documents.
class ProxInfo {
public: 
  /// Make an empty one.
  ProxInfo() : nextPos(0), posList(NULL), size(0),
	       dList(NULL), listSize(0) { }
  /// Make one from an InvFPDocList
  ProxInfo::ProxInfo(int num, int tf, DocInfoList *dl);
  
  /// Make one from a doc positions list.
  ProxInfo(int  num, int tf, int *pl);
  /// clean up.
  ~ProxInfo() { delete(dList); delete[](posList);}

  /// document id of current entry
  int id() {return currEntry.docID();}
  /// term freq of current entry
  int count() {return currEntry.termCount();}
  /// positions array of current entry
  int *positions() {return currEntry.positions();}
  /// return position value at index k in the current entry's
  /// position list
  int position(int k) {return *(positions() + k);}
  /// Iteration support (restartable).
  void startIteration() {
    if (dList != NULL) dList->startIteration();
  }
  /// have any more?
  bool hasMore() {return(dList != NULL && dList->hasMore());}
  /// Advance to next document, return false if no more docs.
  bool nextDoc();
  /// Advance to document did, return false if not in list and
  /// stop on next doc after did.
  bool nextDoc(int did);
  /// how long is the list of positions for all docs.
  int posSize();  
  /// window size
  int size;
  /// next position of this term in document
  /// used by unordered window // unneeded?
  int nextPos;
private:
  /// list of positions in document for operators
  int *posList; // internally allocated, must use delete[]()
  /// contains list of positions in document for terms
  InvFPDocList *dList;
  InvFPDocInfo currEntry;
  int listSize;
};
#endif
