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
