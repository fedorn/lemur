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


#ifndef _WEIGHTEDIDSET_HPP
#define _WEIGHTEDIDSET_HPP

/// A set of ID's with weights
class WeightedIDSet {
public:
  virtual ~WeightedIDSet() {}
  virtual void startIteration() const= 0;
  virtual bool hasMore() const= 0;
  /// returns an ID and its weight
  virtual void nextIDInfo(int &id, double &weight) const= 0;
};

#endif /* _WEIGHTEDIDSET_HPP */



















