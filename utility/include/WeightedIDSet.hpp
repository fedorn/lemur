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



















