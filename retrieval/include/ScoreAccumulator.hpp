/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/


#ifndef _SCOREACCUMULATOR_HPP
#define _SCOREACCUMULATOR_HPP


//------------------------------------------------------------
//      Abstract Interface for A Score Accumulator
//------------------------------------------------------------

/// Abstract Score Accumulator
class ScoreAccumulator {
public:
  /// clear, reset
  virtual void reset()=0;
  /// Find the score value for an id
  virtual bool findScore(int id, double &score)=0;
  /// Set the score value of an id, add the id if it doesn't exist
  virtual void setScore(int id, double score)=0;
  /// Increase the score value of an id, add the id if it doesn't exist
  virtual void incScore(int id, double score)=0;
  
  /// Iteration support
  virtual void startIteration()=0;
  virtual bool hasMore() = 0;
  virtual void nextScore(int &id, double &score) =0;

};


//------------------------------------------------------------
//      An Array Score Accumulator
//------------------------------------------------------------

/// Array Score Accumulator
class ArrayAccumulator : public ScoreAccumulator {
public:
  ArrayAccumulator(int maxID);

  virtual ~ArrayAccumulator() { delete [] acc; delete [] status;}
  
  virtual void reset(){
    for (int i=0; i<sz; i++) {
      acc[i]=0;
      status[i]=0;
    }
    p=0;
  }
  
  /// Find the score value for an id
  virtual bool findScore(int id, double &score);

  /// Set the score value of an id, add the id if it doesn't exist
  virtual void setScore(int id, double score) { 
    acc[id-1] = score;
    status[id-1] =1;
  }

  /// Increase the score value of an id, add the id if it doesn't exist
  virtual void incScore(int id, double score) {
    acc[id-1] += score;
    status[id-1] =1;
  }
  
  /// Iteration support
  virtual void startIteration() { p = 0; }
  bool hasMore();
  
  void nextScore(int &id, double &score); 

protected:
  int p;
  int sz;
  double *acc;
  short *status;
};


// ============== inlines ==============================

inline ArrayAccumulator::ArrayAccumulator(int maxID) : sz(maxID) 
{ 
  acc = new double[maxID]; // index 0 refers to docid 1.
  status = new short[maxID]; // using integer making the test faster
}

inline bool ArrayAccumulator::findScore(int id, double &score)
{ 
  if (status[id-1]>0) {
    score = acc[id-1];
    return true;
  } else {
    return false;
  }
}

inline bool ArrayAccumulator::hasMore() {
  assert (p>=0 && p<=sz);
  while (p<sz && status[p]==0) p++;
  return (p<sz);
}

inline  void ArrayAccumulator::nextScore(int &id, double &score) {
  if (p<sz) id = p+1;
  score = acc[p];
  p++;
}
#endif /* _SCOREACCUMULATOR_HPP */








