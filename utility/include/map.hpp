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

