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
   InvFPIndex(const char* indexName);
  ~InvFPIndex(); 

  /// Open previously created Index using names in given param file, return true if opened successfully
  bool openName(char* filename);


  /// @name Index entry access
  //@{
  /// doc entries in a term index, @see DocList @see InvFPDocList
  DocInfoList* docInfoList(int termID);

  /// word entries in a document index (bag of words), @see TermList
  TermInfoList* termInfoList(int docID);

  /// word entries in a document index (sequence of words), @see TermList
  TermInfoList* termInfoListSeq(int docID);

  //@}
private:

};

#endif
