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
/*! \page GenerateSmoothSupport  GenerateSmoothSupport Application

This application generates two support files for retrieval using the
language modeling approach. Both files contain some pre-computed
quantities that are needed to speed up the retrieval process.  One file
(name given by the parameter <tt>smoothSupportFile</tt>, see below) is
needed by retrieval using smoothed unigram language model. Each entry in
this support file corresponds to one document and records two pieces of
information: (a) the count of unique terms in the document; (b) the sum
of collection language model probabilities for the words in the
document.  The other file (with an extra suffix "<tt>.mc</tt>" is needed
if you run feedback based on the Markov chain query model. Each line in
this file contains a term and a sum of the probability of the word given
all documents in the collection. (i.e., a sum of <tt>p(w|d)</tt> over
all possible <tt>d</tt>'s.)

To run the application, follow the general steps of running a lemur
application and set the following variables in the parameter file:
<ol>

<li><tt>index</tt>: the table-of-content (TOC) record file of the index.

<li> <tt>smoothSupportFile</tt>: file path for the support file
(e.g.,<tt> /usr0/mydata/index.supp</tt>)
</ol>
*/


#include <cassert>
#include "common_headers.hpp"

#include "IndexManager.hpp"
#include "Param.hpp"

using namespace lemur::api;

namespace LocalParameter {
  static std::string index;
  static std::string smoothSupportFile;
  
  void get() {
    index = ParamGetString("index");
    smoothSupportFile = ParamGetString("smoothSupportFile");
  }
};

void GetAppParam()
{
  LocalParameter::get();
}


int AppMain(int argc, char *argv[]) {


  ofstream ofs;


  Index * ind;

  try {
    ind = IndexManager::openIndex(LocalParameter::index);
  } 
  catch (Exception &ex) {
    ex.writeMessage();
    throw Exception("GenerateSmoothSupport", "Can't open index, check parameter index");
  }

  ofs.open(LocalParameter::smoothSupportFile.c_str());


  char mcSuppFileName[500];
  strcpy(mcSuppFileName, LocalParameter::smoothSupportFile.c_str());
  strcat(mcSuppFileName, ".mc");

  ofstream mcOFS;
  mcOFS.open(mcSuppFileName);

  TERMID_T i;


  double *wdPr = new double[ind->termCountUnique()+1];
  for (i=1; i<=ind->termCountUnique();i++)
    wdPr[i]=0;

  double prSum=0;
  for (i=1; i<= ind->docCount(); i++) {
    prSum =  0;
    TermInfoList *tList = ind->termInfoList(i);
    tList->startIteration();
    int size=0;
    while (tList->hasMore()) {
      TermInfo *info = tList->nextEntry();

      wdPr[info->termID()] += info->count()/(double)ind->docLength(i);
      // compute Markov chain support

      prSum += ind->termCount(info->termID());
        

      // cout << i << "\t" <<  ind->termCount(info->id()) << "\t" << ind->term(info->id()) << endl;

      size++;
    }
    // should be ML here
    //    prSum = prSum / (double)(ind->termCount()+ind->termCountUnique());
    prSum = prSum/(double)ind->termCount();
    ofs << i << " " << size << " "<< prSum << endl;
    delete tList;
  }

  for (i=1;i<=ind->termCountUnique();i++) 
    mcOFS << i << " " << wdPr[i] << endl;
  mcOFS.close();

  ofs.close();
  delete [] wdPr;
  return 0;

}
