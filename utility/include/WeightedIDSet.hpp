 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * 
 * The Lemur toolkit for language modeling and information retrieval.
 * Copyright (c) 2001 Carnegie Mellon University.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted for research or educational purposes,
 * provided that this copyright notice is maintained and note is made
 * of any changes to the source code.
 * 
 * This is a research system.  The code is distributed on an "as is" basis,
 * without any warranty, express or implied. 
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef _WEIGHTEDIDSET_HPP
#define _WEIGHTEDIDSET_HPP

/// A set of ID's with weights
class WeightedIDSet {
public:
  virtual void startIteration() = 0;
  virtual bool hasMore() = 0;
  /// returns an ID and its weight
  virtual void nextIDInfo(int &id, double &weight) = 0;
};

#endif /* _WEIGHTEDIDSET_HPP */



















