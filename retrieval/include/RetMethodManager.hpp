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

#ifndef _RETMETHODMANAGER_HPP
#define _RETMETHODMANAGER_HPP

#include "RetParamManager.hpp"

class RetMethodManager {
public:
  enum RetModel {TFIDF=0, OKAPI=1, KL=2, CORI_DOC=3,CORI_CS=4,COS=5};

  /// use specified default if no model is specified in parameters
  static RetrievalMethod* createModel(Index* ind, ArrayAccumulator* accum, RetModel def=KL);

};

#endif
