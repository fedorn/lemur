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


#ifndef _DOCUMENT_HPP
#define _DOCUMENT_HPP

#include "Term.hpp"
#include "DocumentProps.hpp"

/// Interface of a TokenTerm -- a term in a doc stream

class TokenTerm : public Term {
public:

  /// return the spelling of the term (read-only)
  virtual const char *spelling() = 0;
};


/// Abstract document class
/*! 
  The class represents a document that is composed of a string (external) ID
and a sequence of tokens. Subclasses may add extra information such as title
and date.

*/

class Document {
public:
  virtual ~Document() {};
  
  /// returns the external string ID
  virtual char *getID() const =0;


  /// start term iteration
  virtual void startTermIteration()=0;

  /// returns true iff there's at least one term for iteration
  virtual bool hasMore() = 0;

  /// returns a pointer to next term (static memory, do not delete the returned instance). caller should check hasMore() before calling it
  virtual TokenTerm *nextTerm()=0;

  /// "fast forward" to the end of the document
  virtual void skipToEnd()=0;

  /// return the DocumentProps handle
  virtual DocumentProps *property() {
    return (props);
  }
private:
  DocumentProps *props;
};


#endif


  


