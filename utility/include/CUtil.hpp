#ifndef _CUTILH_
#define _CUTILH_

#include <String.hpp>
#include <ctype.h>

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
