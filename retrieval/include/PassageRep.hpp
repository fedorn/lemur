/*==========================================================================
 * Copyright (c) 2003-2004 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

#ifndef _PASSAGEREP_HPP
#define _PASSAGEREP_HPP

#include <vector>
#include "MatchInfo.hpp"

namespace lemur 
{
  namespace retrieval 
  {
    
    /// Score of a passage, with start and end indices
    struct PassageScore {
      /// passage number
      int id;
      /// first token index
      int start;
      /// index of last token + 1
      int end;
      /// the score
      double score;
    };

    /// Vector of passage scores, supports sorting.
    class PassageScoreVector : public vector<PassageScore> {
    public:
      PassageScoreVector() : vector<PassageScore>() {
      }
      /// sort the scores in descending order
      void sortScores() {
        sort(this->begin(), this->end(), cmpFn);
      }
    private:
      class PassageScoreDescending { 
      public: 
        bool operator()(const PassageScore & a, const PassageScore & b) {
          return a.score > b.score;
        }
      };
      static PassageScoreDescending cmpFn;
    };
    /// \brief Passage representation for a document. 
    /*! Supports iteration over passages
      of fixed window size with an overlap of K terms for the window. 
      Encapsulates the DocumentRep for the whole document, modifying its 
      docLength attribute. Delegates calls to termWeight and 
      scoreConstant to the encapsulated DocumentRep. TFIDFRetMethod with BM25 tf weighting and OkapiRetMethod will not compute correct scores, as they use the average document length from the collection in their formulas. The difference should be small.
    */
    class PassageRep : public lemur::api::DocumentRep {
    public:
      /// \brief Fixed sized window passage with overlap. 
      /// @param dRep DocumentRep for the document as returned by computeDocRep. 
      /// @param d length of whole document. 
      /// @param p size of passage in terms of tokens. 
      /// @param o number of tokens to overlap. 
      PassageRep(lemur::api::DocumentRep &dRep, int d, int p, int o) :
        lemur::api::DocumentRep(dRep.getID(), p),
        docRep(dRep), docEnd(d), psgSize(p), overlap(o) {
        // update encapsulated rep.
        docRep.setDocLength(docLength);
      }

#if 0
      PassageRep(): DocumentRep(0, 0), docRep(*this) { 
      }
#endif
      /// Replace with STL style iterator.
#if 0
      /// begin iterating over the passages of a document.
      void startPassageIteration() {
        start = 0;
        end = psgSize < docEnd ? psgSize : docEnd;
      }
      /// any passages left?
      bool hasMorePassage() {
        return(start < docEnd);
      }
      /// advance to next passage of a document, updating the docLength attribute of the encapsulated DocumentRep.
      void nextPassage() {
        int next = start + (end - overlap);
        if(next < docEnd)
          start = next;
        else
          start = docEnd;
        end = (start + psgSize) < docEnd ? (start + psgSize) : docEnd;
        docLength = end - start; // adjust for shorter last passage.
        // update encapsulated rep.
        docRep.setDocLength(docLength);
      }
#endif
      /// iterator over passages.
      class iterator {
      public:
        iterator() : start(0), end(0), psgSize(0), overlap(0), docEnd(0),
                     rep(NULL) {};
        iterator(int s, int e, int p, int o, int d, PassageRep *r) : start(s), 
                                                                     end(e), 
                                                                     psgSize(p), 
                                                                     overlap(o), 
                                                                     docEnd(d) {
          rep = new PassageRep(*r);
        };
        // risk of double deletes when copying?
        virtual ~iterator() { 
          delete(rep);
        };
        /// initialize and return the element.
        virtual PassageRep &operator*(){ return *rep;};

        virtual iterator& operator++(){
          int next = start + (end - overlap);
          if(next < docEnd)
            start = next;
          else
            start = docEnd;
          end = (start + psgSize) < docEnd ? (start + psgSize) : docEnd;
          int docLength = end - start; // adjust for shorter last passage.
          // update encapsulated rep. // fix this!
          rep->setEnd(start, end, docLength);
          return *this;
        };

        virtual iterator& operator++(int){  // ++foo semantics here?
          int next = start + (end - overlap);
          if(next < docEnd)
            start = next;
          else
            start = docEnd;
          end = (start + psgSize) < docEnd ? (start + psgSize) : docEnd;
          int docLength = end - start; // adjust for shorter last passage.
          // update encapsulated rep. // fix this!
          rep->setEnd(start, end, docLength);
          return *this;
        }; 
        /// pointer equality on the int * internal representations.
        virtual bool operator==(iterator& other)
        {
          return (other.start == start && other.end == end);
        };
        /// pointer inequality on the int * internal representations.
        virtual bool operator!=(iterator& other)
        {
          return !(other.start == start && other.end == end);
        };
      protected:
        /// the passage rep we're iterating over.
        PassageRep *rep;
        /// Size of the passage, in number of tokens.
        int psgSize;
        /// Number of tokens to overlap when advancing the passage window.
        int overlap;
        /// Length of the whole document.
        int docEnd;
        /// index of start of the current passage.
        int start;
        /// index of end of the current passage.
        int end;
      };
  
      // could trim start, end attributes.
      PassageRep::iterator begin() {
        start = 0;
        pEnd = psgSize < docEnd ? psgSize : docEnd;
        // need to keep the state in the iterator only.
        PassageRep::iterator retval(0, pEnd, psgSize, overlap, docEnd, this);
        return retval;
      }

      PassageRep::iterator end() {
        PassageRep::iterator retval (docEnd, docEnd, psgSize, overlap, docEnd, 
                                     this);
        return retval;
      }
      /// update end and length values
      void setEnd(int s, int e, int dl) {
        start = s;
        pEnd = e;
        docRep.setDocLength(dl);
      }
      /// \brief Term frequency of a term within the current passage.  
      /// @param tid the term id to count.   
      /// @param matches the term matches returned by MatchInfo::getMatches for
      /// the document. This list is used for efficiency, as it is shorter
      /// than the whole TermInfoList for the document.
      /// @return the frequency of a term within the current passage.
      int passageTF(lemur::api::TERMID_T tid, lemur::api::MatchInfo *matches) const {
        int tf = 0;
        int pos = 0;
        lemur::api::MatchInfo::iterator m = matches->begin();
        while (m != matches->end() && pos < pEnd) {
          lemur::api::TMatch match = *m;
          pos = match.position;
          // adjust for stopwords not counted. Bleah
          if (pos > docEnd) docEnd = pos + 1;
          if (match.tid == tid) {
            if (pos >= start && pos < pEnd) {
              tf++;
            }
          }
          m++;
        }
        return tf;
      }
      /// start of the current passage
      int getStart () const {return start;}
      /// end + 1 of the current passage
      int getEnd () const {return pEnd;}

      /// Delegate call to termWeight of the encapsulated DocumentRep
      virtual double termWeight(lemur::api::TERMID_T termID, const lemur::api::DocInfo *info) const {
        return docRep.termWeight(termID, info);
      }
  
      /// Delegate call to scoreConstant of the encapsulated DocumentRep.
      virtual double scoreConstant()  const {
        return docRep.scoreConstant();
      }
  

    protected:
      /// DocumentRep for the whole document. Calls to termWeight and scoreConstant are delegated to it.
      lemur::api::DocumentRep &docRep;
      /// Size of the passage, in number of tokens.
      int psgSize;
      /// Number of tokens to overlap when advancing the passage window.
      int overlap;
      /// Length of the whole document.
      mutable int docEnd;
      /// index of start of the current passage.
      mutable int start;
      /// index of end of the current passage.
      mutable int pEnd;
    };
  }
}

#endif /* _PASSAGEREP_HPP */



