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


#ifdef __MK__
#pragma mklibrary(Class)
#endif

#pragma implementation

#ifndef _ALL_SOURCE
#define _ALL_SOURCE
#endif

#include <cstdio>
#include <cassert>
#include "common_headers.hpp"
#ifndef _WIN32
	#include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/wait.h>
//#include <sys/access.h>
#include <sys/stat.h>
//#include <sys/statfs.h>
//extern "C" int statfs(const char *, struct statfs *);
#include <cerrno>

#include <CUtil.hpp>

void CUError(const String &message, int soft)
{
  cerr << message << endl;
  if (errno > 0)
    {
      perror("Error");
      cerr << "errno=" << errno << endl;
    }
  if (soft)
    exit(-soft);
  else
    assert(0); /* Always fail and print out this file name */
}

void CUSleep(int time)
{
  int child_pid, status;

  if ((child_pid=fork()))
    {
    if((waitpid(child_pid,&status,WUNTRACED)!=child_pid) 
        || (WEXITSTATUS(status) > 0))
      CUError("restawhile: wait failed.");
    }
  else
    {
      execlp("sleep","sleep",time);
      exit(-1);
    }
}

// queries existence of file
int CUQFileF(const String &fname)
{
  return (access(fname, F_OK)==0);
}

// returns free storage in file system
// int CUFreeStorage(const String &fn) 
// {
//   struct statfs sfs;
//   if (statfs(fn, &sfs) == -1) return -1;
//   return sfs.f_bsize * sfs.f_bfree;
// }

// Return the size of file named filename
int CUFileSize(const String &filename)
{
  struct stat status;

  if (stat(filename,&status) != 0) return -1;
  return (int)status.st_size;
}

// returns x such that 2^(x-1) <= num < 2^x
// 1 -> 0
// 2 -> 1
// 3 -> 2
// 4 -> 2
int CULog2(int num)
{
  int i,x;
  assert(num > 0);
  for (i=1,x=0; i<num; x++,i <<= 1)
    ;
  return x;
}

// read the entire contents of a file
char *CUReadFile(const String &filename)
{
  ifstream ifs(filename);
  if (!ifs) return NULL;
  int size = CUFileSize(filename);
  if (size == -1)
    return NULL;
  char *buffer = new char[size+1];
  if (buffer == NULL)
    return NULL;

  ifs.read(buffer,size);
  buffer[size] = '\0';
  return buffer;
}
// read the entire contents of a file
int CULinesInFile(const String &filename)
{
  ifstream ifs(filename);
  if (!ifs) return -1;
  int numlines = 0;
  while (ifs.ignore(0x7FFFFFFF, '\n').good())
    numlines++;
  return numlines;
}
