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

/// A basic implementation of IndexWithCat based on two Index's
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
  bool open(const char *) {} 

  /// @name Spelling and index conversion
  //@{

  /// Convert a term spelling to a termID
  virtual int term (const char * word) { return baseIndex->term(word);}

  /// Convert a termID to its spelling
  virtual const char * term (int termID) { return baseIndex->term(termID);}

  /// Convert a spelling to docID
  virtual int document (const char * docIDStr) { return baseIndex->document(docIDStr);}

  /// Convert a docID to its spelling
  virtual const char * document (int docID) { return baseIndex->document(docID);}

  /// Convert a spelling to catID
  virtual int category (const char * catIDStr);

  /// Convert a catID to its spelling
  virtual const char * category (int catID);

  //@}

  /// @name Summary counts
  //@{

  /// Total count (i.e., number) of documents in collection
  virtual int docCount () { return baseIndex->docCount();}

  /// Total count of unique terms in collection
  virtual int termCountUnique () { return baseIndex->termCountUnique();}

  /// Total counts of a term in collection
  virtual int termCount (int termID) const { return baseIndex->termCount(termID); }

  /// Total counts of all terms in collection
  virtual int termCount () const { return baseIndex->termCount(); }

  /// Average document length 
  virtual float docLengthAvg() { return baseIndex->docLengthAvg(); }

  /// Total counts of doc with a given term
  virtual int docCount(int termID) { return baseIndex->docCount(termID);}

  /// Total counts of terms in a document  
  virtual int docLength (int docID) const {return baseIndex->docLength(docID); }

  /// Total counts of categories
  virtual int catCount();

  /// Total counts of documents in a category
  virtual int docCountInCat(int catID);

  //@}

  /// @name Base Index entry access (Term X Doc)
  //@{
  /// doc entries in a term index, @see DocList
  virtual DocInfoList *docInfoList(int termID) { return baseIndex->docInfoList(termID); }

  /// word entries in a document index, @see TermList
  virtual TermInfoList *termInfoList(int docID) { return baseIndex->termInfoList(docID); }

  //@}

  /// @name Category Index entry access (Cat X Doc)
  //@{


  virtual void startCatIDIteration(int docID);
  virtual bool hasMoreCatID();
  /// Iteration over all category ids that a doc belongs to
  /*! 
    startCatIDIteration, hasMoreCatID, and nextCatID go together
    to support iteration over category ids that a document has
  */
  virtual int nextCatID();


  virtual void startDocIDIteration(int catID);
  virtual bool hasMoreDocID();

  /// Iteration over all doc ids that belong to a given category
  /*! 
    startDocIDIteration, hasMoreDocID, and nextDocID go together
    to support iteration over all doc ids in a given category
  */
  virtual int nextDocID();

  //@}



private:
  /// base index (word X doc)
  Index *baseIndex; 
  /// label/category index (cat X doc)
  Index *catIndex; 
  bool catAsTerm;

  /// buffers for iteration support
  TermInfoList *tmInfoBuffer;
  DocInfoList *docInfoBuffer;
  TermInfo *tmInfo;
  DocInfo *docInfo;

  /// synchronized doc id lexicons
  int *base2cat;
  int *cat2base;
 
  static const char OOVSTRING[];
};

inline int BasicIndexWithCat::category (const char * catIDStr)
{
  if (catAsTerm) {
    return (catIndex->term(catIDStr));
  } else {
    // docid as term, so cat as doc
    return (catIndex->document(catIDStr));
  }
}


inline const char *BasicIndexWithCat::category (int catID)
{
  if (catAsTerm) {
    return (catIndex->term(catID));
  } else {
    // docid as term, so cat as doc
    return (catIndex->document(catID));
  }
}



inline int BasicIndexWithCat::catCount()
{
  if (catAsTerm) {
    return (catIndex->termCountUnique()-1); // -1 because of [OOV]
  } else {
    // docid as term, so cat as doc
    return (catIndex->docCount());
  }
}

inline int BasicIndexWithCat::docCountInCat(int catID)
{
  if (catAsTerm) {
    return (catIndex->docCount(catID));
  } else {
    // docid as term, so cat as doc
    return (catIndex->docLength(catID));
  }
}



#endif /* _BASICINDEXWITHLABEL_HPP */
