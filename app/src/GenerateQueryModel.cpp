
/// Generating a query model to be used in the KL-divergence retrieval model



/*! \page GenerateQueryModel  Estimating an (expanded) query model for KL-divergence retrieval model

This application computes an expanded query model based on feedback documents and the original query model. 

Parameters:

1) "index": the index table-of-content file for the database index. It should have a file extension with exactly three characters. The convention assumed is
".lmr" or ".LMR" means the basic indexer and ".toc" or ".TOC" means the InvFP indexer. (These extensions should be modified to reflect the characteristics of the corresponding indexer, but this has not been done yet.)

2) "originalQuery": the original query text stream 

3) "resultFile": the result file to be used for feedback

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
#include "ParamManager.hpp"
#include "ResultFile.hpp"


namespace LocalParameter {
  String expandedQuery;
  bool TRECResultFormat;
  void get() {
    expandedQuery = ParamGetString("expandedQuery");
    TRECResultFormat = ParamGetInt("resultFormat",1); // default is TREC format
  }
};

void GetAppParam()
{
  LocalParameter::get();
  RetrievalParameter::get();
  SimpleKLParameter::get();
  
}



/// A query model estimation program

int AppMain(int argc, char *argv[]) {
  
  Index  *ind = IndexManager::openIndex(RetrievalParameter::databaseIndex);
  DocStream *qryStream = new BasicDocStream(RetrievalParameter::textQuerySet);

  ifstream result(RetrievalParameter::resultFile, ios::in);
  if (result.fail()) {
    throw Exception("AppMain", "can't open the result file, check parameter value for resultFile");
  }

  ofstream os(LocalParameter::expandedQuery);
  
  ResultFile resFile(LocalParameter::TRECResultFormat);
  resFile.openForRead(result, *ind);
  SimpleKLRetMethod *model =  new SimpleKLRetMethod(*ind, SimpleKLParameter::smoothSupportFile);
  model->setDocSmoothParam(SimpleKLParameter::docPrm);
  model->setQueryModelParam(SimpleKLParameter::qryPrm);

  IndexedRealVector res;

  qryStream->startDocIteration();
  TextQuery *q;
  while (qryStream->hasMore()) {
    Document *d = qryStream->nextDoc();
    q = (TextQuery *) d;
    cout << "query : "<< q->getID() << endl;
    QueryRep *qr = model->computeQueryRep(*q);
    resFile.getResult(q->getID(), res);
    res.Sort();
    PseudoFBDocs *topDoc = new PseudoFBDocs(res, RetrievalParameter::fbDocCount);
    model->updateQuery(*qr, *topDoc);
    SimpleKLQueryModel *qm = (SimpleKLQueryModel *) qr;
    os << q->getID();
    qm->save(os);
    delete qr;
    delete topDoc;
  }

  os.close();

  delete qryStream;
  delete ind;
  return 0;
}


