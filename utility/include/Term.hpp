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
  virtual const char* spelling() { throw Exception("Term", "spelling() not supported by subclass");}
  virtual int count() { throw Exception("Term", "count() not supported by subclass");}
  virtual int id() { throw Exception("Term", "id() not supported by subclass");}
};


#endif



  


