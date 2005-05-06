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

#ifndef _RETMETHODMANAGER_HPP
#define _RETMETHODMANAGER_HPP

#include "RetParamManager.hpp"
#include "TextHandlerManager.hpp"
#include "InQueryOpParser.hpp"
#include "StringQuery.hpp"

namespace lemur 
{
  namespace api
  {
    /// Factory class for creating RetrievalMethod objects.
    class RetMethodManager {
    public:
      enum RetModel {UNKNOWN=-1, TFIDF=0, OKAPI=1, KL=2, INQUERY=3,CORI_CS=4,COS=5, INQ_STRUCT=6, INDRI=7};

      /// use specified model or check in parameters
      static RetrievalMethod* createModel(const Index* ind, ScoreAccumulator* accum, RetModel type);

      /// use specified model or check in parameters
      static RetrievalMethod* createModel(const Index* ind, ScoreAccumulator* accum, string type = "");

      /// automatically generate retrieval and query models from the given query 
      /// stopword file and stemmer are optional
      /// specify model using string type name
      /// query terms delimited by space
      /// delete IndexedRealVector
      static IndexedRealVector* runQuery(const string& query, const Index* index, const string& modeltype, const string& stopfile="", const string& stemtype="", const string& datadir="", const string& func="");

      /// automatically generate query models from the given query and run through
      /// given retrieval model. stopword file and stemmer are optional
      /// query terms delimited by space
      /// delete IndexedRealVector returned
      static IndexedRealVector* runTextQuery(const string& query, TextQueryRetMethod* model, const string& stopfile="", const string& stemtype="", const string& datadir="", const string& func="");

      /// automatically generate query models from the given query and run through
      /// given retrieval model. stopword file and stemmer are optional
      /// query terms delimited by space
      /// delete IndexedRealVector returned
      static IndexedRealVector* runStructQuery(const string& query, StructQueryRetMethod* model, const string& stopfile="", const string& stemtype="", const string& datadir="", const string& func="");
    };
  }
}

#endif
