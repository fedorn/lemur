/*==========================================================================
 * Copyright (c) 2004 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/


//
// ScoredExtentResult
//
// 8 March 2004 -- tds
//

#ifndef INDRI_SCOREDEXTENTRESULT_HPP
#define INDRI_SCOREDEXTENTRESULT_HPP

#include "lemur-platform.h"

struct ScoredExtentResult {
  struct score_greater {
  public:
    bool operator() ( const ScoredExtentResult& one, const ScoredExtentResult& two ) const {
      return one.score > two.score;
    }
  };

  ScoredExtentResult() :
    score(0),
    document(0),
    begin(0),
    end(0)
  {
  }

  ScoredExtentResult( UINT64 s, int d )
    :
    score( double(s) ),
    document(d),
    begin(0),
    end(0)
  {
  }

  ScoredExtentResult( double s, int d )
    :
    score(s),
    document(d),
    begin(0),
    end(0)
  {
  }

  ScoredExtentResult( double s, int d, int b, int e )
    :
    score(s),
    document(d),
    begin(b),
    end(e)
  {
  }

  ScoredExtentResult( const ScoredExtentResult& other ) {
    score = other.score;
    document = other.document;
    begin = other.begin;
    end = other.end;
  }

  bool operator< ( const ScoredExtentResult& other ) const {
    return score > other.score;
  }

  double score;
  int document;
  int begin;
  int end;
};

#endif // INDRI_SCOREDEXTENTRESULT_HPP

