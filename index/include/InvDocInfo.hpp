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


#ifndef _INVDOCINFO_HPP
#define _INVDOCINFO_HPP

//! Example Class for push method of building an index

/*!

  This class contains document information for a particular term
  All methods are in reference to a particular term.  For information
  about a document in whole, see the class DocumentProps.hpp
*/

/*
 * NAME DATE - COMMENTS
 * tnt 03/2002 - created
 *
 *========================================================================*/

#include "DocInfoList.hpp"
#include "InvFPTypes.hpp"

class InvDocInfo: public DocInfo {
public: 
  friend class InvDocList;

  InvDocInfo() {};
  ~InvDocInfo() {}; 
  
  DOCID_T docID() const{ return id; };
  int termCount() const{ return count; };
 
protected:
  DOCID_T id;    // id of this doc
  int count; // count of this term in this doc
};

#endif
