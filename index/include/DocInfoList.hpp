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

#ifndef _DOCINFOLIST_HPP
#define _DOCINFOLIST_HPP




//!  Abstract Representation of Doc Information Entry 

/*! 

  This is an abstract class that represents the information associated
with each  doc entry in the term to doc index. The basic information 
includes  the doc ID and the frequency of a term in the document.

*/

class DocInfo {
public:
  virtual ~DocInfo() {}
  
  /// doc id
  virtual int docID() =0;
  
  /// term count in the doc
  virtual int termCount()=0;
};



//!  Abstract Interface of Doc Information List 

/*! 

  This is an abstract class that represents the doc entries in term index,
  i.e., the doc ID and the frequency of a term in the document.

*/

class DocInfoList {
public:
  virtual ~DocInfoList() {}
  
  /// prepare iteration
  virtual void startIteration()=0;

  /// test if there's any entry
  virtual bool hasMore()=0;

  /// fetch the next entry, return a pointer to a local static memory
  virtual DocInfo *nextEntry()=0;

};




#endif
