 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  
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
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef _PUSHINDEX_HPP
#define _PUSHINDEX_HPP

//! Abstract Class for push method of building an index

/*!

  This is an abstract class that provides a uniform interface for building an indexed document collection where the index is unaware of the parser (docstream).
*/

/*
 * NAME DATE - COMMENTS
 * tnt 03/01 - created
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


private:
};

#endif
