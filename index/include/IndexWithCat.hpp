 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * 
 * The Lemur toolkit for language modeling and information retrieval.
 * Copyright (c) 2001 Carnegie Mellon University.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted for research or educational purposes,
 * provided that this copyright notice is maintained and note is made
 * of any changes to the source code.
 * 
 * This is a research system.  The code is distributed on an "as is" basis,
 * without any warranty, express or implied. 
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef _INDEXWITHCAT_HPP
#define _INDEXWITHCAT_HPP

#include "Index.hpp"

/// An abstract interface for access to an index with category information

/*!

  IndexWithCat extends Index with document category information. In addition
 to what is supported by Index, it adds support of looking up all categories for a document and looking up all documents for a category. It is typically used by a text categorization application.

 */

class IndexWithCat : public Index {
public:

  /// @name Extra spelling and index conversion that are not in Index
  //@{

  /// Convert a spelling to catID
  virtual int category (const char * catIDStr)=0;

  /// Convert a catID to its spelling
  virtual const char * category (int catID)=0;

  //@}

  /// @name Extra summary counts not in Index
  //@{

  /// Total counts of categories
  virtual int catCount()=0;

  /// Total counts of documents in a category
  virtual int docCountInCat(int catID)=0;

  //@}


  /// @name Category Index entry access (Cat X Doc)
  //@{


  virtual void startCatIDIteration(int docID)=0;
  virtual bool hasMoreCatID()=0;
  /// Iteration over all category ids that a doc belongs to
  /*! 
    startCatIDIteration, hasMoreCatID, and nextCatID go together
    to support iteration over category ids that a document has.
  */
  virtual int nextCatID()=0;


  virtual void startDocIDIteration(int catID)=0;
  virtual bool hasMoreDocID()=0;

  /// Iteration over all doc ids that belong to a given category
  /*! 
    startDocIDIteration, hasMoreDocID, and nextDocID go together
    to support iteration over all doc ids in a given category
  */
  virtual int nextDocID()=0;

  //@}

};

#endif /* _INDEXWITHCAT_HPP */


