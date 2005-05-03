/*==========================================================================
 * Copyright (c) 2003-2004 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/
/*
  author: dmf
 */

#include "common_headers.hpp"
#include "IndexManager.hpp"
#include "Index.hpp"
using namespace lemur::api;

int main(int argc, char *argv[]) {
  Index *ind;
  if (argc < 3) {
    cerr << "usage: dumpTerm <index_name> <internal/external termid> [-ext]" 
	 << endl;
    exit (1);
  }
  
  ind = IndexManager::openIndex(argv[1]);
  TERMID_T did;
  if (argc == 3)
    did = atoi(argv[2]);
  else did = ind->term(argv[2]);
  
  cout << ind->term(did) << endl;
  DocInfoList *tList = ind->docInfoList(did);
  if (tList == NULL) {
    cerr << ": empty docInfoList" << endl;
    exit (1);
  }
  
  DocInfo *info;
  tList->startIteration();
  while (tList->hasMore()) {
    info = tList->nextEntry();
    const LOC_T *pos = info->positions();
    COUNT_T count = info->termCount();
    cout << ind->document(info->docID()) << "(" << count << "): ";
    if (pos != NULL) {
      for (COUNT_T i = 0; i < count; i++)
	cout << pos[i] << " ";
    }
    cout << endl;
  }
  delete tList;
  delete(ind);
  return 0;
}
