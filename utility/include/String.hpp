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

#ifndef _STRINGH_
#define _STRINGH_

#include <string>
#include "ByteHash.hpp"

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

#endif
