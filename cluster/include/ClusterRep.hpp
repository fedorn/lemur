/*==========================================================================
 * Copyright (c) 2002 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/
/*
  Author: dmf 11/2002
 */

#ifndef _CLUSTERREP_HPP
#define _CLUSTERREP_HPP
#include "common_headers.hpp"
#include "FloatFreqVector.hpp"
#include "Index.hpp"
/// Generic vector space representation for a cluster or document.
class ClusterRep {
public:
  /// Empty rep
  ClusterRep(const Index &ind) : index(ind) {
    rep = new FloatFreqVector();
  }
  /// Rep for given document id.
  ClusterRep(DOCID_T did, const Index &ind);
  /// Rep for given TermInfoList
  ClusterRep(TermInfoList *tList, const Index &ind);
  /// Rep for given vector of document ids
  ClusterRep(vector<DOCID_T> &dids, const Index &ind);
  /// Rep for the given vector
  ClusterRep(FloatFreqVector *v, const Index &ind);
  /// Copy constructor.
  ClusterRep(const ClusterRep *old);
  /// clean up.
  virtual ~ClusterRep();  
  /// divide each element by the square root of the sum of the 
  /// squared elements in the vector.
  virtual void normalize();
  /// sum of the squared elements in the vector.
  double sum2() const {return rep->sum2();}
  /// This should not be exposed...
  FloatFreqVector *getRep() const {return rep;}
  /// multiply each element by the corresponding entry in vals. 
  /// Destructively modifies the vector
  void weigh(const double *vals){rep->weigh(vals);} 
  /// multiply each element by val.
  /// Destructively modifies the vector
  void weigh(double val){rep->weigh(val);} 
  /// add val to each element in the vector.
  /// Destructively modifies the vector
  void addVal(int id, double val) {rep->addVal(id, val);}  
  /// add 1 to each element in the vector specified in tids.
  /// Destructively modifies the vector
  void addTerms(const vector<TERMID_T> &tids) {
    for (int i = 0; i < tids.size(); i++) {
      addVal(tids[i], 1);
    }
  }
  /// adds vector to rep.
  /// Destructively modifies the vector
  void addVec(const ClusterRep *v2){rep->addVec(v2->rep);}
  /// subtracts vector from rep.
  /// Destructively modifies the vector
  void subtract(const ClusterRep *v2){rep->subtract(v2->rep);}
  /// computes dot product between two reps.
  double dotProd(const ClusterRep *v2) const {return rep->dotProd(v2->rep);}

private:
  FloatFreqVector *rep;
  const Index &index;
};

#endif /* _CLUSTERREP_HPP */
