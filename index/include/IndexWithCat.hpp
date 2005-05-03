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


#ifndef _INDEXWITHCAT_HPP
#define _INDEXWITHCAT_HPP

#include "Index.hpp"
namespace lemur 
{
  namespace index 
  {
    
    /// An abstract interface for access to an index with category information

    /*!

    IndexWithCat extends Index with document category information. In addition
    to what is supported by Index, it adds support of looking up all categories for a document and looking up all documents for a category. It is typically used by a text categorization application.

    */

    class IndexWithCat : public lemur::api::Index {
    public:

      /// @name Extra spelling and index conversion that are not in Index
      //@{

      /// Convert a spelling to catID
      virtual int category (const string &catIDStr) const=0;

      /// Convert a catID to its spelling
      virtual const string category (int catID) const=0;

      //@}

      /// @name Extra summary counts not in Index
      //@{

      /// Total counts of categories
      virtual int catCount() const=0;

      /// Total counts of documents in a category
      virtual int docCountInCat(int catID) const=0;

      //@}


      /// @name Category Index entry access (Cat X Doc)
      //@{


      virtual void startCatIDIteration(int docID) const=0;
      virtual bool hasMoreCatID() const=0;
      /// Iteration over all category ids that a doc belongs to
      /*! 
        startCatIDIteration, hasMoreCatID, and nextCatID go together
        to support iteration over category ids that a document has.
      */
      virtual int nextCatID() const=0;


      virtual void startDocIDIteration(int catID) const=0;
      virtual bool hasMoreDocID() const=0;

      /// Iteration over all doc ids that belong to a given category
      /*! 
        startDocIDIteration, hasMoreDocID, and nextDocID go together
        to support iteration over all doc ids in a given category
      */
      virtual int nextDocID() const=0;

      //@}

    };
  }
}

#endif /* _INDEXWITHCAT_HPP */


