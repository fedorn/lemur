#ifndef _OKAPIMODEL_HPP
#define _OKAPIMODEL_HPP

/// The Okapi retrieval model 
/*!


 An Exact implementation of the Okapi retrieval model based
 on Robertson's TREC paper. 
 
*/

//---------------------------------------------------------------
// CLASSES: OkapiModel (C. Zhai, 12/22/2000)
//
// OkapiModel: Exact implementation of the Okapi retrieval model 
// 
// Possible use: executing a query on a database
//
//---------------------------------------------------------------

#include <math.h>

#include "IndexedReal.hpp"
#include "Index.hpp"
#include "WorkingSet.hpp"



class OkapiModel  {
public:

  struct Parameter {
    Parameter(double avgLenValue, double k1Value=1.2, double bValue=0.75,
	      double k3Value=7, double expQTFValue=0.5):
      avgLen(avgLenValue), k1(k1Value), b(bValue), k3(k3Value), 
      expQTF(expQTFValue) { 
    }
    double avgLen;

    double k1;
    double b;
    double k3;
    double expQTF; // expanded query term TF
  };
 
  static Parameter defaultParam;


  /// retrieve using the BM25 retrieval function

  static void retrieve(double *query, // query TF vector
		       Index & dbIndex, // database
		       IndexedRealVector &scores, // results
		       const Parameter &param=defaultParam);
  
  

  /// retrieve using BM25 function with pseudo-feedback

  static void feedbackRetrieve(double *query, // query TF vector
			       IndexedRealVector &scores,
			       Index &dbIndex,
			       const Parameter &param,
			       int howManyDocs,
			       int howManyTerms);


private:

  static void scoreDocuments(double *weightedQuery, Index &dbIndex,
			     IndexedRealVector &scores, 
			     const Parameter &param);

  static double BM25TF(double rawTF, double docLength, const Parameter &param) {
    return ((param.k1+1)*rawTF/(rawTF +  param.k1*(1-param.b + param.b*docLength/param.avgLen)));
  }

  static double RSJWeight(double r, double R, 
			  double n, double N) {
    return (log ((r+0.5)*(N-n-R+r+0.5)/((R-r+0.5)*(n-r+0.5))));
  }

};

#endif //_OKAPIMODEL_HPP


  


