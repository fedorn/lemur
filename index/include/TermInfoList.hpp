#ifndef _TERMINFOLIST_HPP
#define _TERMINFOLIST_HPP

#include "Term.hpp"



//!  Abstract Representation of Term Information Entry 

/*! 

  This is an abstract class that represents the information associated
with each  term entry in the doc to term index. The basic information 
includes  the term ID and the frequency of the term.

*/

class TermInfo : public Term {
public:
  virtual ~TermInfo() {}
  
  /// term id
  virtual int id() =0;
  
  /// term count in the doc
  virtual int count()=0;
};


//!  Abstract Interface of Term Information List 

/*! 

  This is an abstract class that represents the term entries in doc index,
  i.e., the ID and frequency of all terms in a document.

*/

class TermInfoList {
public:
  virtual ~TermInfoList() {}

  /// prepare iteration
  virtual void startIteration()=0;

  /// has more entries
  virtual bool hasMore()=0;

  /// Get a pointer to the next entry (pointer to a local static memory)
  virtual TermInfo *nextEntry()=0;
};




#endif
