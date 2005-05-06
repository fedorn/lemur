/*==========================================================================
 * Copyright (c) 2003 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

#include "MultiRegrMergeMethod.hpp"
#include <map>

//typedef map<char *, double, ltstr> docsidmap;
typedef map<string, double, less<string> > docsidmap;

using namespace lemur::api;

lemur::distrib::MultiRegrMergeMethod::MultiRegrMergeMethod(double constA, double constB) {
  defParamA = constA;
  defParamB = constB;
}

void lemur::distrib::MultiRegrMergeMethod::mergeScoreSet(const IndexedRealVector &indexset, 
                                                         const DocScoreVector* const* scoreset,
                                                         DocScoreVector &results) {

  const DocScoreVector* docscores;
  DocScoreVector::const_iterator j;
  double indexscore;
  

  for (int i=0;i<indexset.size();i++) {
    docscores = scoreset[i];
    indexscore = indexset[i].val;

    double docMax=-1e10;//the max doc score of this database;
    double docMin=1e10;//the min doc score of this database;

    for (j=docscores->begin(); j!= docscores->end();j++) {
      if ((*j).val>docMax)
        docMax=(*j).val;
      if ((*j).val<docMin)
        docMin=(*j).val;
    }
    
    if (paramb[i]!=-1){
      for (j=docscores->begin(); j!= docscores->end();j++) {
        double scoreVal=score(indexscore, ((*j).val-docMin)/(docMax-docMin),i);
        results.PushValue((*j).id,scoreVal);
      }
    }
    else{
      for (j=docscores->begin(); j!= docscores->end();j++) {
        double scoreVal=score(indexscore, ((*j).val-docMin)/(docMax-docMin));
        results.PushValue((*j).id,scoreVal);
      }
    }
  }
  results.Sort();
}


lemur::distrib::MultiRegrMergeMethod::~MultiRegrMergeMethod() {
}

/// Calculate the final comparable document score
double lemur::distrib::MultiRegrMergeMethod::score(double dbscore, 
                                                   double docscore,int dbIndex) const {
  return parama[dbIndex]+ paramb[dbIndex]*docscore;
}

//only a dummy implementation of the virtual code
double lemur::distrib::MultiRegrMergeMethod::score(double dbscore, double docscore) const {
  return ( defParamA*docscore + defParamB*dbscore*docscore);
}

/// calculate the a regression model for each of the databases
/// indexset are the database scores for selected databases
/// centralsocres are the central documents scores retrieved by centralized sampling database
/// scoresset are the distributed documents scores retrieved by individual databases
void lemur::distrib::MultiRegrMergeMethod::calcRegrParams(const IndexedRealVector &indexset, 
                                                          const DocScoreVector* centralscores,
                                                          const DocScoreVector*const * scoresset) {
  const DocScoreVector* docscores;
  docsidmap centerScoreHash;
  double indexscore;
  int i,maxArraySize=0;
        


  int NUM_DB=indexset.size();
  parama=new double[NUM_DB];
  paramb=new double[NUM_DB];

  for (i=0;i<indexset.size();i++) {
    docscores = scoresset[i];
    if (maxArraySize<docscores->size())
      maxArraySize=docscores->size();
  }

  //put all the centralized scores into a hash map
  DocScoreVector::const_iterator j;
  for (j=centralscores->begin(); j!= centralscores->end();j++) {
    if ((*j).val!=0){
      //      centerScoreHash[(char *)(*j).id]=(*j).val;
      centerScoreHash[(*j).id]=(*j).val;
    }
  }

  /// the following arrays are created to calculate the linear regression parameters.
  /// here we wants to figure out the parameters of D''=a*D'+b;
  /// Yi=a*X+b

  int lessDbNum=0; //the number of databases that has less overlap documents than the threshold.
  for (i=0;i<NUM_DB;i++){
    double sum_x_x=0;
    double sum_x_y=0;
    double sum_x=0;
    double sum_y=0;
    double aVal=0;//a Parameter
    double bVal=0;//b Parameter
 
    int dbOverlapNum=0; //the number of overlap documents for this specific database
    docscores = scoresset[i];
    indexscore = indexset[i].val;
    DocScoreVector::const_iterator j;
    double docMax=-1e10;//the max doc score of this database;
    double docMin=1e10;//the min doc score of this database;

    for (j=docscores->begin(); j!= docscores->end();j++) {
      if ((*j).val>docMax)
        docMax=(*j).val;
      if ((*j).val<docMin)
        docMin=(*j).val;
    }

    for (j=docscores->begin(); j!= docscores->end();j++) {
      //      char* docId=(char *)(*j).id;
      const string &docId=(*j).id;
      if  ((centerScoreHash[docId]!=0) && ((*j).val!=0) && (dbOverlapNum<MGETTOPDOCSNUM)){
        double y= centerScoreHash[docId];
        double x=(*j).val;
        if (docMax>docMin)
          x=(x-docMin)/(docMax-docMin);
        else
          x=1;
        sum_x+=x;
        sum_y+=y;
        sum_x_x+=x*x;
        sum_x_y+=x*y;
        dbOverlapNum++;
      }
    }

    if (dbOverlapNum<MINOVERLAPDOCNUM){
      lessDbNum++;
      parama[i]=0;
      paramb[i]=0;
    }else{
      sum_x_x=sum_x_x-(double)1/dbOverlapNum*(sum_x*sum_x);
      sum_x_y=sum_x_y-(double)1/dbOverlapNum*(sum_x*sum_y);
      if (sum_x_x!=0)
        bVal=sum_x_y/sum_x_x;
      else{
        parama[i]=0;
        paramb[i]=(double)sum_y/dbOverlapNum;
        break;
      }
      aVal=(double)1/dbOverlapNum*sum_y-(double)1/dbOverlapNum*sum_x*bVal;
      if (bVal<0){ //if the behavior is rather weird, high doc means less relevant,
        lessDbNum++;
        aVal=0;
        bVal=0;
      }
      else{
        if ((aVal+bVal)>1){
          if ((1.5-0.5*bVal-1.5*aVal)>0)
            bVal=1.5-0.5*bVal-1.5*aVal;
          else
            bVal=1-aVal;
          aVal=1-bVal;
        }
      }
      parama[i]=aVal;
      paramb[i]=bVal;
    }
  }
 
  if (((double)lessDbNum/(double)NUM_DB)>MINLESSDBPERCEN){
    //we do not have enough databases that have enough overlap documents, back off to the CORI merging algorithm
    for (int i=0;i<NUM_DB;i++){
      parama[i]=-1;
      paramb[i]=-1;
    }
  }
}

