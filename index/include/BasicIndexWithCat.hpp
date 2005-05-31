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


#ifndef _BASICINDEXWITHCAT_HPP
#define _BASICINDEXWITHCAT_HPP

#include "IndexWithCat.hpp"

namespace lemur
{
  /// Document index components.
  namespace index
  {
    

    /*!

    BasicIndexWithCat is a wrapper that manages two separate Index's: one for word X doc index
    and one for category X doc index. It synchronizes
    the document index in both indices. 

    */

    class BasicIndexWithCat : public IndexWithCat {
    public:
      /// opens the indices. catIsTerm indicates whether a category is actually indexed as a "term" or a "document" in categoryIndex. 
      BasicIndexWithCat(Index &wordIndex, Index &categoryIndex, bool catIsTerm= true);

  
      ~BasicIndexWithCat() { delete [] base2cat; delete [] cat2base ;};

      /// to be implemented later
      bool open(const  string &name) {return false;} 

      /// @name Spelling and index conversion
      //@{

      /// Convert a term spelling to a termID
      virtual lemur::api::TERMID_T term (const string &word) const { return baseIndex->term(word);}

      /// Convert a termID to its spelling
      virtual const string term (lemur::api::TERMID_T termID) const { return baseIndex->term(termID);}

      /// Convert a spelling to docID
      virtual lemur::api::DOCID_T document (const string &docIDStr) const { return baseIndex->document(docIDStr);}

      /// Convert a docID to its spelling
      virtual const string document (lemur::api::DOCID_T docID) const { return baseIndex->document(docID);}

      /// Convert a spelling to catID
      virtual int category (const string &catIDStr) const;

      /// Convert a catID to its spelling
      virtual const string category (int catID) const;

      //@}

      /// @name Summary counts
      //@{

      /// Total count (i.e., number) of documents in collection
      virtual lemur::api::COUNT_T docCount () const { return baseIndex->docCount();}

      /// Total count of unique terms in collection
      virtual lemur::api::COUNT_T termCountUnique () const { return baseIndex->termCountUnique();}

      /// Total counts of a term in collection
      virtual lemur::api::COUNT_T termCount (lemur::api::TERMID_T termID) const { return baseIndex->termCount(termID); }

      /// Total counts of all terms in collection
      virtual lemur::api::COUNT_T termCount () const { return baseIndex->termCount(); }

      /// Average document length 
      virtual float docLengthAvg() const { return baseIndex->docLengthAvg(); }

      /// Total counts of doc with a given term
      virtual lemur::api::COUNT_T docCount(lemur::api::TERMID_T termID) const { return baseIndex->docCount(termID);}

      /// Total counts of terms in a document  
      virtual lemur::api::COUNT_T docLength (lemur::api::DOCID_T docID) const {return baseIndex->docLength(docID); }

      /// Total counts of categories
      virtual int catCount() const;

      /// Total counts of documents in a category
      virtual int docCountInCat(int catID) const;

      //@}

      /// @name Base Index entry access (Term X Doc)
      //@{
      /// doc entries in a term index, @see DocList
      virtual lemur::api::DocInfoList *docInfoList(lemur::api::TERMID_T termID) const { return baseIndex->docInfoList(termID); }

      /// word entries in a document index, @see TermList
      virtual lemur::api::TermInfoList *termInfoList(lemur::api::DOCID_T docID) const { return baseIndex->termInfoList(docID); }

      //@}

      /// @name Category Index entry access (Cat X Doc)
      //@{


      virtual void startCatIDIteration(int docID) const;
      virtual bool hasMoreCatID() const;
      /// Iteration over all category ids that a doc belongs to
      /*! 
        startCatIDIteration, hasMoreCatID, and nextCatID go together
        to support iteration over category ids that a document has
      */
      virtual int nextCatID() const;


      virtual void startDocIDIteration(int catID) const;
      virtual bool hasMoreDocID() const;

      /// Iteration over all doc ids that belong to a given category
      /*! 
        startDocIDIteration, hasMoreDocID, and nextDocID go together
        to support iteration over all doc ids in a given category
      */
      virtual int nextDocID() const;

      //@}



    private:
      /// base index (word X doc)
      lemur::api::Index *baseIndex; 
      /// label/category index (cat X doc)
      lemur::api::Index *catIndex; 
      bool catAsTerm;

      /// buffers for iteration support
      mutable lemur::api::TermInfoList *tmInfoBuffer;
      mutable lemur::api::DocInfoList *docInfoBuffer;
      mutable lemur::api::TermInfo *tmInfo;
      mutable lemur::api::DocInfo *docInfo;

      /// synchronized doc id lexicons
      int *base2cat;
      int *cat2base;
 
      static const string OOVSTRING;  
    };

    inline int BasicIndexWithCat::category (const string &catIDStr) const
    {
      if (catAsTerm) {
        return (catIndex->term(catIDStr));
      } else {
        // docid as term, so cat as doc
        return (catIndex->document(catIDStr));
      }
    }


    inline const string BasicIndexWithCat::category (int catID) const
    {
      if (catAsTerm) {
        return (catIndex->term(catID));
      } else {
        // docid as term, so cat as doc
        return (catIndex->document(catID));
      }
    }



    inline int BasicIndexWithCat::catCount() const
    {
      if (catAsTerm) {
        return (catIndex->termCountUnique());
      } else {
        // docid as term, so cat as doc
        return (catIndex->docCount());
      }
    }

    inline int BasicIndexWithCat::docCountInCat(int catID) const
    {
      if (catAsTerm) {
        return (catIndex->docCount(catID));
      } else {
        // docid as term, so cat as doc
        return (catIndex->docLength(catID));
      }
    }
  }
}



#endif /* _BASICINDEXWITHLABEL_HPP */
