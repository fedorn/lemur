/*==========================================================================
 *
 *  Original source copyright (c) 2000, Carnegie Mellon University.
 *  See copyright.cmu for details.
 *  Modifications copyright (c) 2002, University of Massachusetts.
 *  See copyright.umass for details.
 *
 *==========================================================================
 */

#include "IndexedReal.hpp"
#include <cmath>

lemur::api::IndexedRealVector::IndexedRealAscending lemur::api::IndexedRealVector::ascendOrder;
lemur::api::IndexedRealVector::IndexedRealDescending lemur::api::IndexedRealVector::descendOrder;

void lemur::api::IndexedRealVector::Sort(bool descending)
{
  if (descending) {
    stable_sort(this->begin(), this->end(), descendOrder);
  } else {
    stable_sort(this->begin(), this->end(), ascendOrder);
  }
}

void lemur::api::IndexedRealVector::NormalizeValues()
{
  iterator it = begin();
  double sum=0;
  while (it != end()) {
    sum+=(*it).val;
    it++;
  }
  it = begin();
  while (it != end()) {
    (*it).val/=sum;
    it++;
  }
}

void lemur::api::IndexedRealVector::LogToPosterior()
{
  // In:  log(x1) log(x2) ... log(xN) 
  // Out: x1/sum, x2/sum, ... xN/sum
  // 
  // Extra care is taken to make sure we don't overflow 
  // machine precision when taking exp (log x)
  // This is done by adding a constant K which cancels out
  // Right now K is set to maximally preserve the highest value
  // but could be altered to a min or average, or whatever...

  iterator it = begin();
  double sum=0;
  double K=(*it).val; 
  while (it != end()) { // find the max value;
    if((*it).val>K)
      K=(*it).val;
    it++;
  }
  K=-K;
  it = begin();
  while (it != end()) {
    sum+=(*it).val=exp(K+(*it).val);
    it++;
  }
  it = begin();
  while (it != end()) {
    (*it).val/=sum;
    it++;
  }
}


lemur::api::IndexedRealVector::iterator lemur::api::IndexedRealVector::FindByIndex(int index)
{
  iterator it = begin();
  while (it != end() && (*it).ind != index) {
    it++;
  }
  return  it;
}

bool lemur::api::IndexedRealVector::IncreaseValueFor(int index, double value)
{
  iterator it = FindByIndex(index);
  if (it == end()) {
    IndexedReal entry;
    entry.ind = index;
    entry.val = value;
    push_back(entry);
    return false;
  } else {
    (*it).val += value;
    return true;
  }
}


void lemur::api::IndexedRealVector::PushValue(int index, double value)
{
  IndexedReal entry;
  entry.ind = index;
  entry.val = value;
  push_back(entry);
}


