
/// Evaluating a query model using the KL-divergence retrieval function



/*! \page QueryModelEval  Evaluating an (expanded) query model with the KL-divergence retrieval model

This application loads an expanded query model and evaluates it with
the KL-divergence retrieval model.

Parameters:

1) "index": the index table-of-content file for the database index. It should have a file extension with exactly three characters. The convention assumed is
".lmr" or ".LMR" means the basic indexer and ".toc" or ".TOC" means the InvFP indexer. (These extensions should be modified to reflect the characteristics of the corresponding indexer, but this has not been done yet.)

2) "queryModelFile": the file of the expanded query model 

3) "resultFile": the result file 

4) "resultCount": the number of documents to return for each query

4) "feedbackDocCount": the number of docs to use for pseudo-feedback (0 means no-feedback)

5) "feedbackTermCount": the number of terms to add to a query when doing feedback

6) "expandedQuery": the expanded query file

7) "feedbackMethod": feedback method (0, 1, 2, 3 for maximum-likelihood, mixture model, divergence minimization, and Markov chain respectively).


8) other feedback-method-specific parameters 


 */


#include "Param.hpp"
#include "IndexManager.hpp"
#include "BasicDocStream.hpp"
#include <iostream.h>
#include "SimpleKLRetMethod.hpp"
#include "RetrievalEngine.hpp"

String inputIndex;
String inputQuerySet;
String inputResultFile;
String inputQueryModelFile;

void GetAppParam()
{
 inputIndex = ParamGetString("index");
 inputQuerySet = ParamGetString("querySet", "query");
 inputResultFile = ParamGetString("resultFile", "result");
 inputQueryModelFile = ParamGetString("queryModelFile", "expQuery");
}

int maxCountOfResult = ParamGetInt("resultCount", 1000);

static void writeResults(char *queryID, IndexedRealVector *results, Index *ind, int maxCountOfResult, ostream *resStream)
{
  IndexedRealVector::iterator j;
  int count=0;
  for (j= results->begin();j!=results->end();j++) {
    if (count >= maxCountOfResult) {
      break;
    } else {
      *resStream << queryID << " "  
	  << ind->document((*j).ind) << " " 
	  <<  (*j).val << endl;
      count++;

    }
  }  
  resStream->flush();
}


/// A query model estimation program
int AppMain(int argc, char *argv[]) {
  
  Index  *ind = IndexManager::openIndex(inputIndex);
  ifstream qmodel(inputQueryModelFile, ios::in);
  if (qmodel.fail()) {
    throw Exception("AppMain", "can't open the query model file, check the value for parameter queryModelFile");
  }

  ofstream os(inputResultFile);


  SimpleKLRetMethod model(*ind);

  RetrievalEngine eng(model);

  IndexedRealVector res;
  char qid[300];

  SimpleKLQueryModel *q;
  while (qmodel >> qid) {
    q = new SimpleKLQueryModel(*ind);
    q->load(qmodel);
    eng.scoreInvertedIndex(*q,res);
    writeResults(qid, &res, ind, maxCountOfResult, &os);
    delete q;
  }
  
  os.close();

  delete ind;
  return 0;
}


