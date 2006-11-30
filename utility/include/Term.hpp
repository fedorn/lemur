/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */


#ifndef _TERM_HPP
#define _TERM_HPP
#include "Exception.hpp"
namespace lemur 
{
  namespace api
  {
    
    /// Basic term class
    /*! 
      This class represents a term in a document. 
    */

    class Term {
    public:
      Term() : termptr(NULL) {}
      virtual ~Term() { }
      /// return the spelling of the term (read-only)
      virtual const char* spelling() const { return termptr; }
      /// set the spelling of the term by pointing to the given parameter
      /// value at str must stay valid for this Term object to stay valid
      virtual void spelling(const char* str) { termptr = str; }
      /// set the spelling of the term by pointing to the given parameter
      /// value at str must stay valid for this Term object to stay valid
      virtual void spelling(const string& str) { termptr = str.c_str(); }
      /// reset this term representation
      virtual void reset() { termptr=NULL; }
    protected:
      const char* termptr;                      
    };
  }
}
#endif
