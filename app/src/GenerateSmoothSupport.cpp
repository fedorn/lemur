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


/// Smoothing support file generator

/*! \page GenerateSmoothSupport  GenerateSmoothSupport Application

 

This application (GenerateSmoothSupport.cpp) generates a supporting file needed by 
retrieval using smoothed unigram language model based on BasicIndex.

To run the application, follow the general steps of running a lemur
application and set the following variables in the parameter file:

(1) index: the table-of-content (TOC) record file of the index (i.e., the .bsc file created by 
BuildBasicIndex, e.g., /usr0/mydata/index.bsc)

(2) smoothSupportFile: file path for the support file
(e.g., /usr0/mydata/index.supp)

Each entry in the support file corresponds to one document and records
two pieces of information: (a) the count of unique terms in the document;
(b) the sum of collection language model probabilities for the words in the document.

This application is also a good example of using the doc index (i.e., doc->term index).

*/


#include <fstream.h>
#include <cassert>
#include <cstdlib>
#include <cstdio>

#include "IndexManager.hpp"
#include "BasicIndex.hpp"
#include "IndexCount.hpp"
#include "Param.hpp"
#include "String.hpp"


namespace LocalParameter {
  static String index;
  static String smoothSupportFile;
  
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

  Index * ind = IndexManager::openIndex(LocalParameter::index);


  ofs.open(LocalParameter::smoothSupportFile);


  char mcSuppFileName[500];
  strcpy(mcSuppFileName, LocalParameter::smoothSupportFile);
  strcat(mcSuppFileName, ".mc");

  ofstream mcOFS;
  mcOFS.open(mcSuppFileName);

  int i;


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

      wdPr[info->id()] += info->count()/(double)ind->docLength(i);
      // compute Markov chain support

      prSum += ind->termCount(info->id());
	

      // cout << i << "\t" <<  ind->termCount(info->id()) << "\t" << ind->term(info->id()) << endl;

      size++;
    }
    prSum = prSum / (double)(ind->termCount()+ind->termCountUnique());
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




