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

