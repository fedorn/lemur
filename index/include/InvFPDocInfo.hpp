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


#ifndef _INVFPDOCINFO_HPP
#define _INVFPDOCINFO_HPP

//! Example Class for push method of building an index

/*!

  This class contains document information for a particular term
  All methods are in reference to a particular term.  For information
  about a document in whole, see the class DocumentProps.hpp
*/

/*
 * NAME DATE - COMMENTS
 * tnt 03/2001 - created
 *
 *========================================================================*/

#include "DocInfoList.hpp"
#include "InvFPTypes.hpp"

class InvFPDocInfo: public DocInfo {
public: 
  friend class InvFPDocList;

  InvFPDocInfo() {};
  ~InvFPDocInfo() {}; 
  
  DOCID_T docID() { return id; };
  int termCount() { return count; };
  LOC_T* positions() {return pos; };
 
private:
  DOCID_T id;    // id of this doc
  int count; // count of this term in this doc
  LOC_T* pos;  // list of positions in this doc (size is count)
};

#endif
