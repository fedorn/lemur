#include "String.hpp"

ostream &operator<<(ostream &os, const String &ts) 
{ os << ts.c_str(); return os; }

istream &operator>>(istream &is, String &ts) 
{ String tmp; is >> tmp; ts.assign(tmp); return is; }

