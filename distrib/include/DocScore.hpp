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


#ifndef _DOCSCORE_HPP
#define _DOCSCORE_HPP

/// A list of docids and scores

/*!

 CLASSES: DocScore, DocScoreVector

*/


#include "common_headers.hpp"
#include <algorithm>
#include <string>
struct DocScore {
  double val;
  string id;
};


class DocScoreVector : public vector<DocScore> {
public:
  
  DocScoreVector() : vector<DocScore>() {}
  DocScoreVector(int size) : vector<DocScore>(size) {}
  virtual ~DocScoreVector();

  /// push a value 
  virtual void PushValue(const string &docid, double value);

  /// sort all the values, default is descending order
  virtual void Sort(bool descending = true);
private:

  /// Function object types, 
  /// defines how to compare IndexedReal objects (for sorting/searching)

  class ScoreAscending {
  public:
    bool operator()(const DocScore & a, const DocScore & b) {
      return a.val < b.val;  // based on the real value
    }
  };

  class ScoreDescending { 
  public: 
    bool operator()(const DocScore & a, const DocScore & b) {
      return a.val > b.val;  // based on the real value
    }
  };

  static ScoreAscending ascendOrder;
  static ScoreDescending descendOrder;

};



#endif 


  


