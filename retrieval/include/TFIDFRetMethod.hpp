#ifndef _TFIDFRETMETHOD_HPP
#define _TFIDFRETMETHOD_HPP

#include "RetrievalMethod.hpp"

/// Parameters used in the TFIDF retrieval method
namespace TFIDFParameter {

  enum TFMethod  {RAWTF=0, LOGTF=1, BM25=2};
  struct WeightParam {
    TFMethod tf;
    double bm25K1;
    double bm25B;
  };
};

/// Representation of a query (as a weighted vector) in the TFIDF method
class TFIDFQueryRep : public ArrayQueryRep {
public:
  TFIDFQueryRep(TextQuery &qry, Index &dbIndex, double *idfValue, TFIDFParameter::WeightParam &param);

  virtual ~TFIDFQueryRep() {}

  double queryTFWeight(const double rawTF);
protected:
  TFIDFParameter::WeightParam &prm;
  double *idf;
  Index &ind;
};

/// Representation of a doc (as a weighted vector) in the TFIDF method
class TFIDFDocRep : public DocumentRep {
public:
  TFIDFDocRep(int docID, Index &dbIndex, double *idfValue,
	      TFIDFParameter::WeightParam &param) : 
    DocumentRep(docID), ind(dbIndex), prm(param), idf(idfValue) {
  }
  virtual ~TFIDFDocRep() { }
  virtual double termWeight(int termID, DocInfo *info) { 
    return (idf[termID]*docTFWeight(info->termCount())); 
  }
  virtual double scoreConstant() { return 0;}

  double docTFWeight(const double rawTF);
private:

  Index & ind;
  TFIDFParameter::WeightParam &prm;
  double *idf;
};


/// The TFIDF retrieval method with a few TF formula options

class TFIDFRetMethod : public RetrievalMethod {
public:

  TFIDFParameter::WeightParam qryTFParam;
  TFIDFParameter::WeightParam docTFParam;
  int howManyTerms;
  double posCoeff;
  TFIDFRetMethod(Index &dbIndex);
  ~TFIDFRetMethod() {delete [] idfV; delete scFunc;}

  virtual QueryRep *computeQueryRep(TextQuery &qry) {
    return (new TFIDFQueryRep(qry, *ind, idfV, qryTFParam));
  }

  virtual DocumentRep *computeDocRep(int docID) { 
    return (new TFIDFDocRep(docID, *ind, idfV, docTFParam));
  }
  virtual ScoreFunction *scoreFunc() {
    return (scFunc);
  }


  virtual void updateQuery(QueryRep &qryRep, DocIDSet &relDocs);

  static double BM25TF(const double rawTF, const double k1, const double b, 
		       const double docLen, const double avgDocLen);

protected:
  double *idfV;
  ScoreFunction *scFunc;
};





inline double TFIDFRetMethod ::BM25TF(const double rawTF, const double k1, const double b, 
		     const double docLen, const  double avgDocLen)
{
  double x= rawTF+k1*(1-b+b*docLen/avgDocLen);
  return (k1*rawTF/x);
}



#endif /* _TFIDFRETMETHOD_HPP */







