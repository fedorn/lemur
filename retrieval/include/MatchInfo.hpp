/*==========================================================================
 * Copyright (c) 2002-2003 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */
/*
 * author: dmf
 * 
 */
#ifndef _MATCHINFO_HPP_
#define _MATCHINFO_HPP_
#include <vector>
#include <algorithm>

#include "RetrievalMethod.hpp"
namespace lemur 
{
  namespace api 
  {
    
    /// Term match entry
    class TMatch {
    public:
		TMatch() : tid(-1), start(0), end(0), position(0)  {};
      TMatch(TERMID_T t, int s, int e, int p): tid(t), start(s), end(e), 
                                               position(p) { }
      /// internal term id
      TERMID_T tid;
      /// byte offset of first letter of term in raw document
      int start;
      /// byte offset of last letter of term in raw document
      int end;
      /// token number within document (matches InvFP position).
      int position;
    };

    /// \brief Match information for a query within a document. 
    /*!  Provides a list of match offsets (token based). Also provides
     byte offsets in source text when a DocumentManager is available to
     provide the raw document text.  If the DocumentManager is a
     KeyfileDocMgr, the raw document text is not parsed, the offsets are
     retrieved directly.
    */
    class MatchInfo : public vector<TMatch> {
    public: 
      /// clean up.
      virtual ~MatchInfo() {}

      /// number of matches
      int count() const {return size();}

      /// Iteration support (restartable).
      // void startIteration() const {iter = matchList.begin();}
      /// have any more?
      //bool hasMore() const {return(iter != matchList.end());}

      /// Advance to next document, return false if no more docs.
      //TMatch nextMatch() const {TMatch ret = *iter; iter++; return ret;}

      /// Construct the match list for a given query/document pair.
      /// Query would be better replaced by the QueryDocument class
      /// or some list of internal term ids derived from the input
      /// query.
      static MatchInfo *getMatches(const Index &ind, const Query &qry, DOCID_T docID);

    private:
      /// Make an empty one.
      MatchInfo() {}

      /// add an entry
      void add(TERMID_T tid, int position, int start=-1, int end=-1) {
        TMatch t(tid, start, end, position);
        //matchList.push_back(t);
        push_back(t);
      }

      /// For sorting TMatch vectors
      class TMatchAscending {
      public:
        bool operator()(const TMatch & a, const TMatch & b) {
          return a.position < b.position;
        }
      };

      /// sort the list
      void sort() {
        TMatchAscending tma;
        //    std::sort(matchList.begin(), matchList.end(), tma);
        std::sort(this->begin(), this->end(), tma);
      }

      /// list of matches in document
      //  vector<TMatch> matchList;
      /// iterator for list of matches in document
      //mutable vector<TMatch>::iterator iter;
    };
  }
}

#endif
