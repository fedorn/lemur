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



  


