/// Smoothing support file generator

/*!

 

This application  generates a supporting file needed by 
retrieval using smoothed unigram language model based on BasicIndex.

To run the application, follow the general steps of running a lemur
application and set the following variables in the parameter file:

(1) lemur: the lemur record file of the index (i.e., the .lemur file created by the indexer, e.g., /usr0/mydata/index.lemur)

(2) smoothSupportFile: file path for the support file
(e.g., /usr0/mydata/index.supp)

 
*/


#include <fstream.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "BasicIndex.hpp"
#include "IndexCount.hpp"
#include "Param.hpp"
#include "String.hpp"

#define MIN(a,b) (a<=b) ? a : b

String inputLemur = ParamGetString("lemur");
String inputSmoothSupportFile = ParamGetString("smoothSupportFile");
BasicIndex lemur;
ofstream ofs;

int AppMain(int argc, char *argv[]) {
  ParamInitialize(argc, argv);

  lemur.open(inputLemur);

  ofs.open(inputSmoothSupportFile);


  int i;

  double prSum=0;
  for (i=0; i< lemur.docCount(); i++) {
    prSum =  0;
    TermInfoList *tList = lemur.termInfoList(i);
    tList->startIteration();
    int size=0;
    while (tList->hasMore()) {
      TermInfo *info = tList->nextEntry();
      prSum += lemur.termCount(info->id())/
      (double)(lemur.termCount());
      size++;
    }
    ofs << i << " " << size << " "<< prSum << endl;
  }
  ofs.close();
  return 0;

}

