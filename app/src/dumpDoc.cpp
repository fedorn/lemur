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
    cerr << "usage: dumpDoc <index_name> <internal/external docid> [-ext]" 
	 << endl;
    exit (1);
  }
  ind = IndexManager::openIndex(argv[1]);
  DOCID_T did;
  if (argc == 3)
    did = atoi(argv[2]);
  else did = ind->document(argv[2]);
  cout << ind->document(did) << ": " << ind->docLength(did) << endl;
  TermInfoList *tList = ind->termInfoList(did);
  if (tList == NULL) {
    cerr << ": empty termInfoList" << endl;
    exit (1);
  }
  TermInfo *info;
  tList->startIteration();
  while (tList->hasMore()) {
    info = tList->nextEntry();
    cout << info->termID() << ":" << ind->term(info->termID()) << ": " << info->count() << endl;
  }
  delete tList;
  delete(ind);
  return 0;
}
