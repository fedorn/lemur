#ifndef _MAP_H
#define _MAP_H

#include <map>
#include <stdlib.h>

struct ltstr
{
  bool operator()(char* s1, char* s2) const{
    return strcmp(s1, s2) < 0;
  }
};

#endif

#ifndef _NAMESPACE
#define _NAMESPACE

#ifdef _WIN32
using namespace std;
#endif

#endif

