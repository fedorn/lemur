/// Smoothing support file generator

/*! \page GenerateSmoothSupport  GenerateSmoothSupport Application

 

This application  generates a supporting file needed by 
retrieval using smoothed unigram language model based on BasicIndex.

To run the application, follow the general steps of running a lemur
application and set the following variables in the parameter file:

(1) lemur: the lemur record file of the index (i.e., the .lemur file created by the indexer, e.g., /usr0/mydata/index.lemur)

(2) smoothSupportFile: file path for the support file
(e.g., /usr0/mydata/index.supp)

Each entry in the support file corresponds to one document and records
two pieces of information: (a) the count of unique terms in the document;
(b) the sum of collection language model probabilities for the words in the document.

*/


#include <fstream.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

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

  BasicIndex lemur;
  ofstream ofs;

  lemur.open(LocalParameter::index);

  ofs.open(LocalParameter::smoothSupportFile);

  int i;

  double prSum=0;
  for (i=1; i<= lemur.docCount(); i++) {
    prSum =  0;
    TermInfoList *tList = lemur.termInfoList(i);
    tList->startIteration();
    int size=0;
    while (tList->hasMore()) {
      TermInfo *info = tList->nextEntry();
      prSum += (lemur.termCount(info->id())+1)/
      (double)(lemur.termCount()+lemur.termCountUnique());
      size++;
    }
    ofs << i << " " << size << " "<< prSum << endl;
  }
  ofs.close();
  return 0;

}




