/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */


#ifndef _SCOREACCUMULATOR_HPP
#define _SCOREACCUMULATOR_HPP


namespace lemur 
{
  namespace api 
  {
    
    //------------------------------------------------------------
    //      Abstract Interface for A Score Accumulator
    //------------------------------------------------------------

    /// Abstract Score Accumulator
    class ScoreAccumulator {
    public:
      virtual ~ScoreAccumulator(){}
      
      /// clear, reset
      virtual void reset()=0;
      /// Find the score value for an id
      virtual bool findScore(int id, double &score)const =0;
      /// Set the score value of an id, add the id if it doesn't exist
      virtual void setScore(int id, double score)=0;
      /// Increase the score value of an id, add the id if it doesn't exist
      virtual void incScore(int id, double score)=0;
  
      /// Iteration support
      virtual void startIteration()const =0;
      virtual bool hasMore() const = 0;
      virtual void nextScore(int &id, double &score) const =0;

    };
  }
}

namespace lemur 
{
  namespace retrieval 
  {
    
    //------------------------------------------------------------
    //      An Array Score Accumulator
    //------------------------------------------------------------

    /// Array Score Accumulator
    class ArrayAccumulator : public lemur::api::ScoreAccumulator {
    public:
      ArrayAccumulator(int maxID);

      virtual ~ArrayAccumulator() { delete [] acc; delete [] status;}

      /// Reset all entries to zero
      virtual void reset();
  
      /// Find the score value for an id
      virtual bool findScore(int id, double &score) const ;

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
      virtual void startIteration() const { p = 0; }
      bool hasMore()const ;
  
      void nextScore(int &id, double &score)const ; 

    protected:
      mutable int p;
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
  }
}


#endif /* _SCOREACCUMULATOR_HPP */
