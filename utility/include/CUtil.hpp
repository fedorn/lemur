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


#ifndef _CUTILH_
#define _CUTILH_

#include <String.hpp>
#include <cctype>

// If soft is zero, CUError assert's 0.
// Otherwise, CUError exits with error code -soft.
void CUError(const String &message, int soft=0);
void CUSleep(int time);
int CUQFileF(const String &fname);
int CUFileSize(const String &filename);
int CUFreeStorage(const String &fn);
int CULog2(int num);
char *CUReadFile(const String &filename);
int CULinesInFile(const String &filename);

inline void CUSkipWhitespace(char *&buffer)
{
  char c;
  char * p;
  for (p=buffer; (c = *p) != '\0'; p++) if (!isspace(c)) break;
  buffer = p;
}

inline void CUSkipToWhitespace(char *&buffer)
{
  char c;
  char * p;
  for (p=buffer; (c = *p) != '\0'; p++) if (isspace(c)) break;
  buffer = p;
}

#endif
