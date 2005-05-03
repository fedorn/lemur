/*==========================================================================
 * Copyright (c) 2002 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */
/*
  Author: dmf 11/2002
*/

#ifndef _THRESHOLDFCN_HPP
#define _THRESHOLDFCN_HPP
namespace lemur 
{
  namespace cluster 
  {
    
    /// Generic threshold function.
    class ThresholdFcn{
    public:
      ThresholdFcn(double val) : thresh(val) {}
      virtual ~ThresholdFcn() {}
      virtual double threshold(int size = 0);
      virtual bool threshold(double score, int size = 0);
      double thresh;
    };
  }
}

#endif /* _THRESHOLDFCN_HPP */
