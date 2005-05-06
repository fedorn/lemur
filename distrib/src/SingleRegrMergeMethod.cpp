/*==========================================================================
 * Copyright (c) 2002 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

#include "SingleRegrMergeMethod.hpp"
#include <map>

//typedef map<char *, double, ltstr> docsidmap;
typedef map<string, double, less<string> > docsidmap;

using namespace lemur::api;

lemur::distrib::SingleRegrMergeMethod::SingleRegrMergeMethod(double constA, double constB) {
  parama = constA;
  paramb = constB;
}

lemur::distrib::SingleRegrMergeMethod::~SingleRegrMergeMethod() {
}

/// Calculate the final comparable document score
double lemur::distrib::SingleRegrMergeMethod::score(double dbscore, double docscore) const {
  return parama * docscore + paramb * dbscore * docscore;
}

/// calculate the single regression model for all the databases
/// indexset are the database scores for selected databases
/// centralsocres are the central documents scores retrieved by centralized sampling database
/// scoresset are the distributed documents scores retrieved by individual databases
void lemur::distrib::SingleRegrMergeMethod::calcRegrParams(const IndexedRealVector &indexset, 
                                                           const DocScoreVector* centralscores, 
                                                           const DocScoreVector* const* scoresset) {
  const DocScoreVector* docscores;
  DocScoreVector::const_iterator j;
  docsidmap centerScoreHash;
  double indexscore;
  int maxArraySize=0;
  int i;

  for (i=0;i<indexset.size();i++) {
    docscores = scoresset[i];
    maxArraySize+=docscores->size();
  }

  /// the following arrays are created to calculate the linear regression parameters.
  /// here we wants to figure out the parameters of D''=a*D'+b*C'*D';
  /// Yi=a*X1i+b*X2i
  double* yVals = new double[maxArraySize];
  double* x1Vals = new double[maxArraySize];
  double* dbVals = new double[maxArraySize];
  double* x2Vals = new double[maxArraySize];
  double sum_x1_x1=0;
  double sum_x2_x2=0;
  double sum_x1_x2=0;
  double sum_y_x1=0;
  double sum_y_x2=0;
  double aVal=0;//a Parameter
  double bVal=0;//b Parameter

  for (j=centralscores->begin(); j!= centralscores->end();j++) {
    if ((*j).val!=0){
      //put all the centralized scores into a hash map
      //      centerScoreHash[strdup((*j).id)]=(*j).val;
      centerScoreHash[(*j).id]=(*j).val;
    }
  }
 
  int overlapNum=0;//the total number of overlap documents for all databases.
  int lessDbNum=0; //the number of databases that has less overlap documents than the threshold.
  int dbCount=indexset.size();
  for (i=0;i<indexset.size();i++) {
    int dbOverlapNum=0; //the number of overlap documents for this specific database
    docscores = scoresset[i];
    indexscore = indexset[i].val;
    for (j=docscores->begin(); j!= docscores->end();j++) {
      const string &docId = (*j).id;
      if  ((centerScoreHash[docId]!=0) && ((*j).val!=0) && (dbOverlapNum<GETTOPDOCSNUM)){
        double y= centerScoreHash[docId];
        double x1=(*j).val;
        double x2=(*j).val*indexscore;
        yVals[overlapNum]=y;
        x1Vals[overlapNum]=x1;
        dbVals[overlapNum]=indexscore;
        x2Vals[overlapNum]=x2;
        sum_x1_x1+=x1*x1;
        sum_x2_x2+=x2*x2;
        sum_x1_x2+=x1*x2;
        sum_y_x1+=y*x1;
        sum_y_x2+=y*x2;
        dbOverlapNum++;
        overlapNum++;
      }
    }
    if (dbOverlapNum<MINOVERLAPDOCNUM){
      lessDbNum++;
    }
  }

  if (((double)lessDbNum/(double)dbCount)<=MINLESSDBPERCEN){
    //we have enough databases that have enough overlap documents
    //start to calculate the parameters
    bVal=sum_y_x2*sum_x1_x1-sum_y_x1*sum_x1_x2;
    bVal/=(sum_x1_x1*sum_x2_x2-sum_x1_x2*sum_x1_x2);
    aVal=sum_y_x1-bVal*sum_x1_x2;
    aVal/=sum_x1_x1;
    parama=aVal;
    paramb=bVal;
  }else{
    //we do not have enough databases that have enough overlap documents, back off to the CORI merging algorithm
    parama=0.71428;
    paramb=0.5714;
  }

  delete[] yVals;
  delete[] x1Vals;
  delete[] dbVals;
  delete[] x2Vals;
}

