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


#ifndef _INVFPINDEX_HPP
#define _INVFPINDEX_HPP

///  Class for retrieving indexed document collection based on the abstract class Index.hpp
#include "common_headers.hpp"
#include "InvIndex.hpp"
#include "InvFPDocList.hpp"
#include "InvFPTermList.hpp"
#include "InvFPTypes.hpp"


class InvFPIndex : public InvIndex {
public:
   InvFPIndex();
   InvFPIndex(const string &indexName);
  ~InvFPIndex(); 

  /// @name Index entry access
  //@{
  /// doc entries in a term index, @see DocList @see InvFPDocList
  DocInfoList* docInfoList(TERMID_T termID) const;

  /// word entries in a document index (bag of words), @see TermList
  TermInfoList* termInfoList(DOCID_T docID) const;

  /// word entries in a document index (sequence of words), @see TermList
  TermInfoList* termInfoListSeq(DOCID_T docID) const;

  /// Total count of terms in given document, not including stop words
  COUNT_T docLengthCounted(DOCID_T docID) const;

  //@}
private:

};

#endif
