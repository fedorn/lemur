
#include "Param.hpp"
#include "IndexManager.hpp"
#include "BasicDocStream.hpp"
#include <iostream.h>
#include "TFIDFRetMethod.hpp"
#include "SimpleKLRetMethod.hpp"
#include "OkapiRetMethod.hpp"
#include "RetrievalEngine.hpp"

String inputIndex = ParamGetString("index");
String inputQuerySet = ParamGetString("querySet", "query");
String inputResultFile = ParamGetString("resultFile", "result");


enum RetModel {TFIDF=0, OKAPI=1, KL=2};




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




int AppMain(int argc, char *argv[]) {
  
  Index  *ind = IndexManager::openIndex(inputIndex);
  DocStream *qryStream = new BasicDocStream(inputQuerySet);
  ofstream result(inputResultFile);

  int mod = ParamGetInt("retModel",3); // default is KL divergence model

  int fbDocCount = ParamGetInt("feedbackDocCount",0); 
  int maxCountOfResult = ParamGetInt("resultCount", 1000);

  IndexedRealVector *results;

  RetrievalMethod *model;

  // construct retrieval model

  switch (mod) {
  case 0: 
    model = new TFIDFRetMethod(*ind);
    break;
  case 1:
    model = new OkapiRetMethod(*ind);
    break;
  case 2:
    model = new SimpleKLRetMethod(*ind);
    break;
  default:
    throw Exception("RetrievalExp", "unknown retModel parameter");
    break;
    
  }

  RetrievalEngine eng(*model);
  qryStream->startDocIteration();
  TextQuery *q;
  while (qryStream->hasMore()) {
    Document *d = qryStream->nextDoc();
    q = (TextQuery *) d;
    cout << "query : "<< q->getID() << endl;
    if (fbDocCount>0) {
      eng.retrievePseudoFeedback(*q, fbDocCount, results);
    } else {
      eng.retrieve(*q, results);
    }
    writeResults(q->getID(), results, ind, maxCountOfResult, &result);
  }

  result.close();
  delete qryStream;
  delete ind;
  return 0;
}


