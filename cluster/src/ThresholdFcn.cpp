/*==========================================================================
 * Copyright (c) 2002 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/
/*
  Author: dmf 11/2002
 */

#include "ThresholdFcn.hpp"
double ThresholdFcn::threshold(int size) { return thresh; }
bool ThresholdFcn::threshold(double score, int size) { 
  return score >= thresh; 
}
