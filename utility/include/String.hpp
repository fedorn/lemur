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


#ifndef _STRINGH_
#define _STRINGH_

#include "common_headers.hpp"
#include "ByteHash.hpp"
namespace lemur 
{
  namespace utility 
  {
    
    class String : public string {
    public:
      String() : string() {}
      String(const char * p) : string(p) {}
      String(const string & s) : string(s) {}
      String(unsigned int n, char c) : string(n, c) {}

      unsigned int Hash() const { return ByteHash(data(),length(),0); }
      unsigned int hash() const { return Hash(); } 
      operator const char *() const { return c_str(); }
      friend ostream &operator<<(ostream &os, const String &ts);
      friend istream &operator>>(istream &is, String &ts);
    };
  }
}

#endif
