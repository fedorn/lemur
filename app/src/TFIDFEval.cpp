/// TFIDF Retrieval Evaluation Program



/*!

This application evaluates the TFIDF model on a testing collection
composed of a query source stream (query set) and an indexed database.

To run it, use a parameter that has definitions for the following
input variables:


1) "lemur": the lemur file for the database index

2) "querySet": the query text stream 

3) "resultFile": the result file

4) "resultCount": the number of documents to return for each query

5) "feedbackDocCount": the number of docs to use for pseudo-feedback (0 means no-feedback)

6) "feedbackTermCount": the number of terms to add to a query when doing feedback

7) "feedbackPosCoeff": the coefficient for positive terms in Rocchio feedback

8) "doc.tfMethod": document term TF weighting method

9) "doc.bm25K1": BM25 k1 for doc term TF

10) "doc.bm25B" : BM25 b for doc term TF

11) "doc.bm25AvgLen": BM25 avg doc length 

12) "query.tfMethod": query term TF weighting method

13) "query.bm25K1": BM25 k1 for query term TF

14) "query.bm25B" : BM25 b for query term TF

15) "query.bm25AvgLen": BM25 avg query length 


 */

//---------------------------------------------------------------
//APPLICATION: TFIDF Retrieval Evaluation Program (C. Zhai 12/1/2000)
//
// It takes two input variables
//   1) "lemur": the lemur file for the database index
//   2) "querySet": the query text stream 
//   3) "resultCount": the number of documents to return for each query
//
// It writes the retrieval results to the standard output
//
//---------------------------------------------------------------


#include "TFIDFModel.hpp"


#include <fstream.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <iomanip.h>
#include <strstream.h>

#include "BasicIndex.hpp"
#include "Param.hpp"
#include "String.hpp"
#include "BasicDocStream.hpp"

#define MIN(a,b) (a<=b) ? a : b
String inputLemur = ParamGetString("lemur");
String inputQuerySet = ParamGetString("querySet");
String inputResultFile = ParamGetString("resultFile");
int inputResultCount = ParamGetInt("resultCount",1000);

int inputFeedbackDocCount = ParamGetInt("feedbackDocCount",0);
int inputFeedbackTermCount = ParamGetInt("feedbackTermCount",20);
double inputFeedbackPosCoeff = ParamGetDouble("feedbackPosCoeff",4);

int inputDocTFMethod = ParamGetInt("doc.tfMethod",TFIDFModel::BM25);
double inputDocBM25K1 = ParamGetDouble("doc.bm25K1",1);
double inputDocBM25B = ParamGetDouble("doc.bm25B",0.5);
double inputDocBM25AvgLen = ParamGetDouble("doc.bm25AvgLen",0);

int inputQueryTFMethod = ParamGetInt("query.tfMethod",TFIDFModel::BM25);
double inputQueryBM25K1 = ParamGetDouble("query.bm25K1",1);
double inputQueryBM25B = ParamGetDouble("query.bm25B",0.5);
double inputQueryBM25AvgLen = ParamGetDouble("query.bm25AvgLen",20);


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



int AppMain(int argc, char *argv[]) {

  lemur.open(inputLemur);

  // set the right average doc length
  if (!inputDocBM25AvgLen) {
    inputDocBM25AvgLen = (int)lemur.docLengthAvg();
  }


  TFIDFModel::WeightParam docWeighting((TFIDFModel::TFMethod)inputDocTFMethod, inputDocBM25K1, inputDocBM25B, inputDocBM25AvgLen);
  TFIDFModel::WeightParam queryWeighting((TFIDFModel::TFMethod)inputQueryTFMethod, inputQueryBM25K1, inputQueryBM25B, inputQueryBM25AvgLen);

  ofstream result (inputResultFile);
  BasicDocStream queryStream(inputQuerySet);

  int numTerms = lemur.termCountUnique();
  static double *queryVec  = new double[numTerms];
  IndexedRealVector scores(0);

  char queryID[500];


  double querySum;

  queryStream.startDocIteration();

  while (queryStream.hasMore() ) {

    Document * doc = queryStream.nextDoc();
    
    strcpy(queryID, doc->getID());
    cerr << "query :"<< queryID << endl;
    for (int i =0; i<numTerms; i++) {
      // clear query terms
      queryVec[i]=0;
    }
    querySum =0;

    doc->startTermIteration();
    while (doc->hasMore()) {
      TokenTerm *t = doc->nextTerm();
      queryVec[lemur.term(t->spelling())]++;
      querySum ++;
    }




    TFIDFModel::retrieve(queryVec, querySum, 
			 lemur, scores, queryWeighting, docWeighting);  
    
    scores.Sort();
    if (inputFeedbackDocCount>0) {
      cerr << "doing feedback...\n";
      TFIDFModel::computeFeedbackQuery(queryVec, &scores,
				       lemur, docWeighting, 
				       inputFeedbackDocCount, 
				       inputFeedbackTermCount, 
				       inputFeedbackPosCoeff);      
      TFIDFModel::retrieve(queryVec, querySum, 
			   lemur, scores, queryWeighting, docWeighting, true);  
      scores.Sort();

    }
    PrintResults(result,queryID,&scores);
  }
  result.close();
  return 0;
}

