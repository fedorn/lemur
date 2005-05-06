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

#include "ResultFile.hpp"


void lemur::api::ResultFile::openForRead(istream &is, Index &index)
{
  inStr=&is;
  ind = &index;
  eof = !readLine();
}

void lemur::api::ResultFile::load(istream &is, Index &index)
{
  int i;
  lemur::utility::ResultEntry entry;

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
    DOCID_T did = ind->document(curDID);
    if (did>0) {
      entry.res->PushValue(did, curSC); 
    } else {
      cerr << "Warning: doc id "<< curDID << " not found in database\n";
    }
  }
}

bool lemur::api::ResultFile::findResult(const string& queryID, IndexedRealVector *&res)
{
  lemur::utility::ResultEntry entry;
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

void lemur::api::ResultFile::getResult(const string& expectedQID, IndexedRealVector &res)
{
  res.clear();
  //  if (eof || strcmp(curQID, expectedQID)) {
  if (eof || curQID != expectedQID) {
    if (!eof) {
      cerr << "expected query ID: "<< expectedQID << " actual ID: "<<curQID <<endl;
    } else {
      cerr << "Unexpected end of the result file\n";
    }
    cerr.flush();
    
    throw Exception("ResultFile", "query id mismatch between the original query and the result file");
  }
  do {

    DOCID_T did = ind->document(curDID);
    if (did>0) {
      res.PushValue(did, curSC); 
    } else {
      cerr << "Warning: doc id "<< curDID << " not found in database\n";
    }
    if (! readLine()) {
      // end of file
      eof = true;
      break;
    }
    //  } while (!strcmp(curQID, expectedQID));
  } while (curQID == expectedQID);
}

void lemur::api::ResultFile::writeResults(const string& queryID, IndexedRealVector *results, int maxCountOfResult)
{
  IndexedRealVector::iterator j;
  int count=0;
  for (j= results->begin();j!=results->end();j++) {
    if (count >= maxCountOfResult) {
      break;
    } else {
      count++;
      *outStr << queryID;
      if (trecFmt)
        *outStr << " Q0";
      *outStr << " "  << ind->document((*j).ind) << " " ;
      if (trecFmt)
        *outStr << count << " ";
      *outStr << (*j).val;
      if (trecFmt)
        *outStr << " Exp";
      //      *outStr << endl; //flushes buffer each call.
      *outStr << "\n";
    }
  }  
  outStr->flush();
}



bool lemur::api::ResultFile::readLine()
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


