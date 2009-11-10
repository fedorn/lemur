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

/*! \page GenL2Norm

This application generates a support file for retrieval using the cosine
similarity. The file contains the L2 norms for each document, used to
speed up the retrieval process.

To run the application, follow the general steps of running a lemur
application and set the following variables in the parameter file:
<ol>
<li><tt>index</tt>: the table-of-content (TOC) record file of the index.
<li><tt>L2File</tt>: filename for output of the support file
(e.g.,<tt> /usr0/mydata/index.L2</tt>)
</ol>
*/
/*
  author: dmf
 */

#include "common_headers.hpp"
#include <cmath>
#include "IndexManager.hpp"
#include "Param.hpp"

using namespace lemur::api;

namespace LocalParameter {
  static std::string index;
  static std::string L2File;
  
  void get() {
    index = ParamGetString("index");
    L2File = ParamGetString("L2File");
  }
};

void GetAppParam()
{
  LocalParameter::get();
}


int AppMain(int argc, char *argv[]) {
  ofstream ofs;
  Index * dbIndex;
  try {
    dbIndex = IndexManager::openIndex(LocalParameter::index);
  } 
  catch (Exception &ex) {
    ex.writeMessage();
    throw Exception("GenL2Norm", "Can't open index, check parameter index");
  }
  // pre-compute IDF values
  double *idfV = new double[dbIndex->termCountUnique()+1];
  TERMID_T i;
  for (i=1; i<=dbIndex->termCountUnique(); i++) {
    idfV[i] = log((dbIndex->docCount()+1)/(0.5+dbIndex->docCount(i)));
  }
  ofs.open(LocalParameter::L2File.c_str(), ios::out | std::ios::binary);
  for (i = 1; i <= dbIndex->docCount(); i++) {
    TermInfoList *qList = dbIndex->termInfoList(i);
    TermInfo *qInfo;
    qList->startIteration();
    TERMID_T idx;
    COUNT_T dtf;
    double norm = 0, tmp;
    while (qList->hasMore()) {
      qInfo = qList->nextEntry();
      idx = qInfo->termID();
      dtf = qInfo->count();
      tmp = dtf * idfV[idx];
      norm += tmp * tmp;
    }
    delete qList;
    //  docNorms[docID] = sqrt(norm);
    norm = sqrt(norm);
    if (norm == 0) norm = 1;
    ofs << i << " " << norm << endl;
  }
  ofs.close();
  delete[](idfV);
  delete dbIndex;
  return 0;
}




