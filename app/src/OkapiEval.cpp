
/// Okapi Retrieval Evaluation Program 

/*!

This application evaluates the Okapi retrieval function on a
testing collection composed of a query source stream (query set)
and an indexed documnet  collection.

To run it, use a parameter file with definitions for the following variables:


1) "lemur": the lemur file for the database index

2) "querySet": the query text stream 

3) "resultFile": the result file

4) "resultCount": the number of documents to return for each query

5) "feedbackDocCount": the number of docs to use for pseudo-feedback (0 means no-feedback)

6) "feedbackTermCount": the number of terms to add to a query when doing feedback

7) "BM25K1" : BM25 K1

8) "BM25B" : BM25 B

9) "BM25AvgLen": BM25 avg doc length 

10) "BM25K3": BM25 K3 

11) "BM25QTF": The TF for expanded terms in feedback (the original paper
about the Okapi system is not clear about how this is set, so
it's implemented as a parameter.)


 */


//---------------------------------------------------------------
//APPLICATION: Okapi Retrieval Evaluation Program (C. Zhai 12/22/2000)
//
// It takes the following input variables
//   1) "lemur": the lemur file for the database index
//   2) "querySet": the query text stream 
//   3) "resultFile": result file name 
//   4) "resultCount": the number of documents to return for each query
//   5) parameters to control BM25 function
//
// It writes the retrieval results to the result file
//
//---------------------------------------------------------------


#include "OkapiModel.hpp"


#include <fstream.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <iomanip.h>
#include <strstream.h>

#include "BasicIndex.hpp"
#include "IndexCount.hpp"
#include "Param.hpp"
#include "String.hpp"
#include "BasicDocStream.hpp"

#define MIN(a,b) (a<=b) ? a : b


String inputLemur = ParamGetString("lemur");
String inputQuerySet = ParamGetString("querySet");
String inputResultFile = ParamGetString("resultFile", "tmpres");
int inputResultCount = ParamGetInt("resultCount",1000);

double inputBM25K1 = ParamGetDouble("BM25K1",1.2);
double inputBM25B = ParamGetDouble("BM25B",0.75);
int inputBM25AvgLen = ParamGetInt("BM25AvgLen", 0);
double inputBM25K3 = ParamGetDouble("BM25K3", 7);
double inputBM25QTF = ParamGetDouble("BM25QTF", 0.5);

int inputFeedbackDocCount = ParamGetInt("feedbackDocCount",0);
int inputFeedbackTermCount = ParamGetInt("feedbackTermCount",20);

BasicIndex lemur;


void PrintResults(ofstream &res, char *queryID, IndexedRealVector *sortedScores)
{
  IndexedRealVector::iterator j;
  int count=0;
  for (j= sortedScores->begin();j!=sortedScores->end();j++) {
    if (count >= inputResultCount) {
      break;
    } else {
      res << queryID << " "  
	  << lemur.document((*j).ind) << " " 
	  <<  (*j).val << endl;
      count++;

    }
  }  
}


int main() {
  lemur.open(inputLemur);
  
  // set average doc length if necessary
  if (!inputBM25AvgLen) inputBM25AvgLen = (int) lemur.docLengthAvg();

  OkapiModel::Parameter param(inputBM25AvgLen, inputBM25K1,
			      inputBM25B, inputBM25K3, inputBM25QTF);


  ofstream result (inputResultFile);
  BasicDocStream queryStream(inputQuerySet);

  int numTerms = lemur.termCountUnique();
  static double *queryVec  = new double[numTerms];

  IndexedRealVector scores(0);

  char queryID[500];
  
  queryStream.startDocIteration();

  while (queryStream.hasMore() ) {

    Document * doc = queryStream.nextDoc();

    strcpy(queryID, doc->getID());
    cerr << "query :"<< queryID << endl;
    for (int i =0; i<numTerms; i++) {
      // clear query terms
      queryVec[i]=0;
    }

    doc->startTermIteration();
    while (doc->hasMore()) {
      TokenTerm *t = doc->nextTerm();
      queryVec[lemur.term(t->spelling())]++;
    }

    OkapiModel::retrieve(queryVec, lemur, scores, param);
      
    scores.Sort();
    if (inputFeedbackDocCount>0) {
      cerr << "doing feedback...\n";
      OkapiModel::feedbackRetrieve(queryVec, scores, lemur, param,
				   inputFeedbackDocCount, 
				   inputFeedbackTermCount);      
      scores.Sort();
    }


    PrintResults(result,queryID,&scores);
  }
  result.close();
  return 0;
}

