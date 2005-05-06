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
      Term() { term=NULL; termptr=NULL;}
      virtual ~Term() { free(term); }
      /// return the spelling of the term (read-only)
      virtual const char* spelling() const { if (term) return term; return termptr; }
      /// set the spelling of the term by pointing to the given parameter
      /// value at str must stay valid for this Term object to stay valid
      virtual void spelling(const char* str) { free(term); termptr = str; }
      /// set the spelling of the term by pointing to the given parameter
      /// value at str must stay valid for this Term object to stay valid
      virtual void spelling(const string& str) { free(term); termptr = str.c_str(); }
      /// set the spelling of the term by making a copy
      virtual void copyspelling(const char* str) { reset(); term=strdup(str); }
      /// free the copy of the spelling of the term
      virtual void freespelling() { free(term); }
      /// reset this term representation
      virtual void reset() { free(term); termptr=NULL; }
    protected:
      char* term;
      const char* termptr;                      
    };
  }
}
#endif
