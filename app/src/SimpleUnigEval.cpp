
/// Simple Unigram Retrieval Model Evaluation Program
/*!

This application evaluates
the  unigram language model retrieval approach on a testing
collection composed of a query source stream (query set)
 and an indexed document set. 

To run the application, use a parameter file that has definitions for
the following input variables. 

1) "lemur": the lemur file for the database index

2) "querySet": the query text stream 

3) "resultFile": the result file

4) "resultCount": the number of documents to return for each query

5) "smoothMethod": language model smoothing method 
(0,1,2 for Jelinek-Mercer, Dirichlet-Prior, and Absolute-discounting respectively)

6) "discountDelta": delta value for additive smoothing

7) "JelinekMercerLambda": lambda value for Jelinek-Mercer smoothing

8) "BayesianPrior": prior count for Bayesian smoothing

9) "smoothStrategy": interpolation (0) vs. backoff (1)

10) "queryFidelity": topic weight for the query mixture model  
(1 means no collection model mixed, so essentially the query-likelihood model)


 */
//---------------------------------------------------------------
//APPLICATION: Simple Unigram Retrieval Model Evaluation Program 
//                  (C. Zhai 1/11/2000)
//
// It takes two required input variables
//   1) "lemur": the lemur file for the database index
//   2) "querySet": the query text stream 
//   3) "resultFile": the result file

// It writes the retrieval results to the resultFile 

// It recognizes the following parameters
// 
//   1) "resultCount": the number of documents to return for each query
//   2) "smoothMethod": language model smoothing method
//   3) "discountDelta": delta value for additive smoothing
//   4) "JelinekMercerLambda": lambda value for Jelinek-Mercer smoothing
//   5) "BayesianPrior": prior count for Bayesian smoothing
//
//---------------------------------------------------------------


#include "SimpleUnigModel.hpp"


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
#include "SimpleUnigModel.hpp"


#define MIN(a,b) (a<=b) ? a : b

BasicIndex lemur;


enum SmoothMethod  {JELINEKMERCER=0, BAYESIAN=1, ABSOLUTEDISCOUNT=2} SmoothMethod;

enum SmoothStrategy  {INTERPOLATE=0, BACKOFF=1}; 
// don't change the value association, implicit type conversion
// depends on it. 


String inputLemur = ParamGetString("lemur");
String inputQuerySet = ParamGetString("querySet");
String inputResultFile = ParamGetString("resultFile");

String inputExpandedQueryFile = ParamGetString("expandedQueryFile");
String inputSmoothSupportFile = ParamGetString("smoothSupportFile");

int inputResultCount = ParamGetInt("resultCount",1000);
int inputSmoothMethod = ParamGetInt("smoothMethod", JELINEKMERCER);
int inputSmoothStrategy = ParamGetInt("smoothStrategy", INTERPOLATE);

double inputDiscountDelta = ParamGetDouble("discountDelta",0.5);
double inputJelinekMercerLambda = ParamGetDouble("JelinekMercerLambda",0.5);
double inputBayesianPrior = ParamGetDouble("BayesianPrior",2000);
double inputQueryFidelity = ParamGetDouble("queryFidelity",1);



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

  ifstream ifs;
  ifs.open(inputSmoothSupportFile);
  
  int numDocs = lemur.docCount();

  double *docProbMass = new double[numDocs];
  int *uniqueTermCount = new int[numDocs];
  
  int jj;
  
  for (jj=0; jj< numDocs; jj++) {
    int id, uniqCount;
    double prMass;
    ifs >> id >> uniqCount >> prMass;
    if (id != jj) {
      cerr << "alignment error in smoothing support file, wrong id:" << id << endl; 
      exit(1);
    }
    docProbMass[jj]=prMass;
    uniqueTermCount[jj]=uniqCount;
  }


  ifs.close();

  int numTerms = lemur.termCountUnique();
  static double *collectPr = new double[numTerms];

  for (jj=0; jj< numTerms; jj++) {
    collectPr[jj] = (lemur.termCount(jj)+1)/
      (double)(numTerms+lemur.termCount());
  }



  SimpleDocUnigramModel *docModel;
  
  switch (((enum SmoothMethod)inputSmoothMethod)) {
  case JELINEKMERCER:
    docModel = new JelinekMercerDocModel(&lemur, 
					 collectPr,
					 docProbMass,
					 inputJelinekMercerLambda,
					 inputSmoothStrategy);
    break;
  case BAYESIAN:
    docModel = new BayesianDocModel(&lemur, 
				    collectPr,
				    docProbMass,  
				    inputBayesianPrior,
				    inputSmoothStrategy);
    break;
  case ABSOLUTEDISCOUNT:
    docModel = new AbsoluteDiscountDocModel(&lemur, 
					    collectPr,
					    docProbMass,
					    uniqueTermCount,
					    inputDiscountDelta,
					    inputSmoothStrategy);
    break;
  default:
    cerr << "Unknown language model smoothing method\n";
    exit(1);
  }
  
  ofstream result (inputResultFile);
  BasicDocStream queryStream(inputQuerySet);

  IndexedRealVector scores(0);


  static double *queryArray = new double[numTerms];

  char queryID[500];
  double querySum;
  int i;
  if ( strcmp(inputExpandedQueryFile,"")) {
    ifs.open(inputExpandedQueryFile);
    // use expanded query
    int count;
    while (ifs>> queryID && ifs >> count) {
      if (count <=0 ) {
	cerr << "unexpected empty query\n";
	exit(1);
      }
      for (i=0; i<numTerms; i++) {
	queryArray[i]=0;
      }
      querySum =0;
      char word[300];
      double pr;
      while (count--) {
	ifs >> word >> pr;
	queryArray[lemur.term(word)]=pr;
	querySum += pr;
      }
      cerr << "query :"<< queryID << endl;
      SimpleUnigramModel::Retrieve(queryArray,
				   querySum,
				   collectPr,
				   lemur, 
				   scores,
				   *docModel,
				   inputQueryFidelity);
      scores.Sort();
      PrintResults(result,queryID,&scores);
      
    }
  } else {
    // read query source stream
    queryStream.startDocIteration();
    while (queryStream.hasMore()) {
      Document *doc = queryStream.nextDoc();
      strcpy(queryID, doc->getID());
      for (int i=0; i< numTerms; i++) 
	queryArray[i]=0;
      querySum=0;
      
      doc->startTermIteration();
      while (doc->hasMore()) {
	TokenTerm *t = doc->nextTerm();
	int termIndex = lemur.term(t->spelling());
	queryArray[termIndex]++;
	querySum++;
      }

      cerr << "query :"<< queryID << endl;
      SimpleUnigramModel::Retrieve(queryArray,
				   querySum,
				   collectPr,
				   lemur, 
				   scores,
				   *docModel,
				   inputQueryFidelity);

      scores.Sort();
      PrintResults(result,queryID,&scores);
    }
  }
  result.close();
  return 0;
}


