#ifndef _OKAPIRETMETHOD_HPP
#define _OKAPIRETMETHOD_HPP

#include "QueryRep.hpp"
#include "ScoreFunction.hpp"
#include "RetrievalMethod.hpp"
#include <math.h>

/// Okapi retrieval model parameter namespace

namespace OkapiParameter {
  struct TFParam {
    double k1;
    double b;
    double k3;
    double expQTF; // expanded query term TF
  }; 
};

/// Represent of a query term in Okapi retrieval model, the term carries a count of the number of rel docs with the term

class OkapiQueryTerm : public QueryTerm {
public:
  OkapiQueryTerm(int termID, double count, int pEstCount, double paramK3) : QueryTerm(termID, count), pEst(pEstCount), k3(paramK3) {
  }
  int pEstCount() { return pEst;}
  double weight() { 
    return ((k3+1)*w/(k3+w));
  } 
private:
  int pEst;
  double k3;
};


class OkapiScoreFunc : public ScoreFunction {
public:
  OkapiScoreFunc(Index &dbIndex): ind(dbIndex) {}
  virtual double matchedTermWeight(QueryTerm *qTerm, QueryRep *qRep, DocInfo *info, DocumentRep *dRep);
protected:
  Index &ind;
};


/// OkapiQueryRep carries an array to store the count of relevant docs with a term 
class OkapiQueryRep : public ArrayQueryRep {
public:
  // initial query constructor, no feedback docs assumed
  OkapiQueryRep(TextQuery &qry, Index &dbIndex, double paramK3);

  virtual ~OkapiQueryRep() { delete [] pEst; }
  int pNormCount() { return pNorm;}
  void setPNormCount(int count) { pNorm = count;}
  void incPEst(int wdIndex, int val) { pEst[wdIndex]+=val;}
protected:
  virtual QueryTerm *makeQueryTerm(int wdIndex, double wdCount) {
    return (new OkapiQueryTerm(wdIndex, wdCount, pEst[wdIndex], k3));
  }
  double k3;
  int pNorm;
  int *pEst;
};

/// Doc representation for Okapi model

class OkapiDocRep : public DocumentRep {
public:
  OkapiDocRep(int docID, Index &dbIndex, OkapiParameter::TFParam &param) : DocumentRep(docID), ind(dbIndex),
  prm(param) {
  }
  virtual ~OkapiDocRep() { }
  virtual double termWeight(int termID, DocInfo *info);
  double BM25TF(double rawTF, double docLength);
  virtual double scoreConstant() { return 0;}
protected:
  Index &ind;
  OkapiParameter::TFParam &prm;
};


class OkapiRetMethod : public RetrievalMethod  {
public:
  OkapiParameter::TFParam param;
  int expTermCount;

  OkapiRetMethod(Index &dbIndex);

  virtual ~OkapiRetMethod() { delete scFunc;}

  virtual QueryRep *computeQueryRep(TextQuery &qry) {
    return (new OkapiQueryRep(qry, *ind, param.k3));
  }

  virtual DocumentRep *computeDocRep(int docID) {
    return (new OkapiDocRep(docID, *ind, param));
  }

  virtual ScoreFunction *scoreFunc();
  
  virtual void updateQuery(QueryRep &origRep, DocIDSet &relDocs);

  static double RSJWeight(double r, double R, 

			  double n, double N) {
    return (log ((r+0.5)*(N-n-R+r+0.5)/((R-r+0.5)*(n-r+0.5))));
  }

private:
  OkapiScoreFunc *scFunc;
  


};

inline double OkapiDocRep::BM25TF(double rawTF, double docLength) 
{
  return ((prm.k1+1)*rawTF/(rawTF +  prm.k1*(1-prm.b + prm.b*docLength/ind.docLengthAvg())));
}


inline double OkapiDocRep::termWeight(int termID, DocInfo *info)
{
  return BM25TF(info->termCount(), ind.docLength(info->docID()));
} 




inline ScoreFunction *OkapiRetMethod::scoreFunc()
{
  return scFunc;
}

#endif /* _OKAPIRETMETHOD_HPP */





