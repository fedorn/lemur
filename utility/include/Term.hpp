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


#ifndef _TERM_HPP
#define _TERM_HPP
#include "Exception.hpp"

/// Basic term class
/*! 
  This abstract class represents a term in a document. 
  It is currently empty and mainly serves as a high-level concept.
*/

class Term {
public:

  virtual ~Term() {};
  virtual const char* spelling() const { throw Exception("Term", "spelling() not supported by subclass");}
  virtual int count() const { throw Exception("Term", "count() not supported by subclass");}
  virtual int id() const { throw Exception("Term", "id() not supported by subclass");}
};


#endif



  


