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

#include "ResultFile.hpp"


void ResultFile::openForRead(istream &is, Index &index)
{
  inStr=&is;
  ind = &index;
  eof = !readLine();
}

void ResultFile::load(istream &is, Index &index)
{
  int i;
  static ResultEntry entry;

  // delete any old data
  for (i=0; i<resTable->size(); i++) {
    entry = (*resTable)[i];
    delete entry.res;
  }

  resTable->clear();
 
  inStr=&is;
  ind = &index;
  while (readLine()) {
    entry.key = curQID;
    int qi = (*resTable)[entry];
    if (qi<0) {
      entry.res = new IndexedRealVector();
      qi = resTable->add(entry);
    }
    entry = (*resTable)[qi];
    entry.res->PushValue(ind->document(curDID), curSC);  
  }
}

bool ResultFile::findResult(char *queryID, IndexedRealVector *&res)
{
  static ResultEntry entry;
  entry.key = queryID;
  int qi = (*resTable)[entry];
  if (qi>=0) {
    entry = (*resTable)[qi];
    res = entry.res;
    return true;
  } else {
    res = NULL;
    return false;
  }
}

void ResultFile::getResult(char *expectedQID, IndexedRealVector &res)
{
  res.clear();
  if (eof || strcmp(curQID, expectedQID)) {
    if (!eof) {
      cerr << "expected query ID: "<< expectedQID << " actual ID: "<<curQID <<endl;
    } else {
      cerr << "Unexpected end of the result file\n";
    }
    cerr.flush();
    
    throw Exception("ResultFile", "query id mismatch between the original query and the result file");
  }
  do {
    res.PushValue(ind->document(curDID), curSC);
    if (! readLine()) {
      // end of file
      eof = true;
      break;
    }
  } while (!strcmp(curQID, expectedQID));
}

void ResultFile::writeResults(char *queryID, IndexedRealVector *results, int maxCountOfResult)
{
  IndexedRealVector::iterator j;
  int count=0;
  for (j= results->begin();j!=results->end();j++) {
    if (count >= maxCountOfResult) {
      break;
    } else {
      *outStr << queryID 
	      << (trecFmt ? " Q0 ":" ")  
	      << ind->document((*j).ind) << " " 
	      << (trecFmt? " 0 ":" ")  
	      <<  (*j).val 
	      << (trecFmt? " Exp ":" ")  
	      << endl;
      count++;
      
    }
  }  
  outStr->flush();
}



bool ResultFile::readLine()
{
    char dummy1[100];
    char dummy2[100];
    char dummy3[100];

    if (trecFmt) {
      return (*inStr >> curQID >> dummy1 >> curDID >> dummy2 >> curSC >> dummy3);
    } else {
      return (*inStr >> curQID >> curDID >> curSC);
    }
}


