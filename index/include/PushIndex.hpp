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


#ifndef _PUSHINDEX_HPP
#define _PUSHINDEX_HPP

//! Abstract Class for push method of building an index

/*!

  This is an abstract class that provides a uniform interface for building an indexed document collection where the index is unaware of the parser (docstream).
*/

/*
 * NAME DATE - COMMENTS
 * tnt 03/01 - created
 * tnt 07/02 - modified to add registering of docmgr
 *
 ======================================================================*/

#include "Term.hpp"
#include "DocumentProps.hpp"
#include "CollectionProps.hpp"

class PushIndex {
public:
  virtual ~PushIndex() {};
 
  /// the beginning of a new document, returns true if initiation was successful
  virtual bool beginDoc(DocumentProps* dp)=0;

  /// adding a term to the current document, returns true if term was added successfully.  
  virtual bool addTerm(Term& t)=0;

  /// signify the end of current document
  virtual void endDoc(DocumentProps* dp)=0;

  /// signify the end of this collection.  properties passed at the beginning of a collection should be handled by the constructor.
  virtual void endCollection(CollectionProps* cp)=0;

  virtual void setDocManager(const char* mgrID)=0;

private:
};

#endif
